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

#include "../../common/showmsg.h"

#include "lua_battlefield.h"
#include "lua_baseentity.h"
#include "../entities/npcentity.h"
#include "../utils/mobutils.h"
#include "../utils/zoneutils.h"


/************************************************************************
*																		*
*  Constructor															*
*																		*
************************************************************************/

CLuaBattlefield::CLuaBattlefield(lua_State *L)
{
	if( !lua_isnil(L,-1) )
	{
		m_PLuaBattlefield = (CBattlefield*)(lua_touserdata(L,-1));
		lua_pop(L,1);
	}else{
		m_PLuaBattlefield = nullptr;
	}
}

/************************************************************************
*																		*
*  Constructor															*
*																		*
************************************************************************/

CLuaBattlefield::CLuaBattlefield(CBattlefield* PBattlefield)
{
	m_PLuaBattlefield = PBattlefield;
}

/************************************************************************
*                                                                       *
*						Get methods								        *
*                                                                       *
************************************************************************/

inline int32 CLuaBattlefield::getBattlefieldNumber(lua_State* L)
{
	DSP_DEBUG_BREAK_IF(m_PLuaBattlefield == nullptr);
	
	lua_pushinteger( L, m_PLuaBattlefield->GetBattlefieldNumber() );
	return 1;
}

inline int32 CLuaBattlefield::getTimeLimit(lua_State* L)
{
	DSP_DEBUG_BREAK_IF(m_PLuaBattlefield == nullptr);
	
	lua_pushinteger( L, m_PLuaBattlefield->GetTimeLimit() );
	return 1;
}

inline int32 CLuaBattlefield::getID(lua_State* L)
{
	DSP_DEBUG_BREAK_IF(m_PLuaBattlefield == nullptr);
	
	lua_pushinteger( L, m_PLuaBattlefield->GetID() );
	return 1;
}

inline int32 CLuaBattlefield::getTimeInside(lua_State* L)
{
	DSP_DEBUG_BREAK_IF(m_PLuaBattlefield == nullptr);
	uint32 duration = (m_PLuaBattlefield->m_LastTick - m_PLuaBattlefield->GetStartTime())/1000;
	lua_pushinteger( L, duration);
	return 1;
}

inline int32 CLuaBattlefield::getFastestTime(lua_State* L)
{
	DSP_DEBUG_BREAK_IF(m_PLuaBattlefield == nullptr);
	
	lua_pushinteger( L, m_PLuaBattlefield->GetCurrentRecord().time );
	return 1;
}

inline int32 CLuaBattlefield::getFastestPlayer(lua_State* L)
{
	DSP_DEBUG_BREAK_IF(m_PLuaBattlefield == nullptr);
	
	lua_pushstring( L, m_PLuaBattlefield->GetCurrentRecord().name.c_str() );
	return 1;
}

inline int32 CLuaBattlefield::setCurrentRecord(lua_State* L)
{
	DSP_DEBUG_BREAK_IF(m_PLuaBattlefield == nullptr);
    DSP_DEBUG_BREAK_IF(lua_isnil(L,1) || !lua_isstring(L,1));
    
    int8* name = (int8*)lua_tostring(L, 1);
    uint32 newTime = lua_isnil(L, 2) || !lua_isnumber(L,2) ? (uint32)time(NULL) : lua_tointeger(L,2);

    m_PLuaBattlefield->SetCurrentRecord(name, newTime);
	return 1;
}

inline int32 CLuaBattlefield::getEntrance(lua_State* L)
{
	DSP_DEBUG_BREAK_IF(m_PLuaBattlefield == nullptr);

	lua_pushinteger(L, m_PLuaBattlefield->GetEntrance());
	return 1;
}

inline int32 CLuaBattlefield::getLevelCap(lua_State* L)
{
    DSP_DEBUG_BREAK_IF(m_PLuaBattlefield == nullptr);

    lua_pushinteger(L, m_PLuaBattlefield->GetLevelCap());
    return 1;
}

inline int32 CLuaBattlefield::setLevelCap(lua_State* L)
{
    DSP_DEBUG_BREAK_IF(m_PLuaBattlefield == nullptr);
    DSP_DEBUG_BREAK_IF(lua_isnil(L, 1) || !lua_isnumber(L, 1));

    m_PLuaBattlefield->SetLevelCap(lua_tointeger(L, 1));
    return 0;
}

