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

/* ScriptData
SDName: Ashenvale
SD%Complete: 70
SDComment: Quest support: 6544, 6482
SDCategory: Ashenvale Forest
EndScriptData */

#include "CreatureScript.h"
#include "GameEventMgr.h"
#include "Player.h"
#include "ScriptedCreature.h"
#include "ScriptedEscortAI.h"

enum ElendiladEnum
{
    SAY_AGGRO = 0,
    SAY_DEATH = 1,

    SPELL_JUMP = 98875,
    SPELL_BLADESTORM = 98897,
    SPELL_COMMANDING_SHOUT = 98891,
};

struct npc_elendilad : public ScriptedAI
{
    npc_elendilad(Creature* creature) : ScriptedAI(creature) { }

    void JustDied(Unit* killer) override
    {
        Talk(SAY_DEATH);
    }

    void JustEngagedWith(Unit* /*who*/) override
    {
        _scheduler.Schedule(0s, [this](TaskContext context)
        {
            if (Unit* target = SelectTarget(SelectTargetMethod::MaxThreat, 0, 40.0f))
            {
                DoCast(target, SPELL_BLADESTORM);
            }
            context.Repeat(20s, 20s);
        }).Schedule(6s, [this](TaskContext context)
        {
            DoCastSelf(SPELL_COMMANDING_SHOUT);
        });
    }

    void Reset() override
    {
        _scheduler.CancelAll();
    }

    void UpdateAI(uint32 diff) override
    {
        if (!intro)
        {
            Talk(SAY_AGGRO);
            intro = true;
        }

        if (!jumped)
        {
            if (jumpTimer <= diff)
            {
                me->SetImmuneToNPC(false);
                me->SetImmuneToPC(false);
                DoCastSelf(SPELL_JUMP, true);
                jumped = true;
            }
            else
            {
                jumpTimer -= diff;
            }
        }

        if (!UpdateVictim())
            return;

        _scheduler.Update(diff);

        DoMeleeAttackIfReady();
    }


private:
    bool intro = false;
    bool jumped = false;
    uint32 jumpTimer = 4000;
    TaskScheduler _scheduler;
};

/*######
# npc_gorat_ghost
######*/

enum GoratGhost
{
    EVENT_DIALOGUE_1 = 1,
    EVENT_DIALOGUE_2 = 2,
    EVENT_DIALOGUE_3 = 3,
    EVENT_ELE = 4,

    SAY_GOR_START = 0,
    SAY_GOR_START_1 = 1,
    SAY_GOR_START_2 = 2,
    SAY_GOR_START_3 = 3,
    SAY_GOR_SHOW_YOURSELF = 4,

    SAY_ELE_SPAWN = 0,

    NPC_ELENDILAD = 500234,
    NPC_GORAT = 500252,
};

Position const ElendiladSpawnPos = { 1516.69f, -2142.63f,  88.72f, 1.8668f};

class npc_gorat_ghost : public CreatureScript
{
public:
    npc_gorat_ghost() : CreatureScript("npc_gorat_ghost") { }

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_gorat_ghostAI(creature);
    }

    struct npc_gorat_ghostAI : public npc_escortAI
    {
        npc_gorat_ghostAI(Creature* creature) : npc_escortAI(creature) { }

        void Reset() override { }

        void WaypointReached(uint32 waypointId) override
        {
            switch (waypointId)
            {
            case 2:
                Talk(SAY_GOR_START_1);
                ScheduleUniqueTimedEvent(10s, [&]
                {
                    Talk(SAY_GOR_START_2);
                }, EVENT_DIALOGUE_1);
                ScheduleUniqueTimedEvent(20s, [&]
                {
                    Talk(SAY_GOR_START_3);
                }, EVENT_DIALOGUE_2);
                break;
            case 16:
                me->SetFacingTo(5.0045f);
                ScheduleUniqueTimedEvent(2s, [&]
                {
                    Talk(SAY_GOR_SHOW_YOURSELF);
                    me->HandleEmoteCommand(15);
                }, EVENT_DIALOGUE_3);
                ScheduleUniqueTimedEvent(9s, [&]
                {
                    me->SummonCreature(NPC_ELENDILAD, ElendiladSpawnPos, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 90000);
                }, EVENT_ELE);
                break;
            }
        }

        void AttackStart(Unit* who) override
        {
            if (!intro)
                return;

            npc_escortAI::AttackStart(who);
        }

        void UpdateAI(uint32 diff) override
        {
            if (!intro)
            {
                if (npc_gorat_ghostAI* pEscortAI = CAST_AI(npc_gorat_ghost::npc_gorat_ghostAI, me->AI()))
                    pEscortAI->Start(false, false);

                me->SetFaction(FACTION_ESCORTEE_H_NEUTRAL_PASSIVE);
                me->AI()->Talk(SAY_GOR_START);
                me->RemoveUnitFlag(UNIT_FLAG_NON_ATTACKABLE | UNIT_FLAG_PACIFIED);
                intro = true;
            }

            npc_escortAI::UpdateAI(diff);
            scheduler.Update(diff);
        }

    private:
        bool intro = false;
    };
};

