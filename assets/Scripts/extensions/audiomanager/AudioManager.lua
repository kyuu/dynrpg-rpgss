require "system"
require "extensions.timer"

AudioManager = class {
    __name = "AudioManager",
    
    bgm = {
        fading = nil,
        current = nil,
        memory = nil
    },
    
    bgs = {
        fading = nil,
        current = nil,
        memory = nil
    },
    
    me = {
        current = nil
    },

    se = {
        cache = {}
    },

    onTitleScreen = function(self)
        self:reset()
    end,

    onSceneDrawn = function(self, scene)
        self:update()
    end
}

function AudioManager:load(data)
end

function AudioManager:save()
end

function AudioManager:reset()
end

function AudioManager:update()
    -- get milliseconds passed since last update
    local dt = Timer:getTimeDelta()
    
    if dt == 0 then
        -- no sense to continue
        return
    end
    
    -- update bgm fading
    if self.bgm.fading then
        local fade = self.bgm.fading -- for brevity
        fade.time_left = fade.time_left - dt
        if fade.time_left > 0 then
            -- decrease volume
            fade.sound.volume = fade.orig_volume * (fade.time_left / fade.time_total)
        else
            -- finished fading
            fade.sound:stop()
            self.bgm.fading = nil
        end
    end
    
    -- update bgs fading
    if self.bgs.fading then
        local fade = self.bgs.fading -- for brevity
        fade.time_left = fade.time_left - dt
        if fade.time_left > 0 then
            -- decrease volume
            fade.sound.volume = fade.orig_volume * (fade.time_left / fade.time_total)
        else
            -- finished fading
            fade.sound:stop()
            self.bgs.fading = nil
        end
    end
    
    -- update me fading
    if self.me.fading then
        local fade = self.me.fading -- for brevity
        fade.time_left = fade.time_left - dt
        if fade.time_left > 0 then
            -- decrease volume
            fade.sound.volume = fade.orig_volume * (fade.time_left / fade.time_total)
        else
            -- me fading finished
            fade.sound:stop()
            self.me.fading = nil
            
            -- resume bgm
            self:resumeBgm()
        end
    end
    
    -- check if we need to resume bgm after me stopped
    if self.me.current then
        if not self.me.current.sound.isPlaying then
            -- me stopped, clear it and resume bgm
            self.me.current = nil
            self:resumeBgm()
        end
    end
end

function AudioManager:playBgm(filename, volume, pitch)
    -- sanity checks
    assert(type(filename) == "string", "filename must be a string")
    
    -- init optional arguments to defaults, if omitted
    volume = volume or 1.0
    pitch = pitch or 1.0
    
    -- if filename is empty, clear bgm
    if #filename == 0 then
        if self.bgm.current then
            self.bgm.current.sound:stop()
        end
        self.bgm.current = nil
        return
    end

    -- stop current bgm
    if self.bgm.current then
        self.bgm.current.sound:stop()
    end

    -- play new bgm
    local bgm = {
        sound = audio.openSound(filename) or error("could not open sound '"..filename.."'"),
        filename = filename,
        volume = volume,
        pitch = pitch
    }

    bgm.sound.loop = true
    bgm.sound.volume = volume
    bgm.sound.pitch = pitch
    bgm.sound:play()

    self.bgm.current = bgm
end

function AudioManager:stopBgm()
    if self.bgm.current then
        self.bgm.current.sound:stop()
    end
end

function AudioManager:resumeBgm()
    if self.bgm.current then
        self.bgm.current.sound:play()
    end
end

function AudioManager:fadeBgm(time)
    -- sanity checks
    assert(type(time) == "number" and time > 0, "time must be a positive number")

    if self.bgm.current then
        -- if there is already a fading bgm, stop it
        if self.bgm.fading then
            self.bgm.fading.sound:stop()
        end
        
        -- init fading bgm
        self.bgm.fading = {
            sound = self.bgm.current.sound,
            orig_volume = self.bgm.current.sound.volume,
            time_total = time,
            time_left = time
        }
        
        -- clear current bgm
        self.bgm.current = nil
    end
end

function AudioManager:memorizeBgm()
    if self.bgm.current then
        self.bgm.memory = {
            filename = self.bgm.current.filename,
            volume = self.bgm.current.volume,
            pitch = self.bgm.current.pitch
        }
    else
        self.bgm.memory = nil
    end
end

function AudioManager:restoreBgm()
    if self.bgm.memory then
        self:playBgm(
            self.bgm.memory.filename,
            self.bgm.memory.volume,
            self.bgm.memory.pitch
        )
    end
end

function AudioManager:getBgm()
    if self.bgm.current then
        return self.bgm.current.sound
    else
        return nil
    end
