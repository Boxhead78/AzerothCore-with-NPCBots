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

enum Say
{
    SAY_EARTHQUAKE = 0,
    SAY_SEARING_HEAT_1 = 1,
    SAY_SEARING_HEAT_2 = 2,
    SAY_SEARING_HEAT_3 = 3,
};

enum Spells
{
    SPELL_VIOLENT_EARTH = 98526,
    SPELL_EARTHS_GRIP = 98527,
    SPELL_SEARING_HEAT = 98529,
    SPELL_RAGING_FLAMES = 98530,
    SPELL_FIRE_SHIELD = 98531,
    SPELL_EARTHQUAKE = 98533,
};

enum Phases
{
    PHASE_0 = 0,
    PHASE_1 = 1,
    PHASE_2 = 2,
};

struct boss_emberfury : public BossAI
{
    boss_emberfury(Creature* creature) : BossAI(creature, DATA_EMBERFURY) {
        instance = creature->GetInstanceScript();
        doneTalk = 0;
    }

    InstanceScript* instance;
    uint8 doneTalk;

    void Reset() override
    {
        _Reset();
        doneTalk = 0;
    }

    void JustDied(Unit* /*who*/) override
    {
        _JustDied();
        me->GetMap()->SetZoneMusic(AREA_RAGEFIRE_CHASM, MUSIC_RAGEFIRE_CHASM_HEROIC);
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        _JustEngagedWith();
        scheduler.Schedule(0s, [this](TaskContext context)
        {
            context.SetGroup(PHASE_0);
            if (!me->HasAura(SPELL_FIRE_SHIELD))
            {
                DoCastSelf(SPELL_FIRE_SHIELD, true);
            }
            context.Repeat(35s, 40s);
        }).Schedule(30s, [this](TaskContext context)
        {
            context.SetGroup(PHASE_0);
            uint8 stackAmount = GetSearingHeatStackAmount();
            if (stackAmount >= 3 && doneTalk == 0)
            {
                doneTalk = 1;
                Talk(SAY_SEARING_HEAT_1);
            }
            else if (stackAmount >= 6 && doneTalk == 1)
            {
                doneTalk = 2;
                Talk(SAY_SEARING_HEAT_2);
            }
            else if (stackAmount >= 10 && doneTalk == 2)
            {
                doneTalk = 3;
                Talk(SAY_SEARING_HEAT_3);
            }
            DoCastSelf(SPELL_SEARING_HEAT, true);
            context.Repeat(30s);
        });

        scheduler.Schedule(30s, [this](TaskContext context)
        {
            context.SetGroup(PHASE_1);
            if (Unit* target = SelectTarget(SelectTargetMethod::Random, 0, 30.0f, true, false))
            {
                DoCast(target, SPELL_EARTHS_GRIP);
            }
            context.Repeat(40s, 45s);
        }).Schedule(60s, [this](TaskContext context)
        {
            context.SetGroup(PHASE_1);
            if (Unit* target = SelectTarget(SelectTargetMethod::MaxDistance, 0, 40.0f, true, false))
            {
                DoCast(target, SPELL_VIOLENT_EARTH);
            }
            context.Repeat(30s);
        });

        ScheduleHealthCheckEvent(60, [&] {
            scheduler.Schedule(0s, [this](TaskContext context)
            {
                scheduler.DelayGroup(PHASE_1, 30s);
                Talk(SAY_EARTHQUAKE);
                scheduler.Schedule(0s, [this](TaskContext context)
                {
                    context.SetGroup(PHASE_2);
                    DoCastSelf(SPELL_EARTHQUAKE, true);
                }).Schedule(30s, [this](TaskContext context)
                {
                    DoCastSelf(SPELL_RAGING_FLAMES, true);
                });
                context.Repeat(2min);
            });
        });
    }

    uint8 GetSearingHeatStackAmount()
    {
        uint8 stackAmount;
        Aura* searingHeat = me->GetAura(SPELL_SEARING_HEAT);
        if (searingHeat)
        {
            stackAmount = searingHeat->GetStackAmount();
        }
        else
        {
            stackAmount = 0;
        }

        return stackAmount;
    }
};

void AddSC_boss_emberfury()
{
    RegisterRagefireChasmCreatureAI(boss_emberfury);
}
