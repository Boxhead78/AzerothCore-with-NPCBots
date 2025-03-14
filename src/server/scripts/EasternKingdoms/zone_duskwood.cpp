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

#include "AreaTriggerScript.h"
#include "CreatureScript.h"
#include "Player.h"
#include "ScriptedCreature.h"
#include "ScriptedEscortAI.h"
#include "TaskScheduler.h"
#include "VMapMgr2.h"

enum Spells
{
    SPELL_SOUL_CORRUPTION           = 25805,
    SPELL_CREATURE_OF_NIGHTMARE     = 25806,
    SPELL_SWELL_OF_SOULS            = 21307,
};

enum Misc
{
    ITEM_FRAGMENT                   = 21149,
    NPC_TWILIGHT_CORRUPTER          = 15625
};

enum Say
{
    SAY_RESPAWN = 0,
    SAY_AGGRO,
    SAY_KILL
};

/*######
# boss_twilight_corrupter
# npc_aria_nightclaw
# npc_silithian_hatchling
######*/

struct boss_twilight_corrupter : public ScriptedAI
{
    boss_twilight_corrupter(Creature* creature) : ScriptedAI(creature) { }

    void Reset() override
    {
        _scheduler.CancelAll();
        me->RemoveAurasDueToSpell(SPELL_SWELL_OF_SOULS);
    }

    void InitializeAI() override
    {
        // Xinef: check if copy is summoned
        std::list<Creature*> cList;
        me->GetCreatureListWithEntryInGrid(cList, me->GetEntry(), 50.0f);
        for (Creature* creature : cList)
        {
            if (creature->IsAlive() && me->GetGUID() != creature->GetGUID())
            {
                me->DespawnOrUnsummon(1);
                break;
            }
        }

        _introSpoken = false;
        ScriptedAI::InitializeAI();
    }

    void MoveInLineOfSight(Unit* who) override
    {
        if (!_introSpoken && who->IsPlayer())
        {
            _introSpoken = true;
            Talk(SAY_RESPAWN, who);
            me->SetFaction(FACTION_MONSTER);
        }
        ScriptedAI::MoveInLineOfSight(who);
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        Talk(SAY_AGGRO);
        _scheduler
            .Schedule(12s, 18s, [this](TaskContext context)
            {
                DoCastRandomTarget(SPELL_CREATURE_OF_NIGHTMARE, 1, 100.f);
                context.Repeat(35s, 45s);
            })
            .Schedule(9s, 16s, [this](TaskContext context)
            {
                DoCastVictim(SPELL_SOUL_CORRUPTION);
                context.Repeat(5s, 9s);
            });
    }

    void KilledUnit(Unit* victim) override
    {
        if (victim->IsPlayer())
        {
            Talk(SAY_KILL, victim);
            DoCastSelf(SPELL_SWELL_OF_SOULS);
        }
    }

    void UpdateAI(uint32 diff) override
    {
        if (!UpdateVictim())
            return;

        _scheduler.Update(diff, [this]
            {
                DoMeleeAttackIfReady();
            });
    }

private:
    bool _introSpoken;
    TaskScheduler _scheduler;
};

/*######
# at_twilight_grove
######*/

class at_twilight_grove : public AreaTriggerScript
{
public:
    at_twilight_grove() : AreaTriggerScript("at_twilight_grove") { }

    bool OnTrigger(Player* player, const AreaTrigger* /*at*/) override
    {
        if (player->HasQuestForItem(ITEM_FRAGMENT) && !player->HasItemCount(ITEM_FRAGMENT))
            player->SummonCreature(NPC_TWILIGHT_CORRUPTER, -10328.16f, -489.57f, 49.95f, 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 240000);

        return false;
    };
};

/*######
# npc_aria_nightclaw
######*/

enum AriaNightclaw
{
    EVENT_1 = 1,
    EVENT_2 = 2,
    EVENT_3 = 3,
    EVENT_4 = 4,

    SAY_ARI_CAREFUL = 0,
    SAY_ARI_START = 1,
    SAY_ARI_FREED = 2,
    SAY_ARI_FOUND = 3,

    NPC_ARIA_NIGHTCLAW = 500287,
    NPC_LURKING_WORGEN = 500288,

    QUEST_RESCUE_FROM_THE_UNKNOWN = 50181,
};

