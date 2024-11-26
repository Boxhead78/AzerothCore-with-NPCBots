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

#ifndef DEF_RAGEFIRE_CHASM_H
#define DEF_RAGEFIRE_CHASM_H

#define DataHeader "RC"

#define RagefireChasmScriptName "instance_ragefire_chasm"

enum DataTypes
{
    DATA_ASHCURSE = 0,
    DATA_EMBERFURY = 1,
    DATA_BARON_IGNATIUS = 2,
    DATA_GIANT_EARTHBORER = 3,
    DATA_COMMANDER_VORTHALAX = 4,
    MAX_ENCOUNTERS = 5,

    DATA_VORTHALAXS_GUARD = 10,
    DATA_TWILIGHT_OVERSEER = 11,

    DATA_INITIAND_ALIVE_COUNT = 20,
    DATA_INITIAND_DIED = 21,
};

enum AdditionalData
{
    ADDITIONAL_DATA_SET_ASHURSE_VULNERABLE = 0,
    ADDITIONAL_DATA_SET_ASHURSE_INVULNERABLE = 1,
};

enum NPCIDs
{
    NPC_ASHCURSE = 500000,
    NPC_INITIAND = 500002,
    NPC_TWILIGHT_EGG = 500007,
    NPC_TWILIGHT_WHELP = 500009,
    NPC_EARTHBORER = 500011,
    NPC_COMMANDER_VORTHALAX = 500019,
    NPC_FLAME_WAVE = 500021,
    NPC_HEAT_GROUND = 500022,
    NPC_BLOODY_MEAT = 500023,
    NPC_VORTHALAXS_GUARD = 500024,
    NPC_TWILIGHT_OVERSEER = 500025,
};

enum MiscIDs
{
    POINT_HOME = 1,

    AREA_RAGEFIRE_CHASM = 2437,

    MUSIC_RAGEFIRE_CHASM = 2529,
    MUSIC_RAGEFIRE_CHASM_HEROIC = 70000,

    QUEST_DESTRUCTION_OF_THE_TWILIGHT_EGGS_HORDE = 50013,
    QUEST_DESTRUCTION_OF_THE_TWILIGHT_EGGS_ALLIANCE = 50028,
};

enum GameObjects
{
    GO_FLAME_WALL = 450000,
    GO_FLAME_RING = 450001,
};

enum Actions
{
    ACTION_MAKE_ASHCURSE_VULNERABLE = 0,
    ACTION_MAKE_ASHCURSE_INVULNERABLE = 1
};

template <class AI, class T>
inline AI* GetRagefireChasmAI(T* obj)
{
    return GetInstanceAI<AI>(obj, RagefireChasmScriptName);
}

#define RegisterRagefireChasmCreatureAI(ai_name) RegisterCreatureAIWithFactory(ai_name, GetRagefireChasmAI)

#endif
