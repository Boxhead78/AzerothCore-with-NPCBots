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

#include "InstanceMapScript.h"
#include "InstanceScript.h"
#include "ragefire_chasm.h"
#include "ScriptedCreature.h"

BossBoundaryData const boundaries =
{
    { DATA_EMBERFURY, new CircleBoundary(Position(-150.654f, 30.720f, -38.837f), 25.0) },
    { DATA_BARON_IGNATIUS, new CircleBoundary(Position(-243.894f, 152.212f, -18.725f), 60.0) },
    { DATA_GIANT_EARTHBORER, new CircleBoundary(Position(-358.826f, 202.494f, -22.290f), 50.0) },
    { DATA_COMMANDER_VORTHALAX, new CircleBoundary(Position(-392.352f, 146.620f, 7.738f), 50.0) }
};

DoorData const doorData[] =
{
    {GO_FLAME_WALL,     DATA_ASHCURSE,              DOOR_TYPE_PASSAGE },
};

enum Spells
{
    SPELL_EGG_HATCH = 98539,
};

class instance_ragefire_chasm : public InstanceMapScript
{
public:
    instance_ragefire_chasm() : InstanceMapScript("instance_ragefire_chasm", 389) { }

    InstanceScript* GetInstanceScript(InstanceMap* map) const override
    {
        return new instance_ragefire_chasm_InstanceMapScript(map);
    }

    struct instance_ragefire_chasm_InstanceMapScript : public InstanceScript
    {
        instance_ragefire_chasm_InstanceMapScript(Map* map) : InstanceScript(map) {
            _teamIdInInstance = TEAM_NEUTRAL;
            _aliveInitiandsCount = 0;
        }

        void Initialize() override
        {
            if (instance->IsHeroic())
            {
                SetHeaders(DataHeader);
                SetBossNumber(MAX_ENCOUNTERS);
                LoadDoorData(doorData);
                LoadBossBoundaries(boundaries);
                //LoadObjectData(creatureData, gameObjectData);
                //LoadMinionData(minionData);
                instance->SetZoneMusic(AREA_RAGEFIRE_CHASM, MUSIC_RAGEFIRE_CHASM_HEROIC);
            }
        }

        void OnPlayerEnter(Player* player) override
        {
            if (_teamIdInInstance == TEAM_NEUTRAL)
                _teamIdInInstance = player->GetTeamId();
        }

        void OnCreatureCreate(Creature* creature) override
        {
            switch (creature->GetEntry())
            {
                case NPC_ASHCURSE:
                    _ashcurseGUID = creature->GetGUID();
                    break;
                case NPC_INITIAND:
                    if (creature->IsAlive())
                    {
                        ++_aliveInitiandsCount;
                        if (Creature* pAshcurse = instance->GetCreature(_ashcurseGUID))
                            pAshcurse->AI()->DoAction(ACTION_MAKE_ASHCURSE_INVULNERABLE);
                    }
                    break;
                case NPC_COMMANDER_VORTHALAX:
                    _commanderVorthalaxGUID = creature->GetGUID();
                    break;
                case NPC_VORTHALAXS_GUARD:
                    _vorthalaxsGuardGUID = creature->GetGUID();
                    break;
                case NPC_TWILIGHT_OVERSEER:
                    _twilightOverseerGUID = creature->GetGUID();
                    break;
            }
        }

        void OnGameObjectCreate(GameObject* go) override
        {
            switch (go->GetEntry())
            {
            case GO_FLAME_WALL:
                AddDoor(go);
                break;
            case GO_FLAME_RING:
                _flameRingGUID = go->GetGUID();
                break;
            }
        }

        void SetData(uint32 type, uint32 data) override
        {
            switch (type)
            {
                case DATA_INITIAND_DIED:
                    if (_aliveInitiandsCount > 0)
                    {
                        --_aliveInitiandsCount;
                        if (_aliveInitiandsCount == 0)
                            if (Creature* pAshcurse = instance->GetCreature(_ashcurseGUID))
                                pAshcurse->AI()->DoAction(ACTION_MAKE_ASHCURSE_VULNERABLE);
                    }
                    break;
            }
        }

