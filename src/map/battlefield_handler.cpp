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

#include "alliance.h"
#include "entities/charentity.h"
#include "battlefield_handler.h"
#include "entities/mobentity.h"
#include "utils/zoneutils.h"
#include "utils/charutils.h"
#include "lua/luautils.h"
#include "packets/char_recast.h"
#include "packets/char_skills.h"


CBattlefieldHandler::CBattlefieldHandler(uint16 zoneid)
{
	m_ZoneId = zoneid;

    m_MaxBattlefields = luautils::OnBattlefieldHandlerInitialise(zoneutils::GetZone(m_ZoneId));

    memset(&m_Battlefields, 0, sizeof(m_Battlefields));
}

void CBattlefieldHandler::HandleBattlefields(uint32 tick)
{
    for (int i = 0; i < m_MaxBattlefields; i++)
    {
        if (m_Battlefields[i] != nullptr)
        {
            //handle it!
            CBattlefield* PBattlefield = m_Battlefields[i];
            PBattlefield->m_LastTick = tick;
            luautils::OnBattlefieldTick(PBattlefield);
        }
    }
}
	//send 246 with bunrning circle as target (bcnm is full. followed by time remaining)

int CBattlefieldHandler::RegisterBcnm(uint16 id, CCharEntity* PChar)
{
    CBattlefield* PBattlefield = battlefieldutils::LoadBattlefield(this, id);

    if (!PBattlefield)
        return -1;

    for (int i = 0; i < m_MaxBattlefields; i++)
    {
        if (m_Battlefields[i] == nullptr)
        {
            PBattlefield->SetBattlefieldNumber(i + 1);
            break;
        }
    }

    if (PBattlefield->GetState() != STATE_OPEN)
    {
        //no player met the criteria for entering, so revoke the previous permission.
        ShowDebug("No player has met the requirements for entering the BCNM.\n");
        delete PBattlefield;
        return -1;
    }

    m_Battlefields[PBattlefield->GetBattlefieldNumber() - 1] = PBattlefield;
    PBattlefield->InsertEntity(PChar);

    // first player registered, spawn mobs and begin ticking down
    PBattlefield->InitBattlefield();

    luautils::OnBcnmRegister(PChar, PBattlefield);

    return PBattlefield->GetBattlefieldNumber();
}

/*
bool CBattlefieldHandler::hasFreeSpecialBattlefield(uint16 id){ //reserved for special battlefield like limbus

 switch(id)
	  {
	  case 1290:
		{ if( m_Battlefields[0] == nullptr){return true;}}
       break;
	  case 1291:
        { if( m_Battlefields[1] == nullptr){return true;}}
       break;
	   	  case 1292:
	    { if( m_Battlefields[2] == nullptr){return true;}}
       break;
	   	  case 1293:
		{ if( m_Battlefields[3] == nullptr){return true;}}
       break;
	   	  case 1294:
		{ if( m_Battlefields[4] == nullptr){return true;}}
       break;
	   	  case 1295:
		{ if( m_Battlefields[4] == nullptr){return true;}}
       break;
	   	  case 1296:
		{ if( m_Battlefields[5] == nullptr){return true;}}
       break;
	   	  case 1297:
		{ if( m_Battlefields[5] == nullptr){return true;}}
       break;
	   	 case 1298:
		{ if( m_Battlefields[0] == nullptr){return true;}}
       break;
	   	 case 1299:
		{if( m_Battlefields[1] == nullptr){return true;}}
       break;
	    case 1300:
		{if( m_Battlefields[2] == nullptr){return true;}}
       break;
	    case 1301:
		{if( m_Battlefields[3] == nullptr){return true;}}
       break;
	    case 1302:
		{if( m_Battlefields[3] == nullptr){return true;}}
       break;
	   case 1303:
		{if( m_Battlefields[4] == nullptr){return true;}}
       break;
	   case 1304:
		{if( m_Battlefields[5] == nullptr){return true;}}
       break;
	  case 1305:
		{if( m_Battlefields[6] == nullptr){return true;}}
      break;
	  case 1306:
		{if( m_Battlefields[7] == nullptr){return true;}}
      break;
	  case 1307:
		{if( m_Battlefields[7] == nullptr){return true;}}
      break;
	  default:
        return false;
	 break;
	  }
  return false;
}
*/
/*
bool CBattlefieldHandler::hasSpecialBattlefieldEmpty(uint16 id){ //reserved for special battlefield like limbus
  if(id <= m_MaxBattlefields &&  id!=0){
	  if(m_Battlefields[id-1] != nullptr){
					 return false;
	  }
   }
 return true;
}
*/
void CBattlefieldHandler::SetLootToBCNM(uint16 LootID,uint16 id,uint32 npcID)
{
	m_Battlefields[id-1]->SetLootId(LootID);
	CBaseEntity* PNpc = (CBaseEntity*)zoneutils::GetEntity(npcID, TYPE_NPC);
	m_Battlefields[id-1]->InsertEntity(PNpc);
}

