require "system"

GameClock = class {
    __name = "GameClock",
    
    font = Cache:font("Fonts/msmincho.png"),
    
    windowskin = Cache:windowskin("WindowSkins/system4.png"),

    playTime = 0, -- total play time in milliseconds
    
    visible = true,
    
    onNewGame = function(self)
        self:reset()
    end,

    onLoadGame = function(self, id, data)
        self:load(data)
    end,

    onSaveGame = function(self, id)
        return self:save()
    end,

    onSceneDrawn = function(self, scene)
        self:update(scene)
        self:render(scene)
    end
}

function GameClock:reset()
    -- reset play time
    self.playTime = 0
end

function GameClock:load(data)
    -- clear current state
    self:reset()

    local mfile = io.newMemoryFile(data)
    local reader = io.newReader(mfile)

    -- read play time
    self.playTime = reader:readDouble()

    -- check for EOF
    if mfile.eof then
        error("EOF")
    end
end

function GameClock:save()
    local mfile = io.newMemoryFile()
    local writer = io.newWriter(mfile)

    -- write play time
    writer:writeDouble(self.playTime)

    return mfile:copyBuffer()
end

function GameClock:update(scene)
    -- update play time
    self.playTime = self.playTime + Timer:getTimeDelta()
end

function GameClock:render(scene)
    if self.visible then
        -- only render in the main menu
        if scene == "menu" and game.menu.scene == "main" then
            -- draw window
            game.screen.drawWindow(self.windowskin, 3, 163, 82, 42)

            -- draw play time
            game.screen.drawText(self.font, 8, 168, "Time")
            game.screen.drawText(self.font, 26, 184, self:getPlayTimeString())
        end
    end
end

function GameClock:getPlayTimeMillis()
    return self.playTime
end

function GameClock:getPlayTimeHms()
    -- get total play time in seconds
    local tm = self:getPlayTimeMillis() / 1000
    
    -- decompose play time into hours, minutes and seconds
    local s = math.floor( tm       % 60)
    local m = math.floor((tm / 60) % 60)
    local h = math.floor( tm / 3600    )

    return h, m, s
end

function GameClock:setPlayTimeHms(h, m, s)
    self.playTime = (h * 3600 + m * 60 + s) * 1000
end

function GameClock:getPlayTimeString()
    -- get hours, minutes and seconds
    local h, m, s = self:getPlayTimeHms()
    
    -- build play time string
    local str = ""
    
    if h < 10 then
        str = str.."0"..h
    else
        str = str..h
    end
    
    str = str..":"
    
    if m < 10 then
        str = str.."0"..m
    else
        str = str..m
    end
    
    str = str..":"
    
    if s < 10 then
        str = str.."0"..s
    else
        str = str..s
    end
    
    return str
end

-- register in callback manager
CallbackManager:addListener("GameClock", 100, GameClock)