        uint32 GetData(uint32 type) const override
        {
            if (type == DATA_INITIAND_ALIVE_COUNT)
                return _aliveInitiandsCount;

            return 0;
        }

        ObjectGuid GetGuidData(uint32 data) const override
        {
            switch (data)
            {
                case DATA_COMMANDER_VORTHALAX:
                    return _commanderVorthalaxGUID;
                case DATA_VORTHALAXS_GUARD:
                    return _vorthalaxsGuardGUID;
                case DATA_TWILIGHT_OVERSEER:
                    return _twilightOverseerGUID;
            }

            return ObjectGuid::Empty;
        }

    protected:
        uint8 _aliveInitiandsCount;
        TeamId _teamIdInInstance;
        ObjectGuid _ashcurseGUID;
        ObjectGuid _commanderVorthalaxGUID;
        ObjectGuid _vorthalaxsGuardGUID;
        ObjectGuid _twilightOverseerGUID;
        ObjectGuid _flameRingGUID;
    };
};

class npc_twilight_summon : public CreatureScript
{
public:
    npc_twilight_summon() : CreatureScript("npc_twilight_summon") { }

    struct npc_twilight_summonAI : public ScriptedAI
    {
        npc_twilight_summonAI(Creature* creature) : ScriptedAI(creature)
        {
            instance = creature->GetInstanceScript();
            hatchTimer = urand(8000, 15000);
            playerInRange = false;
        }

        Position homePosition;
        InstanceScript* instance;
        uint32 hatchTimer;
        bool playerInRange;

        void Reset() override
        {
            hatchTimer = urand(8000, 15000);
            playerInRange = false;
        }

        void UpdateAI(uint32 diff) override
        {
            if (me->GetReactState() != REACT_PASSIVE)
                me->SetReactState(REACT_PASSIVE);

            const float checkRadius = 10.0f;
            Map* map = me->GetMap();
            if (map && !playerInRange)
            {
                Map::PlayerList const& players = map->GetPlayers();
                for (Map::PlayerList::const_iterator itr = players.begin(); itr != players.end(); ++itr)
                {
                    Player* player = itr->GetSource();
                    if (player && !player->IsGameMaster() && player->IsAlive() && me->IsWithinDistInMap(player, checkRadius))
                    {
                        playerInRange = true;
                        break;
                    }
                }
            }

            if (playerInRange)
            {
                if (hatchTimer <= diff)
                {
                    TempSummon* twilightWhelp = me->SummonCreature(NPC_TWILIGHT_WHELP, me->GetPositionX(), me->GetPositionY(), me->GetPositionZ(), me->GetOrientation(), TEMPSUMMON_TIMED_OR_DEAD_DESPAWN, 180000); // 180000 ms = 3 Minute Despawn Timer
                    twilightWhelp->CastSpell(twilightWhelp, SPELL_EGG_HATCH, true);
                    twilightWhelp->SetOrientation(frand(0.0f, M_PI));
                    me->DespawnOrUnsummon();
                }
                else
                {
                    hatchTimer -= diff;
                }
            }
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_twilight_summonAI(creature);
    }
};

class npc_kneeling_after_spawn : public CreatureScript
{
public:
    npc_kneeling_after_spawn() : CreatureScript("npc_kneeling_after_spawn") { }

    struct npc_kneeling_after_spawnAI : public ScriptedAI
    {
        npc_kneeling_after_spawnAI(Creature* creature) : ScriptedAI(creature) {
            instance = creature->GetInstanceScript();
        }

        Position homePosition;
        InstanceScript* instance;

        void Reset() override
        {
            homePosition = me->GetHomePosition();

            me->SetStandState(UNIT_STAND_STATE_KNEEL);
        }

        void JustDied(Unit* /*killer*/) override
        {
            if (instance)
                instance->SetData(DATA_INITIAND_DIED, 0);
        }

        void EnterEvadeMode(EvadeReason /*why*/) override
        {
            ScriptedAI::EnterEvadeMode();
            me->GetMotionMaster()->MoveTargetedHome();
        }
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new npc_kneeling_after_spawnAI(creature);
    }
};

void AddSC_instance_ragefire_chasm()
{
    new instance_ragefire_chasm();
    new npc_twilight_summon();
    new npc_kneeling_after_spawn();
}
