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
#include "SpellAuraEffects.h"
#include "SpellScript.h"
#include "SpellScriptLoader.h"
#include "unordered_set"

enum Misc
{
    ADDS_MIN_AMOUNT = 4,
    ADDS_MAX_AMOUNT = 5,
};

enum Say
{
    SAY_AGGRO = 0,
    SAY_SLAY = 1,
    SAY_DEATH = 2,
    SAY_PHASE_2 = 3,
    SAY_THUNDERSTRIKE = 4,
    SAY_UNSTOPPABLE_MARCH = 5,
    SAY_UNSTOPPABLE_MARCH_INFO = 6,
    SAY_CALL_OF_THE_DRAGONKIN = 7,
};

enum Spells
{
    // Vorthalax's Guard
    SPELL_TOUCH_OF_RESTORATION = 98555,
    SPELL_SPHERE_OF_PROTECTION = 98556,

    // Twilight Overseer
    SPELL_TWILIGHTFLAME = 98557,
    SPELL_SHADOW_NOVA = 98558,
    SPELL_SHADOW_NOVA_PERIODIC = 98559,
    SPELL_SHADOW_BOLT = 98572,

    // Commander Vorthalax
    SPELL_CRUSHING_BLOW = 98564,
    SPELL_THUNDERSTRIKE = 98565,
    SPELL_UNSTOPPABLE_MARCH = 98566,
    SPELL_UNSTOPPABLE_MARCH_NEARBY_TARGETS = 98567,
    SPELL_CALL_OF_THE_DRAGONKIN = 98568,
    SPELL_DRAGONS_RAGE = 98569,
    SPELL_SPELL_REFLECTION = 98570,

    // Misc
    SPELL_MARKER_ORANGE = 98561,
};

enum Phases
{
    PHASE_0 = 0,
    PHASE_1 = 1,
    PHASE_2 = 2,
    SCRIPTED = 9,
};

const Position vorthalaxGuardSpawnPos = { -383.077f, 152.542f, 7.738f, 2.74711f };
const Position twilightOverseerSpawnPos = { -393.920f, 141.864f, 7.7385f, 1.96166f };

ObjectGuid vorthalaxsGuardGUID;
ObjectGuid twilightOverseerGUID;

struct boss_commander_vorthalax : public BossAI
{
    boss_commander_vorthalax(Creature* creature) : BossAI(creature, DATA_COMMANDER_VORTHALAX) {
        instance = creature->GetInstanceScript();
    }

    InstanceScript* instance;

    void Reset() override
    {
        _Reset();
        Creature* vorthalaxsGuard = me->SummonCreature(NPC_VORTHALAXS_GUARD, vorthalaxGuardSpawnPos);
        Creature* twilightOverseer = me->SummonCreature(NPC_TWILIGHT_OVERSEER, twilightOverseerSpawnPos);
        if (vorthalaxsGuard)
        {
            vorthalaxsGuardGUID = vorthalaxsGuard->GetGUID();
            vorthalaxsGuard->SetReactState(REACT_PASSIVE);
        }
        if (twilightOverseer)
        {
            twilightOverseerGUID = twilightOverseer->GetGUID();
            twilightOverseer->SetReactState(REACT_PASSIVE);
        }
    }

    void EnterEvadeMode(EvadeReason why) override
    {
        if (Creature* vorthalaxsGuard = ObjectAccessor::GetCreature(*me, vorthalaxsGuardGUID))
            vorthalaxsGuard->DespawnOrUnsummon();

        if (Creature* twilightOverseer = ObjectAccessor::GetCreature(*me, twilightOverseerGUID))
            twilightOverseer->DespawnOrUnsummon();

        BossAI::EnterEvadeMode(why);
    }

    void JustDied(Unit* /*who*/) override
    {
        _JustDied();
        me->GetMap()->SetZoneMusic(AREA_RAGEFIRE_CHASM, MUSIC_RAGEFIRE_CHASM_HEROIC);
        Talk(SAY_DEATH);
    }