inline int32 CLuaBattlefield::setEntrance(lua_State* L)
{
	DSP_DEBUG_BREAK_IF(m_PLuaBattlefield == nullptr);
	DSP_DEBUG_BREAK_IF(!lua_isnumber(L, 1) || lua_isnil(L, 1));

	m_PLuaBattlefield->SetEntrance(lua_tointeger(L, 1));
	return 0;
}

inline int32 CLuaBattlefield::insertAlly(lua_State* L)
{
	DSP_DEBUG_BREAK_IF(m_PLuaBattlefield == nullptr);
	DSP_DEBUG_BREAK_IF(!lua_isnumber(L, 1) || lua_isnil(L, 1));

	uint32 groupid = lua_tointeger(L, 1);

	CMobEntity* PAlly = mobutils::InstantiateAlly(groupid, m_PLuaBattlefield->GetZoneID());
	if (PAlly)
	{
		m_PLuaBattlefield->m_AllyList.push_back(PAlly);
		PAlly->PBCNM = m_PLuaBattlefield;
		lua_getglobal(L, CLuaBaseEntity::className);
		lua_pushstring(L, "new");
		lua_gettable(L, -2);
		lua_insert(L, -2);
		lua_pushlightuserdata(L, (void*)PAlly);
		lua_pcall(L, 2, 1, 0);
	}
	else
	{
		lua_pushnil(L);
		ShowError(CL_RED "CLuaBattlefield::insertAlly - group ID %u not found!" CL_RESET, groupid);
	}
	return 1;
}

inline int32 CLuaBattlefield::getAllies(lua_State* L)
{
	DSP_DEBUG_BREAK_IF(m_PLuaBattlefield == nullptr);

	lua_createtable(L, m_PLuaBattlefield->m_AllyList.size(), 0);
	int8 newTable = lua_gettop(L);
	int i = 1;
	for (auto ally : m_PLuaBattlefield->m_AllyList)
	{
		lua_getglobal(L, CLuaBaseEntity::className);
		lua_pushstring(L, "new");
		lua_gettable(L, -2);
		lua_insert(L, -2);
		lua_pushlightuserdata(L, (void*)ally);
		lua_pcall(L, 2, 1, 0);

		lua_rawseti(L, -2, i++);
	}

	return 1;
}

inline int32 CLuaBattlefield::lose(lua_State* L)
{
	DSP_DEBUG_BREAK_IF(m_PLuaBattlefield == nullptr);

    m_PLuaBattlefield->ClearBattlefield();
	return 0;
}

inline int32 CLuaBattlefield::win(lua_State* L)
{
	DSP_DEBUG_BREAK_IF(m_PLuaBattlefield == nullptr);

	m_PLuaBattlefield->SetState(STATE_WIN);

	return 0;
}

inline int32 CLuaBattlefield::pushMessage(lua_State* L)
{
    DSP_DEBUG_BREAK_IF(m_PLuaBattlefield == nullptr);
    DSP_DEBUG_BREAK_IF(lua_isnil(L, 1) || !lua_isnumber(L, 1));
    DSP_DEBUG_BREAK_IF(lua_isnil(L, 2) || !lua_isnumber(L, 2));

    m_PLuaBattlefield->PushMessage(lua_tointeger(L, 1), lua_tointeger(L, 2));

    return 0;
}

