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
#include <string.h>

#include "../entities/charentity.h"
#include "../entities/mobentity.h"
#include "../party.h"
#include "charutils.h"
#include "../alliance.h"
#include "zoneutils.h"
#include "itemutils.h"
#include "battlefieldutils.h"
#include "../battlefield.h"
#include "../battlefield_handler.h"
#include "../packets/entity_update.h"

namespace battlefieldutils{
	/***************************************************************
		Loads the given battlefield from the database and returns
		a new Battlefield object.
	****************************************************************/
	CBattlefield* LoadBattlefield(CBattlefieldHandler* hand, uint16 bcnmid, BATTLEFIELDTYPE type){
		const int8* fmtQuery = "SELECT name, bcnmId, fastestName, fastestTime, timeLimit, levelCap, lootDropId, rules, partySize, zoneId \
						    FROM bcnm_info \
							WHERE bcnmId = %u";

		int32 ret = Sql_Query(SqlHandle, fmtQuery, bcnmid);

		if (ret == SQL_ERROR ||
		Sql_NumRows(SqlHandle) == 0 ||
		Sql_NextRow(SqlHandle) != SQL_SUCCESS)
		{
			ShowError("Cannot load battlefield BCNM:%i \n",bcnmid);
		}
		else
		{
				CBattlefield* PBattlefield = new CBattlefield(hand,Sql_GetUIntData(SqlHandle,1), type);
				int8* tmpName;
				Sql_GetData(SqlHandle,0,&tmpName,nullptr);
				PBattlefield->SetName(tmpName);
                PBattlefield->SetCurrentRecord(Sql_GetData(SqlHandle, 2), Sql_GetUIntData(SqlHandle, 3));
				PBattlefield->SetTimeLimit(Sql_GetUIntData(SqlHandle,4));
				PBattlefield->SetLevelCap(Sql_GetUIntData(SqlHandle,5));
				PBattlefield->SetLootId(Sql_GetUIntData(SqlHandle,6));
				PBattlefield->SetMaxParticipants(Sql_GetUIntData(SqlHandle,8));
				PBattlefield->SetZoneID(Sql_GetUIntData(SqlHandle,9));
				PBattlefield->SetRuleMask((BATTLEFIELD_RULES)Sql_GetUIntData(SqlHandle,7));
				return PBattlefield;
		}
		return nullptr;
	}

