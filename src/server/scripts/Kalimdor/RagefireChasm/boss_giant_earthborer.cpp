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

enum Misc
{
    ADDS_MIN_AMOUNT = 7,
    ADDS_MAX_AMOUNT = 9,
};

enum Say
{
    SAY_EAT_MEAT = 0,
};

enum Spells
{
    SPELL_CHARGE = 98549,
    SPELL_POISON_SPIT = 98550,
    SPELL_PUNCTURE_WOUND = 98551,
    SPELL_EAT_MEAT = 98552,
    SPELL_POISON_BREATH = 98553,
    SPELL_BERSERK = 98554,
};

enum Phases
{
    PHASE_0 = 0,
    PHASE_1 = 1,
    PHASE_2 = 2,
};

const Position MeatPos[2] =
{
    { -365.174f, 186.941f, -22.375f, 5.05534f },
    { -375.660f, 211.547f, -21.884f, 2.27729f },
};

const Position AddsSpawnPos[6] =
{
    { -327.626f, 205.526f, -20.085f, 4.95663f },
    { -312.426f, 216.721f, -23.157f, 6.06979f },
    { -311.112f, 228.428f, -23.007f, 0.82879f },
    { -338.082f, 225.797f, -20.224f, 3.41993f },
    { -363.515f, 221.305f, -21.409f, 2.99974f },
    { -351.685f, 184.721f, -21.773f, 4.70808f },
};

struct boss_giant_earthborer : public BossAI
{
    boss_giant_earthborer(Creature* creature) : BossAI(creature, DATA_GIANT_EARTHBORER) {
        instance = creature->GetInstanceScript();
        isInMeatPhase = false;
        eatCounter = 0;
    }

    InstanceScript* instance;
    bool isInMeatPhase;
    uint8 eatCounter;

    void Reset() override
    {
        _Reset();
        isInMeatPhase = false;
        eatCounter = 0;
        me->SetReactState(REACT_AGGRESSIVE);
        me->SetCombatMovement(true);
        me->GetMotionMaster()->Clear();
    }

    void JustDied(Unit* /*who*/) override
    {
        _JustDied();
        me->GetMap()->SetZoneMusic(AREA_RAGEFIRE_CHASM, MUSIC_RAGEFIRE_CHASM_HEROIC);
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        _JustEngagedWith();
        DoCastVictim(SPELL_CHARGE, true);
        scheduler.Schedule(5s, [this](TaskContext context)
        {
            context.SetGroup(PHASE_0);
            if (!isInMeatPhase) // Check in case meatPhase is active before first schedule
            {
                if (Unit* target = SelectTarget(SelectTargetMethod::Random, 0, 40.0f))
                {
                    DoCast(target, SPELL_POISON_SPIT);
                }
            }

            context.Repeat(15s, 20s);
        }).Schedule(8s, [this](TaskContext context)
        {
            context.SetGroup(PHASE_0);
            if (!isInMeatPhase) // Check in case meatPhase is active before first schedule
            {
                if (Unit* target = SelectTarget(SelectTargetMethod::MaxThreat, 0, 40.0f))
                {
                    DoCast(target, SPELL_PUNCTURE_WOUND);
                }
            }

            context.Repeat(40s, 50s);
        }).Schedule(45s, [this](TaskContext context)
        {
            context.SetGroup(PHASE_0);
            if (!isInMeatPhase) // Check in case meatPhase is active before first schedule
            {
                if (Unit* target = SelectTarget(SelectTargetMethod::MaxThreat, 0, 40.0f))
                {
                    DoCast(target, SPELL_POISON_BREATH);
                }
            }

            context.Repeat(30s, 30s);
        }).Schedule(5min, [this](TaskContext context)
        {
            context.SetGroup(PHASE_2);
            DoCastSelf(SPELL_BERSERK, true);
        });

        ScheduleHealthCheckEvent(55, [&] {
            scheduler.Schedule(0s, [this](TaskContext context)
            {
                context.SetGroup(PHASE_1);
                scheduler.DelayGroup(PHASE_0, 90s);

                // Setzt den Boss auf passiv und leert die Bedrohungsliste
                me->SetReactState(REACT_PASSIVE);
                DoResetThreatList();
                DoStopAttack();
                me->SetCombatMovement(false);

                // Boss bewegt sich zu einer zufälligen MeatPos-Position
                uint8 meatIndex = urand(0, std::size(MeatPos) - 1);
                me->GetMotionMaster()->Clear();
                me->GetMotionMaster()->MovePoint(1, MeatPos[meatIndex]);
                StartEatMeatPhase(meatIndex);

                Talk(SAY_EAT_MEAT);

                context.Repeat(2min, 3min);
            });
        });
    }

