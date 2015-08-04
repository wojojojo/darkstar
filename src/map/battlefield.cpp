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

#include "battlefield.h"

#include "../common/timer.h"
#include "entities/charentity.h"
#include "entities/mobentity.h"
#include "entities/npcentity.h"
#include "entities/baseentity.h"
#include "packets/entity_animation.h"
#include "packets/entity_update.h"
#include "packets/position.h"
#include "packets/message_basic.h"
#include "lua/luautils.h"
#include "utils/zoneutils.h"


CBattlefield::CBattlefield(CBattlefieldHandler* hand, uint16 id, BATTLEFIELDTYPE type)
{
	m_Type = type;
	m_ID = id;
	m_Handler = hand;
	m_State = 0;
	m_DynaUniqueID = 0;
	m_TreasureSpawned = false;
	m_FightTick = 0;
	m_Entrance = 0;
    m_RuleMask = 0;
    m_LastTick = 0;

    m_CurrentRecord.name = nullptr;
    m_CurrentRecord.time = -1;          // fastest clear time - has to be smaller than this so we'll max this out
}

uint16 CBattlefield::GetID()
{
    return m_ID;
}

uint32 CBattlefield::GetTimeLimit()
{
    return m_TimeLimit;
}

const int8* CBattlefield::GetName()
{
    return m_Name.c_str();
}

uint16 CBattlefield::GetZoneID()
{
    return m_ZoneID;
}

uint8 CBattlefield::GetBattlefieldNumber()
{
    return m_BattlefieldNumber;
}

uint8 CBattlefield::GetMaxParticipants()
{
    return m_MaxParticipants;
}

uint8 CBattlefield::GetLevelCap()
{
    return m_LevelCap;
}

uint16 CBattlefield::GetLootId()
{
    return m_LootId;
}

uint32 CBattlefield::GetStartTime()
{
    return m_StartTime;
}

uint32 CBattlefield::GetDeadTime()
{
    return m_AllDeadTime;
}

uint8 CBattlefield::GetEntrance()
{
    return m_Entrance;
}

uint8 CBattlefield::GetRuleMask()
{
    return m_RuleMask;
}

uint8 CBattlefield::GetState()
{
    return m_State;
}

BattlefieldClearTime CBattlefield::GetCurrentRecord()
{
    return m_CurrentRecord;
}

bool CBattlefield::AllPlayersDead()
{
    bool allDead = true;
    
    for (auto PChar : m_PlayerList)
    {
        if (!PChar->isDead())
        {
            allDead = false;
            break;
        }
    }

    return allDead;
}

void  CBattlefield::SetID(uint16 id)
{
    m_ID = id;
}

void  CBattlefield::SetTimeLimit(uint32 time)
{
    m_TimeLimit = time;
}

void  CBattlefield::SetName(int8* name)
{
    m_Name = name;
}

void  CBattlefield::SetZoneID(uint16 zone)
{
    m_ZoneID = zone;
}

void  CBattlefield::SetBattlefieldNumber(uint8 battlefield)
{
    m_BattlefieldNumber = battlefield;
}

void  CBattlefield::SetMaxParticipants(uint8 max)
{
    m_MaxParticipants = max;
}

void  CBattlefield::SetLevelCap(uint8 cap)
{
    m_LevelCap = cap;
}

void  CBattlefield::SetLootId(uint16 id)
{
    m_LootId = id;
}

void  CBattlefield::SetDeadTime(uint32 time)
{
    m_AllDeadTime = time;
}

void  CBattlefield::SetEntrance(uint8 entrance)
{
    m_Entrance = entrance;
}

void CBattlefield::SetRuleMask(BATTLEFIELD_RULES rules)
{
    m_RuleMask = rules;
}

void CBattlefield::SetCurrentRecord(int8* name, uint32 time)
{
    if (time <= m_CurrentRecord.time)
    {
        Sql_Query(SqlHandle, "UPDATE bcnm_info SET fastestName = %s, fastestTime = %u WHERE bcnmId = %u AND zoneId = %u", name, time, m_ID, m_ZoneID);
        m_CurrentRecord.name = name;
        m_CurrentRecord.time = time;
    }
}

void CBattlefield::SetState(BATTLEFIELD_STATE state)
{
    m_State = state;
}

void CBattlefield::InsertEntity(CBaseEntity* PEntity, uint8 type, uint8 conditions)
{
    if (PEntity->objtype == TYPE_PC)
    {
        m_PlayerList.push_back(static_cast<CCharEntity*>(PEntity));
    }
    else if (PEntity->objtype == TYPE_NPC)
    {
        m_NpcList.push_back(static_cast<CNpcEntity*>(PEntity));
    }
    else if (PEntity->objtype == TYPE_MOB)
    {
        // required mob spawned by bcnm
        if (!type)
        {
            // only apply conditions to mobs spawning by default
            m_MobList.insert(std::make_pair(static_cast<CMobEntity*>(PEntity), conditions));
            m_EnemyList.push_back(static_cast<CMobEntity*>(PEntity));
        }
        // ally
        else if (type == 1)
        {
            m_AllyList.push_back(static_cast<CMobEntity*>(PEntity));
        }
        // stick any adds here (pets spawned by mobs, etc)
        else if (type == 2)
        {
            m_EnemyList.push_back(static_cast<CMobEntity*>(PEntity));
        }
    }
}

void CBattlefield::ClearBattlefield()
{
    // despawn mobs and reset enmity
    for (auto PMob : m_EnemyList)
    {
        if(!PMob->isDead())
            PMob->PBattleAI->SetCurrentAction(ACTION_DESPAWN);
        
        PMob->PBCNM = nullptr;
    }

    // make chest vanish
    for (auto PNpc : m_NpcList)
    {
        PNpc->loc.zone->PushPacket(PNpc, CHAR_INRANGE, new CEntityAnimationPacket(PNpc, CEntityAnimationPacket::FADE_OUT));
        PNpc->animation = ANIMATION_DEATH;
        PNpc->status = STATUS_MOB;
        PNpc->loc.zone->PushPacket(PNpc, CHAR_INRANGE, new CEntityUpdatePacket(PNpc,ENTITY_UPDATE, UPDATE_COMBAT));
    }

    // clear allies
    for (auto PAlly : m_AllyList)
    {
        zoneutils::GetZone(this->GetZoneID())->DeletePET(PAlly);
        delete PAlly;
    }

    luautils::OnBcnmDestroy(this);
    delete this;
}

void CBattlefield::PushMessage(uint16 message, uint32 param)
{
    for (auto PChar : m_PlayerList)
    {
        // what the hell even is lastBcnmTimePrompt
        PChar->pushPacket(new CMessageBasicPacket(PChar, PChar, param, 0, message));
        PChar->m_lastBcnmTimePrompt = param;
    }
}

void CBattlefield::InitBattlefield()
{
    for (auto PMob : m_MobList)
    {
        if (PMob.second & CONDITION_SPAWNED_AT_START)
            PMob.first->PBattleAI->SetCurrentAction(ACTION_SPAWN);
    }
}