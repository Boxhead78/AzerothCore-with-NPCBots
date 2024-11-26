/*
 * This file is part of the AzerothCore Project. See AUTHORS file for Copyright information
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Affero General Public License as published by the
 * Free Software Foundation; either version 3 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "ragefire_chasm.h"
#include "ScriptedCreature.h"

enum FlameWaveModes
{
    OPPOSITE = 0,
    FULL = 1
};

enum MISC
{
    HEAT_GROUND_MIN_DURATION = 45000,
    HEAT_GROUND_MAX_DURATION = 60000,
};

enum Say
{
    SAY_AGGRO = 0,
    SAY_SLAY = 1,
    SAY_DEATH = 2,
    SAY_PYROCLAST_BARRAGE = 3,
    SAY_SUMMON_FLAME_WAVES = 4,
};

enum Spells
{
    SPELL_FLAME_WAVE_DAMAGE_AURA = 98537,
    SPELL_GREATER_FIREBALL = 98540,
    SPELL_FLAME_BUFFET = 98541,
    SPELL_SUMMON_FLAME_WAVES = 98542,
    SPELL_HEAT_GROUND = 98543,
    SPELL_PYROCLAST_BARRAGE = 98544,
};

enum Phases
{
    PHASE_0 = 0,
    PHASE_1 = 1,
    PHASE_2 = 2,
};

const Position FlameWaveSummonPos[4] =
{
    { -237.716f, 152.129f, -18.820f, 0.01329f },
    { -243.697f, 157.520f, -18.710f, 1.29939f },
    { -248.602f, 151.600f, -18.779f, 3.23933f },
    { -244.146f, 147.386f, -18.686f, 4.29963f },
};

const Position FlameWaveTargetPos[4] =
{
    { -201.638f, 154.883f, -25.258f, 0.10361f },
    { -233.510f, 194.693f, -24.865f, 1.30332f },
    { -283.840f, 148.145f, -25.430f, 3.23933f },
    { -261.294f, 108.229f, -25.034f, 4.29963f },
};

struct boss_baron_ignatius : public BossAI
{
    boss_baron_ignatius(Creature* creature) : BossAI(creature, DATA_BARON_IGNATIUS) {
        instance = creature->GetInstanceScript();
        moveToHome = false;
    }

    InstanceScript* instance;
    bool moveToHome;

    void Reset() override
    {
        _Reset();
        me->SetReactState(REACT_AGGRESSIVE);
        moveToHome = false;
    }

    void JustDied(Unit* /*who*/) override
    {
        _JustDied();
        Talk(SAY_DEATH);
    }

    void KilledUnit(Unit* victim) override
    {
        if (roll_chance_i(50) && (victim->IsPlayer() || victim->IsNPCBot()))
            Talk(SAY_SLAY);
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        _JustEngagedWith();
        Talk(SAY_AGGRO);

        scheduler.Schedule(0s, [this](TaskContext context)
        {
            context.SetGroup(PHASE_0);
            if (Unit* target = SelectTarget(SelectTargetMethod::MaxThreat, 0, 40.0f))
            {
                DoCast(target, SPELL_GREATER_FIREBALL);
            }
            context.Repeat(4s, 6s);
        }).Schedule(10s, [this](TaskContext context)
        {
            context.SetGroup(PHASE_0);
            if (Unit* target = SelectTarget(SelectTargetMethod::Random, 0, 40.0f))
            {
                DoCast(target, SPELL_FLAME_BUFFET);
            }
            context.Repeat(15s, 20s);
        }).Schedule(1min, [this](TaskContext context)
        {
            context.SetGroup(PHASE_1);
            Talk(SAY_PYROCLAST_BARRAGE);
            if (Unit* target = SelectTarget(SelectTargetMethod::MaxThreat, 0, 40.0f))
            {
                DoCast(target, SPELL_PYROCLAST_BARRAGE);
            }
            context.Repeat(30s, 45s);
        });

        ScheduleHealthCheckEvent(85, [&] {
            scheduler.Schedule(0s, [this](TaskContext context)
            {
                context.SetGroup(PHASE_1);
                if (Unit* target = SelectTarget(SelectTargetMethod::Random, 0, 40.0f))
                {
                    DoCast(target, SPELL_HEAT_GROUND);
                }
                context.Repeat(15s);
            });
        });

        ScheduleHealthCheckEvent(60, [&] {
            scheduler.Schedule(0s, [this](TaskContext context)
            {
                context.SetGroup(PHASE_2);
                scheduler.DelayGroup(PHASE_0, 18s);
                scheduler.DelayGroup(PHASE_1, 18s);
                Talk(SAY_SUMMON_FLAME_WAVES);
                moveToHome = true; // Markierung, dass Boss zur Home-Position zurückkehren soll
                me->SetReactState(REACT_PASSIVE);
                me->GetMotionMaster()->MovePoint(POINT_HOME, me->GetHomePosition().GetPositionX(), me->GetHomePosition().GetPositionY(), me->GetHomePosition().GetPositionZ(), true, true);
                context.Repeat(60s, 75s);
            });
        });
    }

    void MovementInform(uint32 type, uint32 id) override
    {
        if (type == POINT_MOTION_TYPE && id == POINT_HOME && moveToHome)
        {
            moveToHome = false; // Markierung zurücksetzen
            // Führt den Zauber erst aus, wenn der Boss die Home-Position erreicht hat
            DoCastSelf(SPELL_SUMMON_FLAME_WAVES);
            if (me->GetHealthPct() >= 50)
            {
                HandleFlameWaves(OPPOSITE);
            }
            else
            {
                HandleFlameWaves(FULL);
            }
            me->GetMotionMaster()->Clear(); // Bewegungsliste zurücksetzen
            me->SetReactState(REACT_AGGRESSIVE);
            me->AI()->AttackStart(me->GetVictim());
        }
    }

