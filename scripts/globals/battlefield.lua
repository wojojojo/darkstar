require("scripts/globals/status");
require("scripts/globals/common");

function onBattlefieldTick(battlefield)
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
    
    -- check if we need to send a message out to players 
    handleTimePrompts(battlefield);
    
    
    
end

function applyRuleMask(battlefield)

end

function meetsLosingConditions(battlefield, tick)
    
    return true;
end

function handleTimePrompts(battlefield)
    local TimeLeft = battlefield:getTimeLimit() - TimeInside;
    
    -- 5 mins left in dyna
    if Zone:getType() == ZONETYPE_DYNAMIS and TimeLeft % 60 == 0 and TimeLeft <= 300 then
        battlefield:pushMessage(449, TimeLeft/60);
    -- regular battlefield, stick with usual message
    elseif TimeLeft % 60 == 0 then
        battlefield:pushMessage(202, TimeLeft);
    end
end