    void KilledUnit(Unit* victim) override
    {
        if (roll_chance_i(50) && (victim->IsPlayer() || victim->IsNPCBot()))
            Talk(SAY_SLAY);
    }

    void JustEngagedWith(Unit* who) override
    {
        _JustEngagedWith();
        Talk(SAY_AGGRO);

        if (Creature* vorthalaxsGuard = ObjectAccessor::GetCreature(*me, vorthalaxsGuardGUID))
        {
            vorthalaxsGuard->SetReactState(REACT_AGGRESSIVE);
            vorthalaxsGuard->AI()->AttackStart(who);
        }

        if (Creature* twilightOverseer = ObjectAccessor::GetCreature(*me, twilightOverseerGUID))
        {
            twilightOverseer->SetReactState(REACT_AGGRESSIVE);
            twilightOverseer->AI()->AttackStart(who);
        }

        //PHASE_1 (PHASE_0 is before 85%)
        ScheduleHealthCheckEvent(85, [&] {
            scheduler.Schedule(0s, [this](TaskContext context)
            {
                context.SetGroup(PHASE_1);
                scheduler.Schedule(0s, [this](TaskContext context)
                {
                    Talk(SAY_THUNDERSTRIKE);
                    context.SetGroup(PHASE_1);
                    DoCastSelf(SPELL_THUNDERSTRIKE);
                }).Schedule(10s, [this](TaskContext context)
                {
                    context.SetGroup(PHASE_1);
                    if (Unit* target = SelectTarget(SelectTargetMethod::MaxThreat, 0, 40.0f, true))
                    {
                        DoCast(target, SPELL_CRUSHING_BLOW);
                    }
                }).Schedule(20s, [this](TaskContext context)
                {
                    context.SetGroup(PHASE_1);
                    DoCastSelf(SPELL_THUNDERSTRIKE);
                }).Schedule(40s, [this](TaskContext context)
                {
                    Talk(SAY_UNSTOPPABLE_MARCH);
                    context.SetGroup(PHASE_1);
                    ScheduleUnstoppableMarchSequence();
                });
                context.Repeat(1min);
            });
        });

        //PHASE_2
        ScheduleHealthCheckEvent(30, [&] {
            Talk(SAY_PHASE_2);
            scheduler.CancelGroup(PHASE_1);
            scheduler.Schedule(0s, [this](TaskContext context)
            {
                Talk(SAY_CALL_OF_THE_DRAGONKIN);
                context.SetGroup(PHASE_2);
                DoCastSelf(SPELL_CALL_OF_THE_DRAGONKIN);
                context.Repeat(45s, 60s);
            }).Schedule(0s, [this](TaskContext context)
            {
                context.SetGroup(PHASE_2);
                if (GetDragonsRageStackAmount() < 100)
                {
                    DoCastSelf(SPELL_DRAGONS_RAGE);
                }
                context.Repeat(3s);
            }).Schedule(20s, [this](TaskContext context)
            {
                context.SetGroup(PHASE_2);
                if (!me->HasAura(SPELL_SPELL_REFLECTION))
                    DoCastSelf(SPELL_SPELL_REFLECTION);
                context.Repeat(10s,15s);
            });
        });
    }

