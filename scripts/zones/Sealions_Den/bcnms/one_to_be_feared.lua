-----------------------------------
-- Area: Sealion's Den
-- Name: one_to_be_feared
-- bcnmID : 992
-----------------------------------
package.loaded["scripts/zones/Sealions_Den/TextIDs"] = nil;
-----------------------------------

require("scripts/globals/missions");
require("scripts/globals/battlefield");
require("scripts/zones/Sealions_Den/TextIDs");

-----------------------------------
--battlefield 1   @pos -780 -103 -90
          -- >     -231              = lieux de combat
--battlefield 2   @pos -140 -23 -450
         --  >      -151             = lieux de combat
--battlefield 3   @pos 500  56  -810
         --  >    640  -71   -206           = lieux de combat
		 
		 
	--cs 0,battlefieldID= cs + teleportation	 vers mamet
	--cs 1,battlefieldID= cs + teleportation	 vers ultima	 
	--cs 2,battlefieldID= cs + teleportation	 vers omega	 
	--cs 7 leave l'insctance	 
	-- cs 8 =>navire de guerre > retourner a tavnazia	 
		 
		 
-- After registering the BCNM via bcnmRegister(bcnmid)
function onBcnmRegister(player,battlefield)
end;

-- Physically entering the BCNM via bcnmEnter(bcnmid)
function onBcnmEnter(player,battlefield)
end;

function onBattlefieldTick(battlefield)
    OnBattlefieldTick(battlefield);
end;

-- Leaving the BCNM by every mean possible, given by the LeaveCode
-- 1=Select Exit on circle
-- 2=Winning the BC
-- 3=Disconnected or warped out
-- 4=Losing the BC
-- via bcnmLeave(1) or bcnmLeave(2). LeaveCodes 3 and 4 are called
-- from the core when a player disconnects or the time limit is up, etc

function onBcnmLeave(player,battlefield,leavecode)

	
	if (leavecode == 2) then -- play end CS. Need time and battle id for record keeping + storage
		if (player:getCurrentMission(COP) == ONE_TO_BE_FEARED and player:getVar("PromathiaStatus")==2) then
			player:startEvent(0x7d01,1,1,1,battlefield:getTimeInside(),1,0,0);				
			player:setVar("PromathiaStatus",0);
			player:completeMission(COP,ONE_TO_BE_FEARED);
			player:addMission(COP,CHAINS_AND_BONDS);
		else
			player:startEvent(0x7d01,1,1,1,battlefield:getTimeInside(),1,0,1);			
		end
	elseif (leavecode == 4) then
	       player:startEvent(0x7d02);
   end
	
end;

function onEventUpdate(player,csid,option)
-- print("bc update csid "..csid.." and option "..option);
end;
	
function onEventFinish(player,csid,option)
-- print("bc finish csid "..csid.." and option "..option);	
    if (csid == 0x7d01) then
     player:addExp(1500);
	 player:setPos(438 ,0 ,-18 ,11 ,24);-- tp lufease
	end
end;