enum Muglash
{
    SAY_MUG_START1          = 0,
    SAY_MUG_START2          = 1,
    SAY_MUG_BRAZIER         = 2,
    SAY_MUG_BRAZIER_WAIT    = 3,
    SAY_MUG_ON_GUARD        = 4,
    SAY_MUG_REST            = 5,
    SAY_MUG_DONE            = 6,
    SAY_MUG_GRATITUDE       = 7,
    SAY_MUG_PATROL          = 8,
    SAY_MUG_RETURN          = 9,

    QUEST_VORSHA            = 6641,

    GO_NAGA_BRAZIER         = 178247,

    NPC_WRATH_RIDER         = 3713,
    NPC_WRATH_SORCERESS     = 3717,
    NPC_WRATH_RAZORTAIL     = 3712,

    NPC_WRATH_PRIESTESS     = 3944,
    NPC_WRATH_MYRMIDON      = 3711,
    NPC_WRATH_SEAWITCH      = 3715,

    NPC_VORSHA              = 12940,
    NPC_MUGLASH             = 12717,

    ACTION_EXTINGUISH_BLAZIER = 0
};

Position const FirstNagaCoord[3] =
{
    { 3603.504150f, 1122.631104f,  1.635f, 0.0f },        // rider
    { 3589.293945f, 1148.664063f,  5.565f, 0.0f },        // sorceress
    { 3609.925537f, 1168.759521f, -1.168f, 0.0f }         // razortail
};

Position const SecondNagaCoord[3] =
{
    { 3609.925537f, 1168.759521f, -1.168f, 0.0f },        // witch
    { 3645.652100f, 1139.425415f, 1.322f,  0.0f },        // priest
    { 3583.602051f, 1128.405762f, 2.347f,  0.0f }         // myrmidon
};

Position const VorshaCoord = {3633.056885f, 1172.924072f, -5.388f, 0.0f};

class npc_muglash : public CreatureScript
{
public:
    npc_muglash() : CreatureScript("npc_muglash") { }

    struct npc_muglashAI : public npc_escortAI
    {
        npc_muglashAI(Creature* creature) : npc_escortAI(creature) { }

        void Reset() override
        {
            eventTimer = 10000;
            waveId = 0;
            _isBrazierExtinguished = false;
        }

        void DoAction(int32 actionId) override
        {
            if (actionId == ACTION_EXTINGUISH_BLAZIER)
            {
                Talk(SAY_MUG_BRAZIER_WAIT);
                _isBrazierExtinguished = true;
            }
        }

        void JustEngagedWith(Unit* /*who*/) override
        {
            if (Player* player = GetPlayerForEscort())
                if (HasEscortState(STATE_ESCORT_PAUSED))
                {
                    if (urand(0, 1))
                        Talk(SAY_MUG_ON_GUARD, player);
                    return;
                }
        }

        void JustDied(Unit* /*killer*/) override
        {
            if (HasEscortState(STATE_ESCORT_ESCORTING))
                if (Player* player = GetPlayerForEscort())
                    player->FailQuest(QUEST_VORSHA);
        }

        void JustSummoned(Creature* summoned) override
        {
            summoned->AI()->AttackStart(me);
        }

        void sQuestAccept(Player* player, Quest const* quest) override
        {
            if (quest->GetQuestId() == QUEST_VORSHA)
            {
                Talk(SAY_MUG_START1);
                me->SetFaction(FACTION_ESCORTEE_N_NEUTRAL_PASSIVE);
                npc_escortAI::Start(true, false, player->GetGUID());
            }
        }

        void WaypointReached(uint32 waypointId) override
        {
            if (Player* player = GetPlayerForEscort())
            {
                switch (waypointId)
                {
                    case 0:
                        Talk(SAY_MUG_START2, player);
                        break;
                    case 24:
                        Talk(SAY_MUG_BRAZIER, player);

                        if (GameObject* go = GetClosestGameObjectWithEntry(me, GO_NAGA_BRAZIER, INTERACTION_DISTANCE * 2))
                        {
                            go->RemoveGameObjectFlag(GO_FLAG_NOT_SELECTABLE);
                            SetEscortPaused(true);
                        }
                        break;
                    case 25:
                        Talk(SAY_MUG_GRATITUDE);
                        player->GroupEventHappens(QUEST_VORSHA, me);
                        break;
                    case 26:
                        Talk(SAY_MUG_PATROL);
                        break;
                    case 27:
                        Talk(SAY_MUG_RETURN);
                        break;
                }
            }
        }

