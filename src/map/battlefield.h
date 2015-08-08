/*
===========================================================================

  Copyright (c) 2010-2015 Darkstar Dev Teams

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see http://www.gnu.org/licenses/

  This file is part of DarkStar-server source code.

===========================================================================
*/

#ifndef _CBATTLEFIELD_H
#define _CBATTLEFIELD_H

#include "../common/cbasetypes.h"
#include "../common/mmo.h"
#include <vector>
#include <unordered_map>
#include "utils/battlefieldutils.h"

enum BATTLEFIELD_RULES
{
    RULES_NONE                  = 0x00,
    RULES_DISABLE_SUBJOBS       = 0x01,
    RULES_LOSE_EXP              = 0x02,
    RULES_INSTA_REMOVE_ON_WIPE  = 0x04,
    RULES_SPAWN_TREASURE_ON_WIN = 0x08,
    RULES_MAAT                  = 0x10
};

enum BATTLEFIELD_CONDITIONS
{
    CONDITION_SPAWNED_AT_START = 0x01,
    CONDITION_WIN_REQUIREMENT  = 0x02
};

enum BATTLEIELD_LEAVE_CODE
{
    LEAVE_EXIT   = 1,
    LEAVE_WIN    = 2,
    LEAVE_WARPDC = 3,
    LEAVE_LOSE   = 4
};

enum BATTLEFIELD_STATE
{
    STATE_OPEN,
    STATE_LOCKED,
    STATE_WIN,
    STATE_LOSE
};

struct BattlefieldClearTime
{
    string_t name;
    uint32 time;
};

class CMobEntity;
class CCharEntity;
class CNpcEntity;
class CBaseEntity;
class CBattlefieldHandler;

class CBattlefield
{
public:

    CBattlefield(CBattlefieldHandler* hand, uint16 bcnmid, BATTLEFIELDTYPE type);

    uint16      GetID();
    uint32      GetTimeLimit();
    BATTLEFIELDTYPE GetType();
    const int8* GetName();
    uint16      GetZoneID();
    uint8       GetBattlefieldNumber();
    uint8       GetMaxParticipants();
    uint8       GetLevelCap();
    uint16      GetLootId();
    uint32      GetStartTime();
    uint32      GetDeadTime();
    uint8       GetEntrance();
    uint8       GetRuleMask();
    uint8       GetState();

    BattlefieldClearTime      GetCurrentRecord();

    bool        AllPlayersDead();

    void        SetID(uint16 id);
    void        SetTimeLimit(uint32 time);
    void        SetName(int8* name);
    void        SetZoneID(uint16 zone);
    void        SetBattlefieldNumber(uint8 battlefield);
    void        SetMaxParticipants(uint8 max);
    void        SetLevelCap(uint8 cap);
    void        SetLootId(uint16 id);
    void        SetDeadTime(uint32 time);
    void        SetEntrance(uint8 entrance);
    void        SetCurrentRecord(int8* name, uint32 time);
    void        SetState(BATTLEFIELD_STATE state);
    void        SetRuleMask(BATTLEFIELD_RULES rules);

    void        InsertEntity(CBaseEntity* PEntity, uint8 type = 0, uint8 conditions = 0);

    void        ClearBattlefield();
    void        InitBattlefield();
    void        PushMessage(uint16 message, uint32 param);

    std::vector<CCharEntity*> m_PlayerList;
    std::vector<CBaseEntity*> m_NpcList;
    std::vector<CMobEntity*> m_EnemyList;
    std::vector<CMobEntity*> m_AllyList;

    std::unordered_map<CMobEntity*, uint8> m_MobList;

    bool m_TreasureSpawned;

    uint32 m_LastTick;
    uint32 m_FightTick;

private:
    CBattlefieldHandler* m_Handler;
    uint16               m_ID;
    string_t             m_Name;
    uint16               m_ZoneID;
    BATTLEFIELDTYPE      m_Type;
    uint8                m_BattlefieldNumber;
    uint32               m_LootId;
    uint8                m_LevelCap;
    uint8                m_MaxParticipants;     // 1,3,6,12,18,zone
    uint16               m_DynaUniqueID;        // unique ID for dynamis battlefield
    uint8                m_Entrance;
    CCharEntity*         m_CurrentBattlefieldLeader;
    uint8                m_State;
    uint8                m_RuleMask;

    uint32 m_StartTime;
    uint32 m_AllDeadTime;                       // time when every pt member has fallen
    uint32 m_TimeLimit;

    BattlefieldClearTime m_CurrentRecord;
};

#endif