    void StartEatMeatPhase(uint8 meatIndex)
    {
        isInMeatPhase = true;
        scheduler.Schedule(5s, [this](TaskContext context)
        {
            context.SetGroup(PHASE_1);

            // Beginnt das regelmäßige Wirken von `SPELL_EAT_MEAT` und das Spawnen von Adds
            RepeatEatMeat();
            SpawnEarthborerAdds();
        });
    }

    void CastEatMeat()
    {
        if (Creature* bloodyMeat = me->FindNearestCreature(NPC_BLOODY_MEAT, 10.0f))
        {
            me->SetFacingToObject(bloodyMeat);
            me->CastSpell(bloodyMeat, SPELL_EAT_MEAT, true);
        }
    }

    void RepeatEatMeat()
    {
        scheduler.Schedule(0s, [this](TaskContext context)
        {
            context.SetGroup(PHASE_1);
            // Findet und castet `SPELL_EAT_MEAT` auf das nächstgelegene `NPC_BLOODY_MEAT`
            CastEatMeat();

            // Wiederholt den Zauber alle 3 Sekunden für 50 Sekunden
            if (++eatCounter < 16)
            {
                context.Repeat(3s); // Wiederholt nach 3 Sekunden
            }
            else if (eatCounter >= 16)
            {
                isInMeatPhase = false;

                // Nach 50 Sekunden: Boss wird aggressiv und führt `SPELL_CHARGE` auf den entferntesten Spieler aus
                me->SetReactState(REACT_AGGRESSIVE);
                me->SetCombatMovement(true);
                if (Unit* farthestPlayer = SelectTarget(SelectTargetMethod::MaxDistance, 0, 25.0f, true))
                {
                    DoCast(farthestPlayer, SPELL_CHARGE, true);
                    scheduler.Schedule(2s, [this, farthestPlayer](TaskContext context)
                    {
                        // Boss verfolgt den nächsten Spieler
                        me->GetMotionMaster()->Clear();
                        if (farthestPlayer && farthestPlayer->IsAlive())
                        {
                            me->GetMotionMaster()->MoveChase(farthestPlayer);
                        }
                        else
                        {
                            AttackNearestTarget();
                        }
                    });
                }
                else
                {
                    AttackNearestTarget();
                }
            }
        });
    }

    void SpawnEarthborerAdds()
    {
        uint8 addCount = urand(ADDS_MIN_AMOUNT, ADDS_MAX_AMOUNT); // Anzahl der Adds zwischen 4 und 5
        for (uint8 i = 0; i < addCount; ++i)
        {
            // Jede Iteration hat eine Verzögerung von `i * 1.5 Sekunden`
            auto delay = std::chrono::seconds(static_cast<int>(i * 1.5));
            scheduler.Schedule(delay, [this](TaskContext /*context*/)
            {
                uint8 spawnIndex = urand(0, std::size(AddsSpawnPos) - 1);
                if (Creature* earthborer = me->SummonCreature(NPC_EARTHBORER, AddsSpawnPos[spawnIndex], TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 30000))
                {
                    // Lässt das Earthborer-Add einen zufälligen Spieler angreifen
                    if (Unit* target = SelectTarget(SelectTargetMethod::Random))
                    {
                        earthborer->AI()->AttackStart(target);
                    }
                }
            });
        }
    }

    void AttackNearestTarget()
    {
        // Boss verfolgt den nächsten Spieler
        me->GetMotionMaster()->Clear();
        if (Unit* target = me->SelectNearestTarget())
        {
            me->GetMotionMaster()->MoveChase(target);
        }
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        scheduler.Update(diff);

        if (isInMeatPhase)
            return;

        DoMeleeAttackIfReady();
    }
};

void AddSC_boss_giant_earthborer()
{
    RegisterRagefireChasmCreatureAI(boss_giant_earthborer);
}