inline int32 CLuaBattlefield::getEntities(lua_State* L)
{
    DSP_DEBUG_BREAK_IF(m_PLuaBattlefield == nullptr);
    DSP_DEBUG_BREAK_IF(lua_isnil(L, 1) || !lua_isnumber(L, 1));

    uint8 filter = lua_tointeger(L, 1);
    bool adds = lua_isnil(L, 2) ? 0 : lua_tointeger(L,2);

    auto func = [&L](auto list)
    {
        lua_createtable(L, list.size(), 0);
        int8 newTable = lua_gettop(L);
        int i = 1;

        for (auto PEntity : list)
        {
            lua_getglobal(L, CLuaBaseEntity::className);
            lua_pushstring(L, "new");
            lua_gettable(L, -2);
            lua_insert(L, -2);
            lua_pushlightuserdata(L, (void*)PEntity);
            lua_pcall(L, 2, 1, 0);

            lua_rawseti(L, -2, i++);

        }
    };

    if (filter == TYPE_PC)
    {
        func(m_PLuaBattlefield->m_PlayerList);
    }
    else if (filter == TYPE_NPC)
    {
        func(m_PLuaBattlefield->m_NpcList);
    }
    else if (filter == TYPE_MOB)
    {
        // return mobs required loaded into bcnm at startup (conditions are handled separately)
        if (!adds)
        {
            lua_createtable(L, m_PLuaBattlefield->m_MobList.size(), 0);
            int8 newTable = lua_gettop(L);
            int i = 1;

            for (auto PEntity : m_PLuaBattlefield->m_MobList)
            {
                lua_getglobal(L, CLuaBaseEntity::className);
                lua_pushstring(L, "new");
                lua_gettable(L, -2);
                lua_insert(L, -2);
                lua_pushlightuserdata(L, (void*)PEntity.first);
                lua_pcall(L, 2, 1, 0);

                lua_rawseti(L, -2, i++);
            }

        }
        else
        {
            func(m_PLuaBattlefield->m_EnemyList); // return all enemies, including adds
        }
    }

    return 1;
}

inline int32 CLuaBattlefield::insertEntity(lua_State* L)
{
    DSP_DEBUG_BREAK_IF(m_PLuaBattlefield == nullptr);
    DSP_DEBUG_BREAK_IF(lua_isnil(L, 1) || !lua_isnumber(L, 1));
    DSP_DEBUG_BREAK_IF(lua_isnil(L, 2) || !lua_isnumber(L, 2));

    uint32 id = lua_tointeger(L, 1);
    uint8 condition = lua_isnil(L, 2) ? 0 : lua_tointeger(L, 2);

    CBaseEntity* PEntity = zoneutils::GetEntity(id, TYPE_PC | TYPE_MOB | TYPE_NPC | TYPE_PET);

    if (PEntity)
        m_PLuaBattlefield->InsertEntity(PEntity, 0, condition);
    else
        ShowError("CLuaBattlefield::insertEntity unable to insert entity!");

    return 0;
}

inline int32 CLuaBattlefield::getZone(lua_State* L)
{
    DSP_DEBUG_BREAK_IF(m_PLuaBattlefield == nullptr);
    
    lua_pushlightuserdata(L, (void*)zoneutils::GetZone(m_PLuaBattlefield->GetZoneID()));
    return 1;
}

inline int32 CLuaBattlefield::getType(lua_State* L)
{
    DSP_DEBUG_BREAK_IF(m_PLuaBattlefield == nullptr);
    
    lua_pushinteger(L, m_PLuaBattlefield->GetType());
    return 1;
}

inline int32 CLuaBattlefield::getState(lua_State* L)
{
    DSP_DEBUG_BREAK_IF(m_PLuaBattlefield == nullptr);
    
    lua_pushinteger(L, m_PLuaBattlefield->GetState());
    return 1;
}

inline int32 CLuaBattlefield::getRuleMask(lua_State* L)
{
    DSP_DEBUG_BREAK_IF(m_PLuaBattlefield == nullptr);

    lua_pushinteger(L, m_PLuaBattlefield->GetRuleMask());
    return 1;
}

inline int32 CLuaBattlefield::getDeadTime(lua_State* L)
{
    DSP_DEBUG_BREAK_IF(m_PLuaBattlefield == nullptr);

    lua_pushinteger(L, m_PLuaBattlefield->GetDeadTime());
    return 1;
}

inline int32 CLuaBattlefield::allPlayersDead(lua_State* L)
{
    DSP_DEBUG_BREAK_IF(m_PLuaBattlefield == nullptr);

    lua_pushboolean(L, m_PLuaBattlefield->AllPlayersDead());
    return 1;
}

inline int32 CLuaBattlefield::getMaxParticipants(lua_State* L)
{
    DSP_DEBUG_BREAK_IF(m_PLuaBattlefield == nullptr);

    lua_pushinteger(L, m_PLuaBattlefield->GetMaxParticipants());
    return 1;
}

inline int32 CLuaBattlefield::setState(lua_State* L)
{
    DSP_DEBUG_BREAK_IF(m_PLuaBattlefield == nullptr);
    DSP_DEBUG_BREAK_IF(lua_isnil(L, 1) || !lua_isnumber(L, 1));

    m_PLuaBattlefield->SetState((BATTLEFIELD_STATE)lua_tointeger(L, 1));
    return 0;
}

