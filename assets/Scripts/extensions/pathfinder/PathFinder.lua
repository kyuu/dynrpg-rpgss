require "system"

local _PATH = (...):gsub('%.PathFinder$','')
local JG = require(_PATH..".jumper.grid")
local JPF = require(_PATH..".jumper.pathfinder")

PathFinder = class {
    __name = "PathFinder",
    
    moves = {
        ["-1-1"] = "move up-left",
        ["0-1" ] = "move up",
        ["1-1" ] = "move up-right",
        ["-10" ] = "move left",
        ["10"  ] = "move right",
        ["-11" ] = "move down-left",
        ["01"  ] = "move down",
        ["11"  ] = "move down-right"
    },
    
    searchMode = "diagonal",
    
    walkableTerrains = {1},
    
    terrainMaps = {}
}

function PathFinder:createTerrainMap()
    local terrMap = {}
    for y = 0, game.map.height - 1 do
        local row = {}
        for x = 0, game.map.width - 1 do
            local tileId = game.map.getLowerLayerTileId(x, y)
            local terrId = game.map.getTerrainId(tileId)
            table.insert(row, terrId)
        end
        table.insert(terrMap, row)
    end
    return terrMap
end

function PathFinder:createCollisionMap(terrMap)
    local collMap = {}
    for y = 1, #terrMap do
        local trow = terrMap[y]
        local crow = {}
        for x = 1, #trow do
            local terrId = trow[x]
            crow[x] = self.walkableTerrains[terrId] ~= nil and 1 or 0
        end
        collMap[y] = crow
    end
    do
        local x, y = game.map.hero:getPosition()
        collMap[y+1][x+1] = 0
        for i = 1, #game.map.events do
            local e = game.map.events[i]
            if e.layer == "same as hero" then
                x, y = e:getPosition()
                collMap[y+1][x+1] = 0
            end
        end
    end
    return collMap
end

function PathFinder:getWalkableTerrains()
    return self.walkableTerrains
end

function PathFinder:setWalkableTerrains(terrains)
    assert(type(terrains) == "table")
    self.walkableTerrains = terrains
end

function PathFinder:getSearchMode()
    return self.searchMode
end

function PathFinder:setSearchMode(mode)
    assert(type(mode) == "string")
    self.searchMode = mode
end

function PathFinder:isWalkable(x, y)
    local hx, hy = game.map.hero:getPosition()
    if hx == x and hy == y then
        return false
    end
    for i = 1, #game.map.events do
        local e = game.map.events[i]
        if e.layer == "same as hero" then
            ex, ey = e:getPosition()
            if ex == x and ey == y then
                return false
            end
        end
    end
    local tileId = game.map.getLowerLayerTileId(x, y)
    local terrId = game.map.getTerrainId(tileId)
    if self.walkableTerrains[terrId] == nil then
        return false
    end
    return true
end

function PathFinder:findPath(sx, sy, ex, ey, allowNearest)
    local terrainMap = self.terrainMaps[game.map.id]
    if terrainMap == nil then
        terrainMap = self:createTerrainMap()
        self.terrainMaps[game.map.id] = terrainMap
    end
    local collisionMap = self:createCollisionMap(terrainMap)
    local finder = JPF(JG(collisionMap), "ASTAR", 1)
    finder:setMode(self.searchMode:upper())
    return finder:getPath(sx+1, sy+1, ex+1, ey+1, false, allowNearest)
end

function PathFinder:move(character, x, y, frequency, maxMoves, allowNearest)
    frequency = frequency or 8
    maxMoves = maxMoves or -1
    local sx, sy = character:getPosition()
    if sx == x and sy == y then
        return
    end
    local path, length = self:findPath(sx, sy, x, y, allowNearest)
    if path then
        local mpat = {}
        local prev
        for node, _ in path:iter() do
            if prev then
                table.insert(mpat, self.moves[(node.x-prev.x)..(node.y-prev.y)])
                maxMoves = maxMoves - 1
                if maxMoves == 0 then
                    break
                end
            end
            prev = node
        end
        if #mpat > 0 then
            character:move(mpat, false, true, frequency)
        end
    end
    return path, length
end
