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
#include "ScriptedCreature.h"
#include "World.h"

enum Texts
{
    SAY_AGGRO               = 0,
    SAY_EARTHQUAKE          = 1,
    SAY_OVERRUN             = 2,
    SAY_SLAY                = 3,
    SAY_DEATH               = 4
};

enum Spells
{
    SPELL_EARTHQUAKE        = 32686,
    SPELL_SUNDER_ARMOR      = 33661,
    SPELL_CHAIN_LIGHTNING   = 33665,
    SPELL_OVERRUN           = 32636,
    SPELL_ENRAGE            = 33653,
    SPELL_MARK_DEATH        = 37128,
    SPELL_AURA_DEATH        = 37131
};

struct boss_doomwalker : public ScriptedAI
{
    boss_doomwalker(Creature* creature) : ScriptedAI(creature) { }

    void Reset() override
    {
        _inEnrage = false;
        scheduler.CancelAll();
        me->setActive(true);
    }

    void KilledUnit(Unit* victim) override
    {
        victim->CastSpell(victim, SPELL_MARK_DEATH, 0);

        if (roll_chance_i(25) && (victim->IsPlayer() || victim->IsNPCBot()))
        {
            Talk(SAY_SLAY);
        }
    }

    void JustDied(Unit* /*killer*/) override
    {
        Talk(SAY_DEATH);
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        Talk(SAY_AGGRO);
        scheduler.Schedule(1ms, [this](TaskContext context)
        {
            if (!HealthAbovePct(20))
            {
                DoCastSelf(SPELL_ENRAGE);
                context.Repeat(6s);
                _inEnrage = true;
            }
        }).Schedule(5s, 13s, [this](TaskContext context)
        {
            DoCastVictim(SPELL_SUNDER_ARMOR);
            context.Repeat(10s, 25s);
        }).Schedule(10s, 30s, [this](TaskContext context)
        {
            DoCastRandomTarget(SPELL_CHAIN_LIGHTNING, 1);
            context.Repeat(7s, 27s);
        }).Schedule(25s, 35s, [this](TaskContext context)
        {
            if (urand(0, 1))
            {
                return;
            }
            Talk(SAY_EARTHQUAKE);
            if (_inEnrage) // avoid enrage + earthquake
            {
                me->RemoveAurasDueToSpell(SPELL_ENRAGE);
            }
            DoCastAOE(SPELL_EARTHQUAKE);
            context.Repeat(30s, 55s);
        }).Schedule(30s, 45s, [this](TaskContext context)
        {
            Talk(SAY_OVERRUN);
            DoCastVictim(SPELL_OVERRUN);
            context.Repeat(25s, 40s);
        });
    }

    void MoveInLineOfSight(Unit* who) override
    {
        if (who && who->IsPlayer() && me->IsValidAttackTarget(who))
        {
            if (who->HasAura(SPELL_MARK_DEATH) && !who->HasAura(27827)) // Spirit of Redemption
            {
                who->CastSpell(who, SPELL_AURA_DEATH, 1);
            }
        }
    }

    void UpdateAI(uint32 diff) override
    {
        if (sWorld->getBoolConfig(CONFIG_WORLD_BOSS_RANDOM_SHOUTS))
        {
            if (_checkTimer <= diff)
            {
                if (!me->IsInCombat())
                {
                    uint8 randInt = urand(0, 3);
                    switch (randInt)
                    {
                    case 0:
                        Talk(SAY_AGGRO);
                        break;
                    case 1:
                        Talk(SAY_EARTHQUAKE);
                        break;
                    case 2:
                        Talk(SAY_OVERRUN);
                        break;
                    case 3:
                        Talk(SAY_SLAY);
                        break;
                    }
                }
                _checkTimer = urand(5 * MINUTE * IN_MILLISECONDS, 15 * MINUTE * IN_MILLISECONDS);
            }
            else
            {
                _checkTimer -= diff;
            }
        }

        if (!UpdateVictim())
            return;

        scheduler.Update(diff);

        if (me->HasUnitState(UNIT_STATE_CASTING))
            return;

        DoMeleeAttackIfReady();
    }

private:
    bool _inEnrage;
    uint32 _checkTimer;
};

void AddSC_boss_doomwalker()
{
    RegisterCreatureAI(boss_doomwalker);
}
