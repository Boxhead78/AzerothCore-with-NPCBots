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
                me->HandleEmoteCommand(4);
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
            if (!me || amount == 0)
                return;

            float baseDistance = 6.0f;
            float maxDistance = 12.0f;
            float spreadAngle =  M_PI / 6;

            for (uint8 i = 0; i < amount; ++i)
            {
                float x, y, z;
                float spawnAngle = frand(-spreadAngle, spreadAngle); ;

                if (!me->GetClosePoint(x, y, z, 0.5f, frand(baseDistance, maxDistance), spawnAngle, nullptr, true))
                    continue;

                if (Creature* enemy = me->SummonCreature(NPC_STONESPLINTER_DEEPMAW, x, y, z, me->GetOrientation(), TEMPSUMMON_TIMED_DESPAWN, 90000))
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

    void Reset() override
    {
        _scheduler.CancelAll();
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

        // Falls ein Spieler in der Nähe ist, wirke den ersten Zauber auf ihn
        if (Unit* target = me->SelectNearestPlayer(30.0f))
        {
            float x, y, z;
            if (target->GetClosePoint(x, y, z, 0.5f, 1.0f, 0.0f, me, true))
                me->CastSpell(x, y, z, SPELL_STONE_SHATTER, true);
        }

        // Erzeuge zufällige Spawns um den NPC herum
        for (uint8 i = 1; i < numSpawns; ++i)
        {
            float x, y, z;
            float angle = frand(0, 2 * M_PI);
            float distance = frand(3.0f, 11.0f); // 3.0f + urand(0, 8) entspricht frand(3.0f, 11.0f)

            // GetClosePoint sorgt für eine sichere Position in der Nähe des NPCs
            if (me->GetClosePoint(x, y, z, 0.5f, distance, angle, me, true))
            {
                me->CastSpell(x, y, z, SPELL_STONE_SHATTER, true);
            }
        }
    }

};

void AddSC_loch_modan()
{
    new npc_edrin();
    RegisterCreatureAI(npc_gorlog);
}
