require "system"
require "utility"
require "extensions.cache"
require "extensions.timer"

LightmapManager = class {
    __name = "LightmapManager",
    
    currentMapId = 0,
    
    currentLightmap = nil,
    
    lightmapOpacity = Tween:new(255),
    
    onSceneDrawn = function(self, scene)
        self:update(scene)
        self:render(scene)
    end
}

function LightmapManager:getLightmapFilename(mapId)
    local id = ""
    if mapId < 1000 then
        id = id.."0"
    end
    if mapId < 100 then
        id = id.."0"
    end
    if mapId < 10 then
        id = id.."0"
    end
    id = id..mapId
    return "Picture/lightmaps/map"..id..".png"
end

function LightmapManager:update(scene)
    local dt = Timer:getTimeDelta()
    local mapId = game.map.id
    if self.currentMapId ~= mapId then
        self.currentMapId = mapId
        local filename = self:getLightmapFilename(mapId)
        if io.exists(filename) then
            self.currentLightmap = Cache:image(filename)
        else
            self.currentLightmap = nil
        end
    end
    self.lightmapOpacity:update(dt)
end

function LightmapManager:render(scene)
    if scene == "map" and self.currentLightmap then
        local cx, cy = game.map.getCameraPosition()
        game.screen.draw(
            self.currentLightmap,
            -cx,
            -cy,
            0.0,
            1.0,
            graphics.packColor(
                255,
                255,
                255,
                self.lightmapOpacity:getValue()
            )
        )
    end
end

function LightmapManager:changeOpacity(opacity, ms)
    opacity = math.max(math.min(opacity, 255), 0)
    ms = ms or 0
    self.lightmapOpacity:tweenTo(opacity, ms)
end

CallbackManager:addListener("LightmapManager", 200, LightmapManager)
