-------------------------------------------------------------------------------
-- Resets the game clock.
-------------------------------------------------------------------------------
function reset_gameclock()
    GameClock:reset()
end

-------------------------------------------------------------------------------
-- Gets the value that determines if the game clock is visible.
--
-- @param var           (Number) ID of the variable that will hold the Boolean
--                               value (1 or 0) that determines if the game
--                               clock is visible.
-------------------------------------------------------------------------------
function get_gameclock_visible(var)
    game.variables[var] = GameClock.visible
end

-------------------------------------------------------------------------------
-- Sets the value that determines if the game clock is visible.
--
-- @param visible       (Number) Boolean (1 or 0) that determines if the game
--                               clock is visible.
-------------------------------------------------------------------------------
function set_gameclock_visible(visible)
    GameClock.visible = visible ~= 0 and true or false
end

-------------------------------------------------------------------------------
-- Gets the number of hours, minutes and seconds played.
--
-- @param var_h         (Number) ID of the variable that will hold the hours.
-- @param var_m         (Number) ID of the variable that will hold the minutes.
-- @param var_s         (Number) ID of the variable that will hold the seconds.
-------------------------------------------------------------------------------
function get_gameclock_hms(var_h, var_m, var_s)
    local h, m, s = GameClock:getPlayTimeHms()
    game.variables[var_h] = h
    game.variables[var_m] = m
    game.variables[var_s] = s
end

-------------------------------------------------------------------------------
-- Sets the number of hours, minutes and seconds played.
--
-- @param h             (Number) The new number of hours.
-- @param m             (Number) The new number of minutes.
-- @param s             (Number) The new number of seconds.
-------------------------------------------------------------------------------
function set_gameclock_hms(h, m, s)
    GameClock:setPlayTimeHms(h, m, s)
end
