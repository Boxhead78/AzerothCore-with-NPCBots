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

/*
Dun_Morogh
Quest support: 3628. Teleporter to Rise of the Defiler.
*/

#include "ScriptedCreature.h"
#include <VMapMgr2.h>

/*#####
# npc_aqualon
#####*/

enum AqualonSpells
{
    SPELL_FROST_ARMOR = 98730,
    SPELL_AQUA_NOVA = 98731,
    SPELL_WATER_BOLT = 98736,
};

struct npc_aqualon : public ScriptedAI
{
    npc_aqualon(Creature* creature) : ScriptedAI(creature) { }

    void JustEngagedWith(Unit* /*who*/) override
    {
        _scheduler.Schedule(0s, [this](TaskContext context)
        {
            if (Unit* target = SelectTarget(SelectTargetMethod::MaxThreat, 0, 40.0f))
            {
                DoCast(target, SPELL_WATER_BOLT);
            }
            context.Repeat(8s, 10s);
        }).Schedule(7s, [this](TaskContext context)
        {
            CastInFourDirections();
            context.Repeat(15s);
        });
    }

    void UpdateAI(uint32 diff) override
    {
        if (!me->HasAura(SPELL_FROST_ARMOR))
            DoCast(SPELL_FROST_ARMOR);

        if (!UpdateVictim())
            return;

        _scheduler.Update(diff);

        DoMeleeAttackIfReady();
    }

private:
    TaskScheduler _scheduler;

    void CastInFourDirections()
    {
        float baseAngle = me->GetOrientation();
        float increment = 5.0f;

        for (int i = 0; i < 4; ++i)
        {
            float angle = baseAngle + (M_PI / 2) * i;

            for (int j = 0; j < 5; ++j)
            {
                float currentDistance = increment * j;
                float x = me->GetPositionX() + currentDistance * cos(angle);
                float y = me->GetPositionY() + currentDistance * sin(angle);
                float z = me->GetMap()->GetGridHeight(x, y);
                if (z == VMAP_INVALID_HEIGHT_VALUE)
                    z = me->GetPositionZ();

                me->CastSpell(x, y, z, SPELL_AQUA_NOVA, true);
            }
        }
    }
};

void AddSC_dun_morogh()
{
    RegisterCreatureAI(npc_aqualon);
}
