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

#ifndef _CBATTLEFIELDHANDLER_H
#define _CBATTLEFIELDHANDLER_H

#include "../common/cbasetypes.h"
#include "../common/mmo.h"

#include "battlefield.h"
#include <vector>

class CCharEntity;
class CMobEntity;

class CBattlefieldHandler
{
public:

    CBattlefieldHandler(uint16 zoneid);
    void	HandleBattlefields(uint32 tick);							// called every tick to handle win/lose conditions, locking the bcnm, etc
    int		RegisterBcnm(uint16 bcnmid, CCharEntity* PChar);		// returns the battlefield id of the registration, -1 if no free bcnm.
                                                                    // also registers all people in the characters PT, etc.

    bool	enterBcnm(uint16 bcnmid, CCharEntity* PChar);			// Enters the BCNM battlefield if you're registered
    bool	leaveBcnm(uint16 bcnmid, CCharEntity* PChar);			// Leaves the BCNM battlefield if you're registered
    bool	winBcnm(uint16 bcnmid, CCharEntity* PChar);				// Wins a BCNM battlefield (e.g. the player opening the chest)

    bool    hasFreeBattlefield();
    uint8   findBattlefieldIDFor(CCharEntity* PChar);

    void    SetLootToBCNM(uint16 LootID, uint16 id, uint32 npcID);
    void    RestoreOnBattlefield(uint16 id);                          //restor MP HP ability on a specific battlefield
    CBattlefield* getBattlefield(CCharEntity*);                           // returns the battlefield a player is in

private:
    uint16					m_ZoneId;
    uint8					m_MaxBattlefields;							// usually 3 except dynamis, einherjar, besieged, ...
    CBattlefield*				m_Battlefields[8];
};

#endif