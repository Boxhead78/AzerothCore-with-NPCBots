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

#include "CreatureScript.h"
#include "Player.h"
#include "ScriptedCreature.h"
#include "SpellScript.h"
#include "SpellScriptLoader.h"
#include "halls_of_stone.h"

enum spells
{
    GROUND_SPIKE_H              = 59750,
    BOULDER_TOSS                = 50843,
    BOULDER_TOSS_H              = 59742,
    SHATTER                     = 50810,
    SHATTER_H                   = 61546,
    STOMP                       = 50868,
    STOMP_H                     = 59744,
    GROUND_SLAM                 = 50827,
    GROUND_SLAM_STONED_EFFECT   = 50812,
    SPELL_SHATTER_EFFECT        = 50811,
};

enum events
{
    EVENT_NONE,
    EVENT_BOULDER,
    EVENT_STOMP,
    EVENT_GROUND_SLAM,
    EVENT_GROUND_SPIKE,
    EVENT_SHATTER,
    EVENT_REMOVE_STONED,
};

enum Yells
{
    SAY_AGGRO                                   = 0,
    SAY_KILL                                    = 1,
    SAY_DEATH                                   = 2,
    SAY_SHATTER                                 = 3
};

class boss_krystallus : public CreatureScript
{
public:
    boss_krystallus() : CreatureScript("boss_krystallus") { }

    CreatureAI* GetAI(Creature* pCreature) const override
    {
        return GetHallsOfStoneAI<boss_krystallusAI>(pCreature);
    }

    struct boss_krystallusAI : public ScriptedAI
    {
        boss_krystallusAI(Creature* c) : ScriptedAI(c)
        {
            pInstance = me->GetInstanceScript();
        }

        EventMap events;
        InstanceScript* pInstance;

        void Reset() override
        {
            events.Reset();
            if (pInstance)
                pInstance->SetData(BOSS_KRYSTALLUS, NOT_STARTED);
        }

        void JustEngagedWith(Unit*  /*who*/) override
        {
            events.Reset();
            events.RescheduleEvent(EVENT_BOULDER, 8s);
            //events.RescheduleEvent(EVENT_STOMP, 5s);
            events.RescheduleEvent(EVENT_GROUND_SLAM, 15s);
            if (me->GetMap()->IsHeroic())
                events.RescheduleEvent(EVENT_GROUND_SPIKE, 10s);

            if (pInstance)
                pInstance->SetData(BOSS_KRYSTALLUS, IN_PROGRESS);

            Talk(SAY_AGGRO);
        }

        void RemoveStonedEffect()
        {
            Map* map = me->GetMap();
            if (map->IsDungeon())
            {
                Map::PlayerList const& players = map->GetPlayers();
                for(Map::PlayerList::const_iterator itr = players.begin(); itr != players.end(); ++itr)
                    if (itr->GetSource()->IsAlive())
                        itr->GetSource()->RemoveAura(GROUND_SLAM_STONED_EFFECT);
            }
        }

        void UpdateAI(uint32 diff) override
        {
            if (!UpdateVictim())
                return;

            events.Update(diff);

            if (me->HasUnitState(UNIT_STATE_CASTING))
                return;

            switch (events.ExecuteEvent())
            {
                case EVENT_BOULDER:
                    {
                        if (Unit* target = SelectTarget(SelectTargetMethod::Random, 0, 50.0f, true, 0))
                            me->CastSpell(target, DUNGEON_MODE(BOULDER_TOSS, BOULDER_TOSS_H), false);

                        events.Repeat(5s, 7s);
                        break;
                    }
                case EVENT_GROUND_SPIKE:
                    {
                        me->CastSpell(me->GetVictim(), GROUND_SPIKE_H, false); // current enemy target
                        events.Repeat(8s, 11s);
                        break;
                    }
                case EVENT_STOMP:
                    {
                        me->CastSpell(me, DUNGEON_MODE(STOMP, STOMP_H), false);
                        //events.Repeat(13s, 18s);
                        break;
                    }
                case EVENT_GROUND_SLAM:
                    {
                        events.Repeat(20s, 23s);
                        //me->CastSpell(me->GetVictim(), GROUND_SLAM, true);
                        Talk(SAY_SHATTER);
                        events.DelayEvents(10s);
                        //events.RescheduleEvent(EVENT_SHATTER, 8s);
                        break;
                    }
                case EVENT_SHATTER:
                    {
                        me->CastSpell((Unit*)nullptr, SHATTER, false);
                        Talk(SAY_SHATTER);
                        events.RescheduleEvent(EVENT_REMOVE_STONED, 1500ms);
                        break;
                    }
                case EVENT_REMOVE_STONED:
                    {
                        RemoveStonedEffect();
                        break;
                    }
            }

            DoMeleeAttackIfReady();
        }

        void JustDied(Unit*  /*killer*/) override
        {
            Talk(SAY_DEATH);
            if (pInstance)
                pInstance->SetData(BOSS_KRYSTALLUS, DONE);
        }

        void KilledUnit(Unit* /*victim*/) override
        {
            Talk(SAY_KILL);
        }
    };
};

class spell_krystallus_shatter : public SpellScript
{
    PrepareSpellScript(spell_krystallus_shatter);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_SHATTER_EFFECT });
    }

    void HandleScript(SpellEffIndex /*effIndex*/)
    {
        if (Unit* target = GetHitUnit())
        {
            target->RemoveAurasDueToSpell(GROUND_SLAM_STONED_EFFECT);
            target->CastSpell((Unit*)nullptr, SPELL_SHATTER_EFFECT, true);
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_krystallus_shatter::HandleScript, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
    }
};

class spell_krystallus_shatter_effect : public SpellScript
{
    PrepareSpellScript(spell_krystallus_shatter_effect);

    void CalculateDamage()
    {
        if (!GetHitUnit())
            return;

        float radius = GetSpellInfo()->Effects[EFFECT_0].CalcRadius(GetCaster());
        if (!radius)
            return;

        float distance = GetCaster()->GetDistance2d(GetHitUnit());
        if (distance > 1.0f)
            SetHitDamage(int32(GetHitDamage() * ((radius - distance) / radius)));
    }

    void Register() override
    {
        OnHit += SpellHitFn(spell_krystallus_shatter_effect::CalculateDamage);
    }
};

void AddSC_boss_krystallus()
{
    new boss_krystallus();
    RegisterSpellScript(spell_krystallus_shatter);
    RegisterSpellScript(spell_krystallus_shatter_effect);
}