        void DoWaveSummon()
        {
            switch (waveId)
            {
                case 1:
                    me->SummonCreature(NPC_WRATH_RIDER,     FirstNagaCoord[0], TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 60000);
                    me->SummonCreature(NPC_WRATH_SORCERESS, FirstNagaCoord[1], TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 60000);
                    me->SummonCreature(NPC_WRATH_RAZORTAIL, FirstNagaCoord[2], TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 60000);
                    break;
                case 2:
                    me->SummonCreature(NPC_WRATH_PRIESTESS, SecondNagaCoord[0], TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 60000);
                    me->SummonCreature(NPC_WRATH_MYRMIDON,  SecondNagaCoord[1], TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 60000);
                    me->SummonCreature(NPC_WRATH_SEAWITCH,  SecondNagaCoord[2], TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 60000);
                    break;
                case 3:
                    me->SummonCreature(NPC_VORSHA, VorshaCoord, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 60000);
                    break;
                case 4:
                    SetEscortPaused(false);
                    Talk(SAY_MUG_DONE);
                    break;
            }
        }

        void UpdateAI(uint32 diff) override
        {
            npc_escortAI::UpdateAI(diff);

            if (!me->GetVictim())
            {
                if (HasEscortState(STATE_ESCORT_PAUSED) && _isBrazierExtinguished)
                {
                    if (eventTimer < diff)
                    {
                        ++waveId;
                        DoWaveSummon();
                        eventTimer = 10000;
                    }
                    else
                        eventTimer -= diff;
                }
                return;
            }
            DoMeleeAttackIfReady();
        }

    private:
        uint32 eventTimer;
        uint8  waveId;
        bool   _isBrazierExtinguished;
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_muglashAI(creature);
    }
};

enum SentinelThenysil
{
    PHASE_NORMAL = 1,
    PHASE_BLASTRANAAR = 2,

    QUEST_ASTRANAARS_BURNING = 50145,
    QUEST_RETURN_FIRE = 50146,
};

class npc_sentinel_thenysil : public CreatureScript
{
public:
    npc_sentinel_thenysil() : CreatureScript("npc_sentinel_thenysil") { }

    bool OnQuestReward(Player* player, Creature* creature, Quest const* quest, uint32 opt) override
    {
        if (!player)
            return false;

        if (player->GetQuestRewardStatus(QUEST_ASTRANAARS_BURNING) &&
            quest->GetQuestId() == QUEST_RETURN_FIRE)
        {
            player->SetPhaseMask(PHASE_NORMAL, true);
        }

        return true;
    }

};

enum Twidel
{ 
    TAXI_ROUTE = 1993,

    QUEST_BLASTRANAAR = 50142,
};

class npc_twidel : public CreatureScript
{
public:
    npc_twidel() : CreatureScript("npc_twidel") { }

    void CheckAndResetPhase(Player* player)
    {
        if (!player || !player->IsInWorld())
            return;

        if (!player->IsInFlight())
        {
            player->SetPhaseMask(PHASE_NORMAL, true);
        }
        else
        {
            player->m_Events.AddEventAtOffset([this, player]() { this->CheckAndResetPhase(player); }, 1s);
        }
    }

    void StartTaxiFlight(Player* player)
    {
        if (player->GetPhaseMask() != PHASE_BLASTRANAAR)
        {
            player->SetPhaseMask(PHASE_BLASTRANAAR, true);
        }

        player->ActivateTaxiPathTo(TAXI_ROUTE);

        CheckAndResetPhase(player);
    }

    bool OnGossipSelect(Player* player, Creature* creature, uint32 /*sender*/, uint32 action) override
    {
        ClearGossipMenuFor(player);

        if (action == 1)
        {
            CloseGossipMenuFor(player);
            StartTaxiFlight(player);
        }

        return true;
    }

    bool OnQuestAccept(Player* player, Creature* creature, Quest const* quest) override
    {
        if (quest->GetQuestId() == QUEST_BLASTRANAAR)
            StartTaxiFlight(player);

        return true;
    }
};

void AddSC_ashenvale()
{
    new npc_muglash();
    new npc_gorat_ghost();
    new npc_twidel();
    new npc_sentinel_thenysil();
    RegisterCreatureAI(npc_elendilad);
}
