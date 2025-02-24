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
Loch_Modan
*/

/* ContentData
npc_edrin
EndContentData */

#include "CreatureScript.h"
#include "Player.h"
#include "ScriptedCreature.h"
#include "ScriptedEscortAI.h"
#include "VMapMgr2.h"

/*######
# npc_edrin
######*/

enum Edrin
{
    EVENT_SPAWN_ENEMIES_1 = 1,
    EVENT_SPAWN_ENEMIES_2 = 2,

    SAY_EDR_CAREFUL = 0,
    SAY_EDR_START = 1,
    SAY_EDR_FREED = 2,

    GO_CAGE = 450015,

    NPC_EDRIN = 500184,
    NPC_STONESPLINTER_DEEPMAW = 500176,

    QUEST_A_HUNTER_IN_CHAINS = 50105,
};

class npc_edrin : public CreatureScript
{
public:
    npc_edrin() : CreatureScript("npc_edrin") { }

    bool OnQuestAccept(Player* player, Creature* creature, const Quest* quest) override
    {
        if (quest->GetQuestId() == QUEST_A_HUNTER_IN_CHAINS)
        {
            GameObject* object = GetClosestGameObjectWithEntry(creature, GO_CAGE, 10.0f);
            object->UseDoorOrButton(0, false);

            creature->SetFaction(FACTION_ESCORTEE_A_NEUTRAL_ACTIVE);
            creature->AI()->Talk(SAY_EDR_START, player);
            creature->RemoveUnitFlag(UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_PACIFIED);

            if (npc_edrinAI* pEscortAI = CAST_AI(npc_edrin::npc_edrinAI, creature->AI()))
                pEscortAI->Start(false, false, player->GetGUID(), quest);
        }
        return true;
    }

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_edrinAI(creature);
    }

    struct npc_edrinAI : public npc_escortAI
    {
        npc_edrinAI(Creature* creature) : npc_escortAI(creature) { }

        void Reset() override { }

        void WaypointReached(uint32 waypointId) override
        {
            Player* player = GetPlayerForEscort();
            if (!player)
                return;

            switch (waypointId)
            {
            case 6:
                Talk(SAY_EDR_CAREFUL, player);
                ScheduleUniqueTimedEvent(3s, [&]
                {
                    SpawnEnemies(me, 2);
                }, EVENT_SPAWN_ENEMIES_1);
                break;
            case 15:
                Talk(SAY_EDR_CAREFUL, player);
                ScheduleUniqueTimedEvent(3s, [&]
                {
                    SpawnEnemies(me, 3);
                }, EVENT_SPAWN_ENEMIES_2);
                break;
            case 40:
                Talk(SAY_EDR_FREED, player);
                player->GroupEventHappens(QUEST_A_HUNTER_IN_CHAINS, me);
                break;
            }
        }

        void JustRespawned() override
        {
            npc_escortAI::JustRespawned();
            GameObject* object = GetClosestGameObjectWithEntry(me, GO_CAGE, 10.0f);
            object->ResetDoorOrButton();
        }

        void UpdateAI(uint32 diff) override
        {
            npc_escortAI::UpdateAI(diff);
            scheduler.Update(diff);
        }

    private:
        void SpawnEnemies(Creature* me, uint8 amount)
        {
            uint8 spawnCount = amount;
            float angleOffset = 45.0f / (spawnCount - 1);
            float baseAngle = me->GetOrientation() - (45.0f / 2 * M_PI / 180.0f);

            for (uint8 i = 0; i < spawnCount; ++i)
            {
                float angle = baseAngle + (angleOffset * i * M_PI / 180.0f);
                float distance = 8.0f;
                float x, y, z;
                uint8 attempts = 9;

                while (attempts--)
                {
                    me->GetNearPoint2D(x, y, distance, angle);
                    z = me->GetMap()->GetHeight(me->GetPhaseMask(), x, y, me->GetPositionZ());

                    if (z != VMAP_INVALID_HEIGHT_VALUE)
                        break;
                }

                if (z == VMAP_INVALID_HEIGHT_VALUE)
                    continue;

                Creature* enemy = me->SummonCreature(NPC_STONESPLINTER_DEEPMAW, x, y, z, angle, TEMPSUMMON_TIMED_DESPAWN, 90000);
                if (enemy)
                {
                    enemy->GetMotionMaster()->Clear();
                    enemy->Attack(me, true);
                    enemy->SetReactState(REACT_AGGRESSIVE);
                }
            }
        }
    };
};

enum GorlogEnum
{
    SAY_AGGRO = 0,
    SAY_DEATH = 1,

    SPELL_MAGMA_BURST = 98808,
    SPELL_ENRAGE = 98809,
    SPELL_STONE_SHATTER = 98815,
};

struct npc_gorlog : public ScriptedAI
{
    npc_gorlog(Creature* creature) : ScriptedAI(creature) { }

    void JustDied(Unit* killer) override
    {
        Talk(SAY_DEATH);
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        Talk(SAY_AGGRO);
        _scheduler.Schedule(0s, [this](TaskContext context)
        {
            if (Unit* target = SelectTarget(SelectTargetMethod::MaxThreat, 0, 40.0f))
            {
                DoCast(target, SPELL_MAGMA_BURST);
            }
            context.Repeat(8s, 10s);
        }).Schedule(6s, [this](TaskContext context)
        {
            CastRandomStoneShatter();
            context.Repeat(17s);
        }).Schedule(1min, [this](TaskContext context)
        {
            DoCastSelf(SPELL_ENRAGE);
        });
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        _scheduler.Update(diff);

        DoMeleeAttackIfReady();
    }

private:
    TaskScheduler _scheduler;

    void CastRandomStoneShatter()
    {
        const uint8 numSpawns = urand(5, 7);
        float baseX = me->GetPositionX();
        float baseY = me->GetPositionY();
        float baseZ = me->GetPositionZ();

        if (Unit* target = me->SelectNearestPlayer(30.0f))
        {
            float x = target->GetPositionX();
            float y = target->GetPositionY();
            float z = target->GetPositionZ();
            me->CastSpell(x, y, z, SPELL_STONE_SHATTER, true);
        }

        for (uint8 i = 0; i < numSpawns - 1; ++i)
        {
            float angle = frand(0, 2 * M_PI);
            float distance = 3.0f + urand(0, 8);
            float x, y, z;
            uint8 attempts = 9;

            while (attempts--)
            {
                x = baseX + distance * cos(angle);
                y = baseY + distance * sin(angle);
                z = me->GetMap()->GetHeight(me->GetPhaseMask(), x, y, baseZ);

                if (z != VMAP_INVALID_HEIGHT_VALUE)
                    break;
            }

            if (z == VMAP_INVALID_HEIGHT_VALUE)
                continue;

            me->CastSpell(x, y, z, SPELL_STONE_SHATTER, true);
        }
    }


};

void AddSC_loch_modan()
{
    new npc_edrin();
    RegisterCreatureAI(npc_gorlog);
}