inline int32 CLuaBattlefield::setBattlefieldNumber(lua_State* L)
{
    DSP_DEBUG_BREAK_IF(m_PLuaBattlefield == nullptr);
    DSP_DEBUG_BREAK_IF(lua_isnil(L, 1) || !lua_isnumber(L, 1));

    m_PLuaBattlefield->SetBattlefieldNumber(lua_tointeger(L, 1));
    return 0;
}

inline int32 CLuaBattlefield::getMobConditions(lua_State* L)
{
    DSP_DEBUG_BREAK_IF(m_PLuaBattlefield == nullptr);
    DSP_DEBUG_BREAK_IF(lua_isnil(L, 1) || !lua_isuserdata(L, 1));

    CMobEntity* PMob = (CMobEntity*)lua_touserdata(L, 1);

    uint8 conditions = 0;

    for (auto mob : m_PLuaBattlefield->m_MobList)
    {
        if (mob.first == PMob)
        {
            conditions = mob.second;
            break;
        }
    }

    lua_pushinteger(L, conditions);
    return 1;
}

inline int32 CLuaBattlefield::setFightTick(lua_State* L)
{
    DSP_DEBUG_BREAK_IF(m_PLuaBattlefield == nullptr);
    
    uint32 tick = lua_isnil(L, 1) || !lua_isnumber(L,1) ? (uint32)time(NULL) : lua_tointeger(L, 1);

    m_PLuaBattlefield->m_FightTick = tick;
    return 0;
}

/************************************************************************
*																		*
*  declare lua function													*
*																		*
************************************************************************/

const int8 CLuaBattlefield::className[] = "CBattlefield";
Lunar<CLuaBattlefield>::Register_t CLuaBattlefield::methods[] =
{
    LUNAR_DECLARE_METHOD(CLuaBattlefield,getBattlefieldNumber),
    LUNAR_DECLARE_METHOD(CLuaBattlefield,getID),
    LUNAR_DECLARE_METHOD(CLuaBattlefield,getTimeLimit),
    LUNAR_DECLARE_METHOD(CLuaBattlefield,getTimeInside),
    LUNAR_DECLARE_METHOD(CLuaBattlefield,getFastestTime),
    LUNAR_DECLARE_METHOD(CLuaBattlefield,getFastestPlayer),
    LUNAR_DECLARE_METHOD(CLuaBattlefield,getEntrance),
    LUNAR_DECLARE_METHOD(CLuaBattlefield,setEntrance),
    LUNAR_DECLARE_METHOD(CLuaBattlefield,getLevelCap),
    LUNAR_DECLARE_METHOD(CLuaBattlefield,insertAlly),
    LUNAR_DECLARE_METHOD(CLuaBattlefield,getAllies),
    LUNAR_DECLARE_METHOD(CLuaBattlefield,lose),
    LUNAR_DECLARE_METHOD(CLuaBattlefield,win),
    LUNAR_DECLARE_METHOD(CLuaBattlefield,pushMessage),
    LUNAR_DECLARE_METHOD(CLuaBattlefield,getEntities),
    LUNAR_DECLARE_METHOD(CLuaBattlefield,insertEntity),
    LUNAR_DECLARE_METHOD(CLuaBattlefield,getZone),
    LUNAR_DECLARE_METHOD(CLuaBattlefield,getType),
    LUNAR_DECLARE_METHOD(CLuaBattlefield,getState),
    LUNAR_DECLARE_METHOD(CLuaBattlefield,getRuleMask),
    LUNAR_DECLARE_METHOD(CLuaBattlefield,getDeadTime),
    LUNAR_DECLARE_METHOD(CLuaBattlefield,getMaxParticipants),
    LUNAR_DECLARE_METHOD(CLuaBattlefield,allPlayersDead),
    LUNAR_DECLARE_METHOD(CLuaBattlefield,setState),
    LUNAR_DECLARE_METHOD(CLuaBattlefield,setBattlefieldNumber),
    LUNAR_DECLARE_METHOD(CLuaBattlefield,setCurrentRecord),
    LUNAR_DECLARE_METHOD(CLuaBattlefield,setLevelCap),
    LUNAR_DECLARE_METHOD(CLuaBattlefield,getMobConditions),
    LUNAR_DECLARE_METHOD(CLuaBattlefield,setFightTick),

	{nullptr,nullptr}
}; 