void CBattlefieldHandler::RestoreOnBattlefield(uint16 id)
{
    if (id <= m_MaxBattlefields && id > 0)
    {
        CBattlefield* PBattlefield = m_Battlefields[id - 1];
        for (auto PChar : PBattlefield->m_PlayerList)
        {
            if (PChar->animation != ANIMATION_DEATH)
            {
                uint16 hp = PChar->GetMaxHP();
                uint16 mp = PChar->GetMaxMP();

                PChar->PRecastContainer->Del(RECAST_MAGIC);
                PChar->PRecastContainer->Del(RECAST_ABILITY);

                PChar->addHP(hp);
                PChar->addMP(mp);

                PChar->pushPacket(new CCharSkillsPacket(PChar));
                PChar->pushPacket(new CCharRecastPacket(PChar));
                charutils::UpdateHealth(PChar);


                //361 - All of <target>'s abilities are recharged.
                PChar->pushPacket(new CMessageBasicPacket(PChar, PChar, 0, 0, 361));

                //357 - <target> regains .. HP.
                PChar->pushPacket(new CMessageBasicPacket(PChar, PChar, hp, mp, 357));

                //357 - <target> regains .. HP.
                PChar->pushPacket(new CMessageBasicPacket(PChar, PChar, mp, mp, 358));
            }
        }
    }
}

CBattlefield* CBattlefieldHandler::getBattlefield(CCharEntity* PChar)
{
    CStatusEffect* PEffect = PChar->StatusEffectContainer->GetStatusEffect(EFFECT_BATTLEFIELD);

    for (int i = 0; i < m_MaxBattlefields; i++)
        if (PEffect && m_Battlefields[i] && 
                PEffect->GetPower() == m_Battlefields[i]->GetID() && PEffect->GetSubID() == m_Battlefields[i]->GetBattlefieldNumber())
                    return m_Battlefields[i];

    return nullptr;
}

//========================DYNAMIS FUNCTIONS=============================================//
/*
int CBattlefieldHandler::SpecialBattlefieldAddPlayer(uint16 id, CCharEntity* PChar)
{
	short Inst =0;
	switch(id)
	{
	  case 1290: Inst = 0; break;
	  case 1291: Inst = 1; break;
	  case 1292: Inst = 2; break;
	  case 1293: Inst = 3; break;
	  case 1294: Inst = 4; break;
	  case 1295: Inst = 4; break;
	  case 1296: Inst = 5; break;
	  case 1297: Inst = 5; break;
	  case 1298: Inst = 0; break;
	  case 1299: Inst = 1; break;
	  case 1300: Inst = 2; break;
	  case 1301: Inst = 3; break;
	  case 1302: Inst = 3; break;
	  case 1303: Inst = 4; break;
	  case 1304: Inst = 5; break;
	  case 1305: Inst = 6; break;
	  case 1306: Inst = 7; break;
	  case 1307: Inst = 7; break;
	}

	if(m_Battlefields[Inst]->addPlayerToBcnm(PChar)){
		ShowDebug("BattlefieldHandler ::Registration for Special Battlefield by %s succeeded \n",PChar->GetName());
	}
	return 1;
}
*/


bool CBattlefieldHandler::leaveBcnm(uint16 bcnmid, CCharEntity* PChar)
{
    // leave bcnm
    return false;
}

bool CBattlefieldHandler::winBcnm(uint16 bcnmid, CCharEntity* PChar)
{
    // win bcnm
    return false;
}

bool CBattlefieldHandler::enterBcnm(uint16 bcnmid, CCharEntity* PChar)
{
    // enter bcnm
    return false;
}

bool CBattlefieldHandler::hasFreeBattlefield()
{
    for (int i = 0; i < m_MaxBattlefields; i++)
        if (m_Battlefields[i] == nullptr)
            return true;

    return false;
}

uint8 CBattlefieldHandler::findBattlefieldIDFor(CCharEntity* PChar)
{
    // todo: get rid of this cause it's dumb

    if (getBattlefield(PChar))
        return getBattlefield(PChar)->GetBattlefieldNumber();

    return 255;
}