class npc_aria_nightclaw : public CreatureScript
{
public:
    npc_aria_nightclaw() : CreatureScript("npc_aria_nightclaw") { }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action) override
    {
        ClearGossipMenuFor(player);

        if (action == 1)
        {
            CloseGossipMenuFor(player);

            if (Quest const* quest = sObjectMgr->GetQuestTemplate(QUEST_RESCUE_FROM_THE_UNKNOWN))
            {
                if (player->GetQuestStatus(QUEST_RESCUE_FROM_THE_UNKNOWN) == QUEST_STATUS_INCOMPLETE)
                {
                    creature->SetUInt32Value(UNIT_NPC_EMOTESTATE, 0);
                    creature->SetFaction(FACTION_ESCORTEE_A_NEUTRAL_ACTIVE);
                    creature->AI()->Talk(SAY_ARI_START, player);
                    creature->RemoveUnitFlag(UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_PACIFIED);
                    creature->RemoveNpcFlag(UNIT_NPC_FLAG_GOSSIP);

                    if (npc_aria_nightclaw::npc_aria_nightclawAI* pEscortAI = CAST_AI(npc_aria_nightclaw::npc_aria_nightclawAI, creature->AI()))
                        pEscortAI->Start(false, false, player->GetGUID(), quest);
                }
            }
        }
        return true;
    }

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_aria_nightclawAI(creature);
    }

    struct npc_aria_nightclawAI : public npc_escortAI
    {
        npc_aria_nightclawAI(Creature* creature) : npc_escortAI(creature) { }

        void Reset() override { }

        void WaypointReached(uint32 waypointId) override
        {
            Player* player = GetPlayerForEscort();
            if (!player)
                return;

            switch (waypointId)
            {
            case 15:
                Talk(SAY_ARI_CAREFUL, player);
                ScheduleUniqueTimedEvent(3s, [&]
                    {
                        SpawnEnemies(me, 2);
                    }, EVENT_1);
                break;
            case 22:
                me->SetStandState(UNIT_STAND_STATE_KNEEL);
                ScheduleUniqueTimedEvent(4s, [&]
                    {
                        me->SetStandState(UNIT_STAND_STATE_STAND);
                        if (Player* player = me->SelectNearestPlayer(10.0f))
                        {
                            Talk(SAY_ARI_FOUND, player);
                            me->SetFacingToObject(player);
                        }
                    }, EVENT_4);
                break;
            case 25:
                Talk(SAY_ARI_CAREFUL, player);
                ScheduleUniqueTimedEvent(3s, [&]
                    {
                        SpawnEnemies(me, 3);
                    }, EVENT_2);
                break;
            case 35:
                Talk(SAY_ARI_CAREFUL, player);
                ScheduleUniqueTimedEvent(3s, [&]
                    {
                        SpawnEnemies(me, 3);
                    }, EVENT_3);
                break;
            case 40:
                Talk(SAY_ARI_FREED, player);
                me->HandleEmoteCommand(4);
                player->GroupEventHappens(QUEST_RESCUE_FROM_THE_UNKNOWN, me);
                break;
            }
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

            float baseDistance = 12.0f;
            float maxDistance = 15.0f;
            float spreadAngle =  M_PI / 6;

            for (uint8 i = 0; i < amount; ++i)
            {
                float x, y, z;
                float spawnAngle = frand(-spreadAngle, spreadAngle); ;

                if (!me->GetClosePoint(x, y, z, 0.5f, frand(baseDistance, maxDistance), spawnAngle, nullptr, true))
                    continue;

                if (Creature* enemy = me->SummonCreature(NPC_LURKING_WORGEN, x, y, z, me->GetOrientation(), TEMPSUMMON_TIMED_DESPAWN, 90000))
                {
                    enemy->GetMotionMaster()->Clear();
                    enemy->Attack(me, true);
                    enemy->SetReactState(REACT_AGGRESSIVE);
                }
            }
        }
    };
};

static const std::array<Position, 5> HatchlingRandomEscapes = {
    Position{-10376.38f, -290.20f, 46.19f, 0.0f},
    Position{-10371.48f, -304.18f, 48.12f, 0.0f},
    Position{-10384.11f, -319.58f, 50.13f, 0.0f},
    Position{-10395.90f, -307.98f, 47.79f, 0.0f},
    Position{-10401.85f, -297.14f, 48.18f, 0.0f},
};

struct npc_silithian_hatchling : public ScriptedAI
{
    npc_silithian_hatchling(Creature* creature) : ScriptedAI(creature) { }

    void InitializeAI() override
    {
        ScriptedAI::InitializeAI();
        _scheduler.Schedule(100ms, [this](TaskContext /*context*/)
        {
            SearchAndAttack();
        });

        _scheduler.Schedule(7s, 10s, [this](TaskContext /*context*/)
        {
            MoveToRandomPositionAndDespawn();
        });
    }

    void SearchAndAttack()
    {
        if (Unit* target = me->SelectNearestTarget(40.0f))
        {
            AttackStart(target);
        }
        else
        {
            MoveToRandomPositionAndDespawn();
        }
    }

    void MoveToRandomPositionAndDespawn()
    {
        if (!_evadeStarted)
        {
            const Position& targetPos = Acore::Containers::SelectRandomContainerElement(HatchlingRandomEscapes);
            me->GetMotionMaster()->Clear();
            me->GetMotionMaster()->MovePoint(1, targetPos);

            _scheduler.Schedule(3s, [this](TaskContext /*context*/)
            {
                me->DespawnOrUnsummon(0);
            });
        }
    }

    void Reset() override
    {
        _evadeStarted = false;
        _scheduler.CancelAll();
    }

    void UpdateAI(uint32 diff) override
    {
        _scheduler.Update(diff);

        if (!UpdateVictim())
            return;

        DoMeleeAttackIfReady();
    }

private:
    bool _evadeStarted;
    TaskScheduler _scheduler;
};

void AddSC_duskwood()
{
    RegisterCreatureAI(boss_twilight_corrupter);
    new at_twilight_grove();
    new npc_aria_nightclaw();
    RegisterCreatureAI(npc_silithian_hatchling);
}