    void ScheduleUnstoppableMarchSequence()
    {
        if (Unit* target = SelectTarget(SelectTargetMethod::MaxDistance, 0, 40.0f, true))
        {
            // Schritt 1: Markiere das Ziel mit SPELL_MARKER_ORANGE
            DoCast(target, SPELL_MARKER_ORANGE);
            Talk(SAY_UNSTOPPABLE_MARCH);
            Talk(SAY_UNSTOPPABLE_MARCH_INFO, target);

            // Schritt 2: Setze das Ziel als Fokus und stoppe den Angriff
            me->AttackStop();
            me->SetReactState(REACT_PASSIVE);

            // Schritt 3: Warte 4 Sekunden, dann führe SPELL_UNSTOPPABLE_MARCH aus und setze den Angriff fort
            scheduler.Schedule(0s, [this, target](TaskContext context)
            {
                context.SetGroup(SCRIPTED);

                // Prüfen, ob das Ziel noch vorhanden und gültig ist
                if (!target || !target->IsInWorld() || !target->IsAlive())
                {
                    return;
                }

                // Boss lässt das Ziel im Fokus und verfolgt es, indem er sich kontinuierlich neu ausrichtet
                me->SetFacingToObject(target);

                // Wiederhole die Ausrichtung regelmäßig, um dem Ziel weiter zu folgen
                context.Repeat(10ms);
            });

            scheduler.Schedule(4s, [this, target](TaskContext)
            {
                scheduler.CancelGroup(SCRIPTED);
                me->SetReactState(REACT_AGGRESSIVE);

                // Überprüfen, ob das Ziel noch vorhanden und gültig ist
                if (!target || !target->IsInWorld() || !target->IsAlive())
                {
                    return;
                }

                DoCast(target, SPELL_UNSTOPPABLE_MARCH);
            });
        }
    }

    uint8 GetDragonsRageStackAmount()
    {
        uint8 stackAmount;
        Aura* dragonsRage = me->GetAura(SPELL_DRAGONS_RAGE);
        if (dragonsRage)
        {
            stackAmount = dragonsRage->GetStackAmount();
        }
        else
        {
            stackAmount = 0;
        }

        return stackAmount;
    }
};

struct npc_vorthalaxs_guard : public ScriptedAI
{
    npc_vorthalaxs_guard(Creature* creature) : ScriptedAI(creature)
    {
        instance = creature->GetInstanceScript();
    }

    InstanceScript* instance;

    void Reset() override
    {
        ScriptedAI::Reset();
        scheduler.CancelAll();
    }

    void DamageTaken(Unit* who, uint32& damage, DamageEffectType, SpellSchoolMask) override
    {
        if (me->GetReactState() == REACT_PASSIVE)
        {
            if (Creature* vorthalax = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_COMMANDER_VORTHALAX)))
            {
                vorthalax->AI()->AttackStart(who);
            }
        }
    }

    void JustEngagedWith(Unit* who) override
    {
        scheduler.Schedule(5s, [this](TaskContext context)
        {
            if (Unit* target = DoSelectLowestHpFriendly(40.0f, 50000))
            {
                DoCast(target, SPELL_TOUCH_OF_RESTORATION);
            }
            context.Repeat(7s, 10s);
        }).Schedule(15s, [this](TaskContext context)
        {
            if (Creature* vorthalax = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_COMMANDER_VORTHALAX)))
                if (vorthalax->GetHealthPct() <= 50.0f)
                {
                    DoCast(vorthalax, SPELL_SPHERE_OF_PROTECTION);
                }
            else if (Unit* target = DoSelectLowestHpFriendly(40.0f, 0))
            {
                if (target->GetHealthPct() <= 50.0f)
                {
                    DoCast(target, SPELL_SPHERE_OF_PROTECTION);
                }
            }
            context.Repeat(5s, 15s);
        });
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
        {
            return;
        }

        scheduler.Update(diff);

        if (me->HasUnitState(UNIT_STATE_CASTING))
        {
            return;
        }
        DoMeleeAttackIfReady();
    }
};

struct npc_twilight_overseer : public ScriptedAI
{
    npc_twilight_overseer(Creature* creature) : ScriptedAI(creature)
    {
        instance = creature->GetInstanceScript();
        stopShadowBolt = false;
    }

    InstanceScript* instance;
    bool stopShadowBolt;

