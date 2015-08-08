require("scripts/globals/status");
require("scripts/globals/common");
require("scripts/globals/zone");


BATTLEFIELD_RULES = {
    NONE               = 0x00,
    DISABLE_SUBJOBS      = 0x01,
    LOSE_EXP           = 0x02,
    INSTA_REMOVE_ON_WIPE  = 0x04,
    SPAWN_TREASURE_ON_WIN = 0x08,
    MAAT               = 0x10
};

BATTLEFIELD_CONDITIONS = {
    SPAWNED_AT_START = 0x01,
    WIN_REQUIREMENT  = 0x02
};

BATTLEFIELD_STATE = {
    OPEN   = 0,
    LOCKED = 1,
    WIN    = 2,
    LOSE   = 3
};

-- most bcnms have 3 areas
local MaxBattlefields = 3;

-- dynamis and any other zones which all fall into a specific region
local MaxBattlefieldAreas = {
    -- {max number of battlefields, {regiontype}}
    {maxAreas = 1, regions = {REGION_DYNAMIS}},
    
};

-- cause apparently temenos and apollyon dont have the same amount of battlefield areas
local Hipsterfields = {
    -- {max number of battlefields, {zone, ids, in, no, particular, order}}
    {MaxAreas = 1, Zones = {140}},
    {MaxAreas = 6, Zones = {38}},
    {MaxAreas = 8, Zones = {37}}
};



function onBattlefieldHandlerInitialise(zone)
    return GetMaxBattlefields(zone);
end;

function GetMaxBattlefields(zone)
    local Region = zone:getRegion();
    local ZoneID = zone:getID();
    
    -- handle special snowflakes first
    for i, Area in pairs(Hipsterfields) do
        for _, Zone in pairs(Area.Zones) do
            if ZoneID == Zone then
                return Area.MaxAreas;
            end
        end
    end
    
    -- not a hipster, probably dynamis
    for i, Area in pairs(MaxBattlefieldAreas) do
        for _, RegionType in pairs(Area.Regions) do
            if Region == RegionType then
                return Area.MaxAreas;
            end
         end
    end
    
    return MaxBattlefields;
end;

function OnBattlefieldTick(battlefield)
    
    if MeetsEndingConditions(battlefield, battlefield:isScripted()) then
        HandleBattlefieldEnd(battlefield);
    end
    
    -- send a message out to players 
    HandleTimePrompts(battlefield);
    
end;

function ApplyRuleMask(battlefield, player)
    -- generic cases, handle anything to be applied on entering the bcnm here 
    local RuleMask = battlefield:getRuleMask();
    local LevelCap = battlefield:getLevelCap();
    
    if bit.band(RuleMask, BATTLEFIELD_RULES.DISABLE_SUBJOBS) then
        player:addStatusEffect(EFFECT_SJ_RESTRICTION, player:getSubJob(), 0, 0);
    end
    
    if LevelCap > 0 then
        player:addStatusEffect(EFFECT_LEVEL_RESTRICTION, LevelCap, 0, 0);
    end
    
    -- todo: handle generic cases here, specific in the battlefield's script
end;

function MeetsEndingConditions(battlefield, scripted)
    -- this is true for all bcnms
    if battlefield:getState() == BATTLEFIELD_STATE.LOSE or battlefield:getState() == BATTLEFIELD_STATE.WIN or HandlePartyWipe(battlefield) then
        return true;
    elseif battlefield:getState() == BATTLEFIELD_STATE.LOCKED then
        -- it's scripted, find out from its script
        if scripted then
            -- load the script
            local Script = "scripts/zones/"..battlefield:getZone():getName().."/bcnms/"..battlefield:getName();
            
            require(Script);
            
            -- it's scripted so we set the state in the script's function, not here
            local Ret = meetsEndingConditions(battlefield);
            
            package.loaded[Script] = nil;
            
            return Ret;
        else
            -- not scripted, assume default win condition is to kill all enemies
            -- check if all enemies are dead
            local Enemies = battlefield:getEntities(TYPE_MOB, true);
            local RequiredKills = 0;
            local TotalKills = 0;
            
            -- check only mobs with a condition set on them
            for _, Mob in pairs(Enemies)
                local CurrentAction = GetMobAction(Mob:getID());
                local Conditions = battlefield:getConditions(Mob);
                
                -- it's a win requirement, increment the amount of required kills
                if bit.band(Conditions, BATTLEFIELD_CONDITIONS.WIN_REQUIREMENT) then
                   RequiredKills = RequiredKills + 1;
                   
                   -- mob's dead, increment our total kills
                   if CurrentAction >= ACTION_FALL and CurrentAction <= ACTION_FADE_OUT then
                       TotalKills = TotalKills + 1;
                   end
                   
                end
            
            -- congratulations, you dont suck - you get to win!
            if TotalKills == RequiredKills then
                battlefield:setState(BATTLEFIELD_STATE.WIN);
                return true;
            end
            
        end
        
    else
        -- lets check if we need to lock players out
        local Enemies = battlefield:getEntities(TYPE_MOB, true);
        
        for _, Mob in pairs(Enemies) do
            local CurrentAction = GetMobAction(Mob:getID());
            
            -- mob is fighting, lock the battlefield and set the fight time
            if mob:getEnmityList() ~= nil then
                battlefield:setState(BATTLEFIELD_STATE.LOCKED);
                battlefield:setFightTick(os.time());
                break;
            end
        end
        
    end
    
    -- todo: handle generic cases here, specific in the battlefield's script
    -- todo is dun - i think
    return false;
end;

function HandlePartyWipe(battlefield)
    local Tick = battlefield:getLastTick();
    local RuleMask = battlefield:getRuleMask();
    
    -- handle party wipes
    if battlefield:allPlayersDead() then
        if battlefield:getDeadTime() == 0 then
            battlefield:setDeadTime(Tick);
        else
            -- failed bcnm cause insta fail on wipe, or everyone's been dead for 3 mins
            if bit.band(RuleMask, BATTLEFIELD_RULES.INSTA_REMOVE_ON_WIPE) or ((Tick - battlefield:getDeadTime()/1000) >= 180) then
                battlefield:setState(BATTLEFIELD_STATE.LOSE);
                return true;
            end
        end
    else
        -- set the time of the wipe
        if battlefield:getDeadTime() ~= 0 then
            battlefield:setDeadTime(0);
        end
    end
    
    return false;
end;

function HandleTimePrompts(battlefield)
    local TimeLeft = battlefield:getTimeLimit() - battlefield:getTimeInside();
    
    -- 5 mins left in dyna
    if Zone:getType() == ZONETYPE_DYNAMIS and TimeLeft % 60 == 0 and TimeLeft <= 300 then
        battlefield:pushMessage(449, TimeLeft/60);
    -- regular battlefield, stick with usual message
    elseif TimeLeft % 60 == 0 then
        battlefield:pushMessage(202, TimeLeft); 
    end
end;

function HandleBattlefieldEnd(battlefield)
    local RuleMask = battlefield:getRuleMask();
    local State = battlefield:getState();
    -- todo: handle win/lose here
end