end

function AudioManager:playBgs(filename, volume, pitch)
    -- sanity checks
    assert(type(filename) == "string", "filename must be a string")
    
    -- init optional arguments to defaults, if omitted
    volume = volume or 1.0
    pitch = pitch or 1.0
    
    -- if filename is empty, clear bgs
    if #filename == 0 then
        if self.bgs.current then
            self.bgs.current.sound:stop()
        end
        self.bgs.current = nil
        return
    end

    -- stop current bgs
    if self.bgs.current then
        self.bgs.current.sound:stop()
    end

    -- play new bgs
    local bgs = {
        sound = audio.openSound(filename) or error("could not open sound '"..filename.."'"),
        filename = filename,
        volume = volume,
        pitch = pitch
    }

    bgs.sound.loop = true
    bgs.sound.volume = volume
    bgs.sound.pitch = pitch
    bgs.sound:play()

    self.bgs.current = bgs
end

function AudioManager:stopBgs()
    if self.bgs.current then
        self.bgs.current.sound:stop()
    end
end

function AudioManager:resumeBgs()
    if self.bgs.current then
        self.bgs.current.sound:play()
    end
end

function AudioManager:fadeBgs(time)
    -- sanity checks
    assert(type(time) == "number" and time > 0, "time must be a positive number")

    if self.bgs.current then
        -- if there is already a fading bgs, stop it
        if self.bgs.fading then
            self.bgs.fading.sound:stop()
        end
        
        -- init fading bgs
        self.bgs.fading = {
            sound = self.bgs.current.sound,
            orig_volume = self.bgs.current.sound.volume,
            time_total = time,
            time_left = time
        }
        
        -- clear current bgs
        self.bgs.current = nil
    end
end

function AudioManager:memorizeBgs()
    if self.bgs.current then
        self.bgs.memory = {
            filename = self.bgs.current.filename,
            volume = self.bgs.current.volume,
            pitch = self.bgs.current.pitch
        }
    else
        self.bgs.memory = nil
    end
end

function AudioManager:restoreBgs()
    if self.bgs.memory then
        self:playBgs(
            self.bgs.memory.filename,
            self.bgs.memory.volume,
            self.bgs.memory.pitch
        )
    end
end

function AudioManager:getBgs()
    if self.bgs.current then
        return self.bgs.current.sound
    else
        return nil
    end
end

function AudioManager:playMe(filename, volume, pitch)
    -- sanity checks
    assert(type(filename) == "string", "filename must be a string")
    
    -- init optional arguments to defaults, if omitted
    volume = volume or 1.0
    pitch = pitch or 1.0

    -- if filename is empty, clear me
    if #filename == 0 then
        self:stopMe()
        return
    end
    
    -- stop current bgm
    self:stopBgm()
    
    -- stop current me
    if self.me.current then
        self.me.current.sound:stop()
    end
    
    -- play new me
    local me = {
        sound = audio.openSound(filename) or error("could not open sound '"..filename.."'")
    }
    
    me.sound.volume = volume
    me.sound.pitch = pitch
    me.sound:play()
    
    self.me.current = me
end

function AudioManager:stopMe()
    if self.me.current then
        -- stop me
        self.me.current.sound:stop()
        
        -- clear me
        self.me.current = nil
        
        -- resume bgm
        self:resumeBgm()
    end
end

function AudioManager:fadeMe(time)
    -- sanity checks
    assert(type(time) == "number" and time > 0, "time must be a positive number")

    if self.me.current then
        -- if there is already a fading me, stop it
        if self.me.fading then
            self.me.fading.sound:stop()
        end
        
        -- init fading me
        self.me.fading = {
            sound = self.me.current.sound,
            orig_volume = self.me.current.sound.volume,
            time_total = time,
            time_left = time
        }
        
        -- clear current me
        self.me.current = nil
    end
end

function AudioManager:playSe(filename)
    -- sanity checks
    assert(type(filename) == "string" and #filename > 0)
    
    -- get cached sound effect
    local se = self.se.cache[filename]
    
    -- if sound effect is not cached yet,
    -- open and cache the sound effect
    if se == nil then
        se = audio.openSoundEffect(filename)
        
        -- make sure openSoundEffect succeeded
        assert(se, "could not open sound effect '"..filename.."'")
        
        self.se.cache[filename] = se
    end
    
    -- play sound effect
    se:play()
end

function AudioManager:stopSe()
    -- multiple sound effects could be playing,
    -- so iterate over all sound effects in the
    -- sound effect cache and stop them
    for _, se in pairs(self.se.cache) do
        se:stop()
    end
end

-- register in callback manager
CallbackManager:addListener("AudioManager", 1, AudioManager)