	/***************************************************************
		Spawns monsters for the given BCNMID/Battlefield number by
		looking at bcnm_battlefield table for mob ids then spawning
		them and adding them to the monster list for the given
		battlefield.
	****************************************************************/
	bool LoadMobs(CBattlefield* PBattlefield)
    {
		DSP_DEBUG_BREAK_IF(PBattlefield == nullptr);

		//get ids from DB
		const int8* fmtQuery = "SELECT monsterId, conditions \
						    FROM bcnm_battlefield \
							WHERE bcnmId = %u AND battlefieldNumber = %u";

		int32 ret = Sql_Query(SqlHandle, fmtQuery, PBattlefield->GetID(), PBattlefield->GetBattlefieldNumber());

		if (ret == SQL_ERROR ||
			Sql_NumRows(SqlHandle) == 0)
		{
			ShowError("spawnMonstersForBcnm : SQL error - Cannot find any monster IDs for BCNMID %i Battlefield %i \n",
				PBattlefield->GetID(), PBattlefield->GetBattlefieldNumber());
		}
		else
        {
			while(Sql_NextRow(SqlHandle) == SQL_SUCCESS)
            {
				uint32 mobid = Sql_GetUIntData(SqlHandle,0);
				uint8 condition = Sql_GetUIntData(SqlHandle,1);
				CMobEntity* PMob = (CMobEntity*)zoneutils::GetEntity(mobid, TYPE_MOB);
				if (PMob != nullptr)
				{

					PMob->m_battlefieldID = PBattlefield->GetBattlefieldNumber();
					PMob->m_bcnmID = PBattlefield->GetID();

                    // TODO: handle maat and restrictions somewhere
					PBattlefield->InsertEntity(PMob, condition & CONDITION_SPAWNED_AT_START ? 0 : 2, condition);
					
				}
                else
                {
				    ShowDebug("SpawnMobForBcnm: mob %u not found\n", mobid);
				}
			}
			return true;
		}
		return false;
	}
    /*
	uint8 getMaxLootGroups(CBattlefield* battlefield){
		const int8* fmtQuery = "SELECT MAX(lootGroupId) \
						FROM bcnm_loot \
						JOIN bcnm_info ON bcnm_info.LootDropId = bcnm_loot.LootDropId \
						WHERE bcnm_info.LootDropId = %u LIMIT 1";

		int32 ret = Sql_Query(SqlHandle, fmtQuery, battlefield->GetLootId());
		if (ret == SQL_ERROR ||	Sql_NumRows(SqlHandle) == 0 || Sql_NextRow(SqlHandle) != SQL_SUCCESS){
				ShowError("SQL error occured \n");
				return 0;
			}
			else {
				return (uint8)Sql_GetUIntData(SqlHandle,0);
			}
	}
    */
    /*
	uint16 getRollsPerGroup(CBattlefield* battlefield, uint8 groupID)
    {
		const int8* fmtQuery = "SELECT SUM(CASE \
			WHEN LootDropID = %u \
			AND lootGroupId = %u \
			THEN rolls  \
			ELSE 0 END) \
			FROM bcnm_loot;";

		int32 ret = Sql_Query(SqlHandle, fmtQuery, battlefield->GetLootId(), groupID);
		if (ret == SQL_ERROR || Sql_NumRows(SqlHandle) == 0 || Sql_NextRow(SqlHandle) != SQL_SUCCESS){
			ShowError("SQL error occured \n");
			return 0;
		}
		else {
			return (uint16)Sql_GetUIntData(SqlHandle,0);
		}
	}
    */
    /*
	bool spawnSecondPartDynamis(CBattlefield* battlefield)
    {
		DSP_DEBUG_BREAK_IF(battlefield == nullptr);

		//get ids from DB
		const int8* fmtQuery = "SELECT monsterId \
								FROM bcnm_battlefield \
								WHERE bcnmId = %u AND battlefieldNumber = 2";

		int32 ret = Sql_Query(SqlHandle, fmtQuery, battlefield->GetID());

		if (ret == SQL_ERROR ||
			Sql_NumRows(SqlHandle) == 0)
		{
			ShowError("spawnSecondPartDynamis : SQL error - Cannot find any monster IDs for Dynamis %i \n",
				battlefield->GetID(), battlefield->GetBattlefieldNumber());
		}
		else
        {
			while(Sql_NextRow(SqlHandle) == SQL_SUCCESS)
            {
				uint32 mobid = Sql_GetUIntData(SqlHandle,0);
				CMobEntity* PMob = (CMobEntity*)zoneutils::GetEntity(mobid, TYPE_MOB);
				if (PMob != nullptr)
				{
				    if (PMob->PBattleAI->GetCurrentAction() == ACTION_NONE ||
				        PMob->PBattleAI->GetCurrentAction() == ACTION_SPAWN)
				    {
				        PMob->PBattleAI->SetLastActionTime(0);
				        PMob->PBattleAI->SetCurrentAction(ACTION_SPAWN);

						PMob->m_battlefieldID = battlefield->GetBattlefieldNumber();

						ShowDebug("Spawned %s (%u) id %i inst %i \n",PMob->GetName(),PMob->id,battlefield->GetID(),battlefield->GetBattlefieldNumber());
						battlefield->InsertEntity(PMob, CONDITION_SPAWNED_AT_START & CONDITION_WIN_REQUIREMENT);
				    } 
                    else 
                    {
				        ShowDebug(CL_CYAN"spawnSecondPartDynamis: <%s> (%u) is already spawned\n" CL_RESET, PMob->GetName(), PMob->id);
				    }
				}
                else 
                {
				    ShowDebug("spawnSecondPartDynamis: mob %u not found\n", mobid);
				}
			}
			return true;
		}
		return false;
	}
    */
};