    void Reset() override
    {
        ScriptedAI::Reset();
        scheduler.CancelAll();
        me->SetCombatMovement(false);
        stopShadowBolt = false;
    }

    void DamageTaken(Unit* who, uint32& damage, DamageEffectType, SpellSchoolMask) override
    {
        if (me->GetReactState() == REACT_PASSIVE)
        {
            if (Creature* vorthalax = ObjectAccessor::GetCreature(*me, instance->GetGuidData(DATA_COMMANDER_VORTHALAX)))
            {
                vorthalax->AI()->AttackStart(who);
            }
        }
    }

    void JustEngagedWith(Unit* who) override
    {
        scheduler.Schedule(2s, [this](TaskContext context)
        {
            if (!stopShadowBolt)
            {
                if (Unit* target = SelectTarget(SelectTargetMethod::Random, 0, 50.0f, true))
                {
                    DoCast(target, SPELL_SHADOW_BOLT);
                }
            }
            context.Repeat(4s, 5s);
        }).Schedule(15s, [this](TaskContext context)
        {
            if (Unit* target = SelectTarget(SelectTargetMethod::MaxThreat, 0, 50.0f, true))
            {
                me->InterruptNonMeleeSpells(true);
                DoCast(target, SPELL_TWILIGHTFLAME);
            }
            scheduler.Schedule(10s, [this](TaskContext context)
            {
                if (Unit* target = SelectTarget(SelectTargetMethod::Random, 0, 50.0f))
                {
                    me->InterruptNonMeleeSpells(true);
                    stopShadowBolt = true;
                    MoveNearRandomPlayerAndCast(target);
                }
            }).Schedule(25s, [this](TaskContext context)
            {
                if (Unit* target = SelectTarget(SelectTargetMethod::Random, 0, 50.0f))
                {
                    me->InterruptNonMeleeSpells(true);
                    stopShadowBolt = true;
                    MoveNearRandomPlayerAndCast(target);
                }
            });
            context.Repeat(1min);
        });
    }

    void MovementInform(uint32 type, uint32 id) override
    {
        if (type == POINT_MOTION_TYPE && id == 1) // Überprüfen, ob das Bewegungsziel erreicht ist
        {
            // Fähigkeit zaubern, nachdem der NPC das Ziel erreicht hat
            DoCastAOE(SPELL_SHADOW_NOVA);
        }
        stopShadowBolt = false;
    }

    void MoveNearRandomPlayerAndCast(Unit* who)
    {
        if (!who || !who->IsAlive())
        {
            stopShadowBolt = false;
            return;
        }

        // NPC bewegt sich zu der Position in der Nähe des Spielers
        me->GetMotionMaster()->MovePoint(1, who->GetNearPosition(3.0f, frand(0.0f, 2 * M_PI)));
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
        {
            return;
        }

        scheduler.Update(diff);

        if (!me->IsCombatMovementAllowed() && me->GetPowerPct(POWER_MANA) < 5.0f)
        {
            me->SetCombatMovement(true);
        }

        if (me->HasUnitState(UNIT_STATE_CASTING))
        {
            return;
        }


        DoMeleeAttackIfReady();
    }
};

class spell_call_of_the_dragonkin : public SpellScript
{
    PrepareSpellScript(spell_call_of_the_dragonkin);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_CALL_OF_THE_DRAGONKIN });
    }

    void HandleOnCast()
    {
        Unit* caster = GetCaster();
        if (!caster)
            return;

        uint8 numWhelps = urand(ADDS_MIN_AMOUNT, ADDS_MAX_AMOUNT); // Anzahl der zu spawnenden Whelps (4 oder 5)
        float radius = 4.0f; // Abstand vom Boss
        float angleStep = 2 * M_PI / numWhelps; // Winkel zwischen den Spawns im Kreis

        for (uint8 i = 0; i < numWhelps; ++i)
        {
            // Berechne die Position im Kreis
            float angle = i * angleStep;
            float x = caster->GetPositionX() + radius * cos(angle);
            float y = caster->GetPositionY() + radius * sin(angle);
            float z = caster->GetPositionZ();

            // Spawne das Whelp an der berechneten Position
            caster->SummonCreature(NPC_TWILIGHT_WHELP, x, y, z, caster->GetOrientation(), TEMPSUMMON_TIMED_DESPAWN, 300000); // 300 Sekunden Despawn-Zeit
        }
    }

    void Register() override
    {
        OnCast += SpellCastFn(spell_call_of_the_dragonkin::HandleOnCast);
    }
};

