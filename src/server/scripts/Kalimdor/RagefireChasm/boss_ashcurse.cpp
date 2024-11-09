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
    SAY_AGGRO = 0,
    SAY_SLAY = 1,
    SAY_DEATH = 2,
    SAY_FIRE_STORM = 3,
    SAY_PHASE_2 = 4,
    SAY_SUMMON_FIRE_ELEMENTAL = 5,
    SAY_FIRE_JETS = 6,
};

enum Spells
{
    SPELL_FIREBALL = 98500,
    SPELL_FLAME_SPIKE = 98501,
    SPELL_FIRE_STORM = 98502,
    SPELL_BLAST_WAVE = 98503,
    SPELL_SUMMON_FIRE_ELEMENTAL = 98504,
    SPELL_FIRE_JETS = 98505,
    SPELL_FIRE_CHANNELING = 98510,
};

enum Phases
{
    PHASE_0 = 0,
    PHASE_1 = 1,
    PHASE_2 = 2,
};

struct boss_ashcurse : public BossAI
{
    boss_ashcurse(Creature* creature) : BossAI(creature, DATA_ASHCURSE) {
        instance = creature->GetInstanceScript();
    }

    InstanceScript* instance;

    void Reset() override
    {
        _Reset();
        DoCastSelf(SPELL_FIRE_CHANNELING, true);
    }

    void JustDied(Unit* /*who*/) override
    {
        me->GetMap()->SetZoneMusic(AREA_RAGEFIRE_CHASM, MUSIC_RAGEFIRE_CHASM_HEROIC);
        _JustDied();
        Talk(SAY_DEATH);
    }

    void KilledUnit(Unit* victim) override
    {
        if (roll_chance_i(50) && (victim->IsPlayer() || victim->IsNPCBot()))
        {
            Talk(SAY_SLAY);
        }
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        _JustEngagedWith();
        Talk(SAY_AGGRO);
        me->RemoveAurasDueToSpell(SPELL_FIRE_CHANNELING);

        scheduler.Schedule(0s, [this](TaskContext context)
        {
            context.SetGroup(PHASE_0);
            if (Unit* target = SelectTarget(SelectTargetMethod::Random, 0, 40.0f))
            {
                DoCast(target, SPELL_FIREBALL);
            }

            context.Repeat(6s, 10s);
        }).Schedule(10s, [this](TaskContext context)
        {
            context.SetGroup(PHASE_1);
            if (Unit* target = SelectTarget(SelectTargetMethod::MaxDistance, 0, 40.0f))
            {
                me->InterruptNonMeleeSpells(true);
                DoCast(target, SPELL_FLAME_SPIKE);
            }

            context.Repeat(20s, 25s);
        }).Schedule(20s, [this](TaskContext context)
        {
            context.SetGroup(PHASE_1);
            if (roll_chance_i(10))
            {
                Talk(SAY_FIRE_STORM);
            }

            if (Unit* target = SelectTarget(SelectTargetMethod::Random, 0, 40.0f))
            {
                me->InterruptNonMeleeSpells(true);
                DoCast(target, SPELL_FIRE_STORM);
            }

            context.Repeat(30s, 35s);
        });

        ScheduleHealthCheckEvent(60, [&] {
            scheduler.Schedule(0s, [this](TaskContext context)
            {
                context.SetGroup(PHASE_2);
                Talk(SAY_PHASE_2);
                scheduler.DelayGroup(PHASE_1, 37s);
                DoCastAOE(SPELL_BLAST_WAVE, true);
                scheduler.Schedule(1s, [this](TaskContext context)
                {
                    context.SetGroup(PHASE_0);
                    if (Unit* target = SelectTarget(SelectTargetMethod::MaxThreat, 0, 40.0f))
                    {
                        me->InterruptNonMeleeSpells(true);
                        DoCast(target, SPELL_FIREBALL);
                    }
                }).Schedule(10s, [this](TaskContext context)
                {
                    if (roll_chance_i(50))
                    {
                        Talk(SAY_SUMMON_FIRE_ELEMENTAL);
                    }
                    context.SetGroup(PHASE_2);
                    DoCastSelf(SPELL_SUMMON_FIRE_ELEMENTAL, true);
                }).Schedule(30s, [this](TaskContext context)
                {
                    if (roll_chance_i(50))
                    {
                        Talk(SAY_FIRE_JETS);
                    }
                    context.SetGroup(PHASE_2);
                    DoCastAOE(SPELL_FIRE_JETS);
                });
                context.Repeat(100s, 120s);
            });
        });
    }

    void DoAction(int32 action) override
    {
        if (action == ACTION_MAKE_ASHCURSE_VULNERABLE)
        {
            me->SetImmuneToAll(false);
            me->SetReactState(REACT_AGGRESSIVE);
            me->RemoveUnitFlag(UNIT_FLAG_NON_ATTACKABLE);
        }
        if (action == ACTION_MAKE_ASHCURSE_INVULNERABLE)
        {
            me->SetImmuneToAll(true);
            me->SetReactState(REACT_PASSIVE);
            me->SetUnitFlag(UNIT_FLAG_NON_ATTACKABLE);
        }
    }
};

void AddSC_boss_ashcurse()
{
    RegisterRagefireChasmCreatureAI(boss_ashcurse);
}
