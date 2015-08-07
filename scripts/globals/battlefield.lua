require("scripts/globals/status");
require("scripts/globals/common");
require("scripts/globals/zone");


local BATTLEFIELD_RULES = {
    NONE                = 0x00,
    DISABLE_SUBJOBS      = 0x01,
    LOSE_EXP            = 0x02,
    REMOVE_3MIN          = 0x04,
    SPAWN_TREASURE_ON_WIN = 0x08,
    MAAT                = 0x10
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
    {maxAreas = 1, zones = {140}},
    {maxAreas = 6, zones = {38}},
    {maxAreas = 8, zones = {37}}
};



function onBattlefieldHandlerInitialise(zone)
    return GetMaxBattlefields(zone);
end;

function GetMaxBattlefields(zone)
    local region = zone:getRegion();
    local zoneid = zone:getID();
    
    -- handle special snowflakes first
    for i, area in pairs(Hipsterfields) do
        for _, zone in pairs(area.zones) do
            if zoneid == zone then
                return area.maxAreas;
            end
        end
    end
    
    -- not a hipster, probably dynamis
    for i, area in pairs(MaxBattlefieldAreas) do
        for _, regiontype in pairs(area.regions) do
            if region == regiontype then
                return area.maxAreas;
            end
         end
    end
    
    return MaxBattlefields;
end;

function OnBattlefieldTick(battlefield)
    local StartTime = battlefield:getStartTime();
    local Zone = battlefield:getZone();
    
    local Tick = os.time();
    local TimeInside = battlefield:getTimeInside();
    
    -- handle party wipes
    if battlefield:allPlayersDead() then
        if battlefield:getDeadTime() == 0 then
            battlefield:setDeadTime(Tick);
        end
    else
        if battlefield:getDeadTime() ~= 0 then
            battlefield:setDeadTime(0);
        end
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

function MeetsEndingConditions(battlefield, tick)
    local ReturnType = 0;
    -- todo: handle generic cases here, specific in the battlefield's script
    return ReturnType;
end;

function HandleTimePrompts(battlefield)
    local TimeLeft = battlefield:getTimeLimit() - TimeInside;
    
    -- 5 mins left in dyna
    if Zone:getType() == ZONETYPE_DYNAMIS and TimeLeft % 60 == 0 and TimeLeft <= 300 then
        battlefield:pushMessage(449, TimeLeft/60);
    -- regular battlefield, stick with usual message
    elseif TimeLeft % 60 == 0 then
        battlefield:pushMessage(202, TimeLeft); 
    end
end;