class spell_unstoppable_march : public AuraScript
{
    PrepareAuraScript(spell_unstoppable_march);

    std::unordered_set<uint64> hitDamageTargets; // Liste für Schaden

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_UNSTOPPABLE_MARCH, SPELL_UNSTOPPABLE_MARCH_NEARBY_TARGETS });
    }

    void HandlePeriodic(AuraEffect const* aurEff)
    {
        Unit* caster = GetCaster();
        if (!caster)
            return;

        // Radius des Effekts
        float radius = 5.0f;
        std::list<Unit*> targetList;

        // Ziele im Umkreis des Zaubernden finden
        Acore::AnyUnfriendlyUnitInObjectRangeCheck u_check(caster, caster, radius);
        Acore::UnitListSearcher<Acore::AnyUnfriendlyUnitInObjectRangeCheck> searcher(caster, targetList, u_check);
        Cell::VisitAllObjects(caster, searcher, radius);

        for (Unit* target : targetList)
        {
            uint64 targetGUID = target->GetGUID().GetRawValue();

            // Schaden nur einmal pro Ziel
            if (hitDamageTargets.find(targetGUID) == hitDamageTargets.end())
            {
                caster->CastSpell(target, SPELL_UNSTOPPABLE_MARCH_NEARBY_TARGETS, true); // Schaden
                hitDamageTargets.insert(targetGUID); // Ziel als getroffen markieren für Schaden
            }
        }
    }

    void Register() override
    {
        // Periodischer Effekt
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_unstoppable_march::HandlePeriodic, EFFECT_2, SPELL_AURA_PERIODIC_DUMMY);
    }
};

class spell_shadow_nova_no_self_damage : public SpellScript
{
    PrepareSpellScript(spell_shadow_nova_no_self_damage);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_SHADOW_NOVA, SPELL_SHADOW_NOVA_PERIODIC });
    }

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        // Entferne den Caster aus der Ziel-Liste
        targets.remove_if([this](WorldObject* target) -> bool
            {
                return target == GetCaster();
            });
    }

    void HandleDamage(SpellEffIndex /*effIndex*/)
    {
        // Nur Schaden ausführen, wenn der Caster nicht das einzige Ziel ist
        if (GetHitUnit() == GetCaster())
        {
            // Schaden für den Caster verhindern
            PreventHitDamage();
        }
    }

    void Register() override
    {
        // Filtere die Ziele, um den Caster zu entfernen
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_shadow_nova_no_self_damage::FilterTargets, EFFECT_0, TARGET_UNIT_SRC_AREA_ENEMY);

        // HandleDamage aufrufen, um zu verhindern, dass der Caster Schaden nimmt
        OnEffectHitTarget += SpellEffectFn(spell_shadow_nova_no_self_damage::HandleDamage, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
    }
};

void AddSC_boss_commander_vorthalax()
{
    RegisterRagefireChasmCreatureAI(boss_commander_vorthalax);
    RegisterRagefireChasmCreatureAI(npc_vorthalaxs_guard);
    RegisterRagefireChasmCreatureAI(npc_twilight_overseer);
    RegisterSpellScript(spell_unstoppable_march);
    RegisterSpellScript(spell_call_of_the_dragonkin);
    RegisterSpellScript(spell_shadow_nova_no_self_damage);
}
