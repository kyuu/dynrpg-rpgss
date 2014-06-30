require "system"

Timer = class {
    __name = "Timer",

    f0 = game.frameCounter, -- frame counter before
    f1 = game.frameCounter, -- frame counter current
    df = 0,                 -- frame counter delta
    dt = 0,                 -- time delta (ms passed since last update)
    
    onNewGame = function(self)
        -- frame counter was reset,
        -- so we need to reset as well
        self:reset()
    end,

    onLoadGame = function(self, id, data)
        -- frame counter was loaded,
        -- so we need to reset
        self:reset()
    end,
    
    onSceneDrawn = function(self, scene)
        self:update()
    end
}

function Timer:reset()
    self.f0 = game.frameCounter
    self.f1 = game.frameCounter
    self.df = 0
    self.dt = 0
end

function Timer:update()
    self.f0 = self.f1
    self.f1 = game.frameCounter
    self.df = self.f1 - self.f0
    self.dt = self.df * game.millisPerFrame
end

function Timer:getFrameDelta()
    return self.df
end

function Timer:getTimeDelta()
    return self.dt
end

-- register in callback manager
CallbackManager:addListener("Timer", -1000, Timer)