private:
    void HandleFlameWaves(uint8 waveMode)
    {
        switch (waveMode)
        {
        case OPPOSITE:
        {
            uint8 indexStart = urand(0, 1);

            for (uint8 i = 0; i < 2; ++i)
            {
                uint8 spawnIndex = indexStart;
                if (i == 1)
                {
                    spawnIndex += 2; // Set it to the opposite of the first wave
                }
                if (Creature* flameWave = me->SummonCreature(NPC_FLAME_WAVE, FlameWaveSummonPos[spawnIndex], TEMPSUMMON_TIMED_DESPAWN, 12000))
                {
                    flameWave->SetObjectScale(0.2f); // Startgröße 20 %
                    flameWave->AI()->SetData(1, spawnIndex); // Startet die Skalierung und Bewegung nach 3 Sekunden
                }
            }
            break;
        }
        case FULL:
        {
            for (uint8 i = 0; i < 4; ++i)
            {
                if (Creature* flameWave = me->SummonCreature(NPC_FLAME_WAVE, FlameWaveSummonPos[i], TEMPSUMMON_TIMED_DESPAWN, 12000))
                {
                    flameWave->SetObjectScale(0.2f);
                    flameWave->AI()->SetData(1, i); // Startet die Skalierung und Bewegung nach 3 Sekunden
                }
            }
            break;
        }
        default:
            break;
        }
    }
};

struct npc_heat_ground_controller : public ScriptedAI
{
    npc_heat_ground_controller(Creature* creature) : ScriptedAI(creature) {
        despawnTimer = 0;
    }

    uint32 despawnTimer;

    void Reset() override
    {
        despawnTimer = urand(HEAT_GROUND_MIN_DURATION, HEAT_GROUND_MAX_DURATION);
    }

    void UpdateAI(uint32 diff) override
    {
        if (despawnTimer <= diff)
        {
            me->DespawnOrUnsummon();
        }
        else
        {
            despawnTimer -= diff;
        }
    }
};

struct npc_flame_wave_controller : public ScriptedAI
{
    npc_flame_wave_controller(Creature* creature) : ScriptedAI(creature), scaleTimer(1250), moveTimer(0), despawnTimer(0), targetIndex(0) { }

    void SetData(uint32 type, uint32 data) override
    {
        if (type == 1)
        {
            scaleTimer = 1250; // Setze Skalierungs-Delay auf 1.25 Sekunden
            targetIndex = data; // Zielposition für Bewegung festlegen
        }
    }

    void UpdateAI(uint32 diff) override
    {
        if (me->GetReactState() != REACT_PASSIVE)
            me->SetReactState(REACT_PASSIVE);

        ScriptedAI::UpdateAI(diff);

        // Skalierungs- und Schadensaktivierungstimer
        if (scaleTimer > 0)
        {
            if (scaleTimer <= diff)
            {
                me->SetObjectScale(0.8f); // Volle Größe erreicht
                me->AddAura(SPELL_FLAME_WAVE_DAMAGE_AURA, me); // Schaden aktivieren
                moveTimer = 1500; // Startet 1.5 Sekunden Verzögerung für die Bewegung zum Zielort
                scaleTimer = 0; // Timer zurücksetzen
            }
            else
            {
                scaleTimer -= diff;
            }
        }

        // Bewegungs-Logik: Beginne die Bewegung nach der Verzögerung
        if (moveTimer > 0)
        {
            if (moveTimer <= diff)
            {
                StartMoving(); // Bewegung zum Ziel starten
                moveTimer = 0; // Timer zurücksetzen
            }
            else
            {
                moveTimer -= diff;
            }
        }

        // Despawn-Logik: Wenn der Timer aktiv ist, Countdown bis zum Despawn
        if (despawnTimer > 0)
        {
            if (despawnTimer <= diff)
            {
                me->DespawnOrUnsummon(); // Entferne NPC
            }
            else
            {
                despawnTimer -= diff;
            }
        }
    }

    void MovementInform(uint32 type, uint32 id) override
    {
        // Überprüfe, ob die Bewegung zum Ziel abgeschlossen ist
        if (type == POINT_MOTION_TYPE && id == targetIndex)
        {
            // NPC erreicht Zielort: Schrumpft auf 0.2 und setzt den Despawn-Timer
            me->SetObjectScale(0.2f); // Reduziere Größe
            despawnTimer = 3000; // Setze 3-Sekunden-Timer für den Despawn nach dem Schrumpfen
        }
    }

private:
    uint32 scaleTimer;
    uint32 moveTimer; // Timer für die Bewegung zum Zielort nach der Skalierung
    uint32 despawnTimer;
    uint8 targetIndex;

    void StartMoving()
    {
        if (targetIndex < 4)
        {
            me->GetMotionMaster()->MovePoint(targetIndex, FlameWaveTargetPos[targetIndex]);
        }
    }
};

void AddSC_boss_baron_ignatius()
{
    RegisterRagefireChasmCreatureAI(boss_baron_ignatius);
    RegisterRagefireChasmCreatureAI(npc_heat_ground_controller);
    RegisterRagefireChasmCreatureAI(npc_flame_wave_controller);
}
