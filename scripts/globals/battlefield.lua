require("scripts/globals/status");
require("scripts/globals/common");
require("scripts/globals/zone");

BATTLEFIELD_RULES = {
    NONE                = 0x00,
    DISABLE_SUBJOBS      = 0x01,
    LOSE_EXP            = 0x02,
    REMOVE_3MIN          = 0x04,
    SPAWN_TREASURE_ON_WIN = 0x08,
    MAAT                = 0x10
};

function OnBattlefieldTick(battlefield)
    local StartTime = battlefield:getStartTime();
    local Zone = battlefield:getZone();
    
    -- load the script for this bcnm
    --require("scripts/zones/"..Zone:getName().."/bcnms/"..battlefield:getName()..".lua");
    
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
    
    -- check if we need to send a message out to players 
    handleTimePrompts(battlefield);
    
end

function ApplyRuleMask(battlefield)
    local RuleMask = battlefield:getRuleMask();
    
    if bit.band(RuleMask, )
    
    -- todo: handle generic cases here, specific in the battlefield's script
end

function MeetsEndingConditions(battlefield, tick)
    local ReturnType = 0;
    -- todo: handle generic cases here, specific in the battlefield's script
    return ReturnType;
end

function HandleTimePrompts(battlefield)
    local TimeLeft = battlefield:getTimeLimit() - TimeInside;
    
    -- 5 mins left in dyna
    if Zone:getType() == ZONETYPE_DYNAMIS and TimeLeft % 60 == 0 and TimeLeft <= 300 then
        battlefield:pushMessage(449, TimeLeft/60);
    -- regular battlefield, stick with usual message
    elseif TimeLeft % 60 == 0 then
        battlefield:pushMessage(202, TimeLeft); 
    end
end