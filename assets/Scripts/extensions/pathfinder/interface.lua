-------------------------------------------------------------------------------
-- Sets the pathfinder search mode.
--
-- @param mode          (String) The new search mode. Possible values are
--                               "orthogonal" (4-way movement) and "diagonal"
--                               (8-way movement).
-------------------------------------------------------------------------------
function set_search_mode(mode)
    assert(mode == "orthogonal" or mode == "diagonal", "invalid mode")
    PathFinder:setSearchMode(mode)
end

-------------------------------------------------------------------------------
-- Sets the walkable terrains.
--
-- @param (...)         (Number) Variable number of terrain IDs to set as
--                               walkable.
--                               For instance, set_walkable_terrains(1, 2, 3)
--                               will set the terrain IDs 1, 2 and 3 as
--                               walkable.
-------------------------------------------------------------------------------
function set_walkable_terrains(...)
    local walkable = {}
    for _, tid in ipairs(arg) do
        table.insert(walkable, tid)
    end
    PathFinder:setWalkableTerrains(walkable)
end

-------------------------------------------------------------------------------
-- Determines if a particular position on the map is walkable, taking into
-- account the currently set walkable terrains, the hero position and the
-- positions of events that are at the same level as hero.
--
-- @param x, y          (Number) The map position to test for walkability.
-- @param vresult       (Number) The ID of the variable that will hold the
--                               boolean result (1 for walkable, 0 otherwise).
-------------------------------------------------------------------------------
function is_walkable(x, y, vresult)
    game.variables[vresult] = PathFinder:isWalkable(x, y)
end

-------------------------------------------------------------------------------
-- Looks for a path from the current position of a character to the position
-- specified by (x, y) and, if found moves the character on the path.
--
-- @param name          (String) The name of the character to move. Possible
--                               values are "hero" to move the hero, or the
--                               name of an existing event to move the event.
-- @param x, y          (Number) The map position to move the character to.
-- @param frequency     (Number) Optional frequency (speed) at which to move
--                               the character, in the range 1 (slowest) to
--                               8 (fastest). Defaults to 8.
-- @param maxmoves      (Number) Optional upper limit for the number of moves
--                               performed. If you don't care, either pass 0
--                               (or any smaller number), or leave out.
-- @param allownearest  (Number) Optional flag that determines (in case a path
--                               to the specified position could not be found)
--                               if a path to the nearest position is also
--                               acceptable. Pass 1 if acceptable, 0 otherwise.
--                               If left out, a path to the nearest position
--                               is assumed as inacceptable.
-- @param vresult       (Number) Optional ID of the variable that will hold the
--                               result of the operation (1 for path found,
--                               0 otherwise).
-------------------------------------------------------------------------------
function move_character(name, x, y, frequency, maxmoves, allownearest, vresult)
    assert(type(name) == "string" and #name > 0, "invalid name")
    frequency = frequency or 8
    maxmoves = maxmoves or -1
    allownearest = allownearest == 1 and true or false
    local character
    if name == "hero" then
        character = game.map.hero
    else
        character = game.map.findEvent(name)
    end
    if character == nil then
        error("character '"..name.."' does not exist")
    end
    local path, length = PathFinder:move(character, x, y, frequency, maxmoves, allownearest)
    if vresult then
        game.variables[vresult] = path and 1 or 0
    end
end
