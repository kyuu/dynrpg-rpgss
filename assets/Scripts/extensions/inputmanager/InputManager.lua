require "system"
require "extensions.timer"

InputManager = class {
    __name = "InputManager",
    
    listeners = {},
    sortedListeners = {},
    
    keyRepeat = {
        delay = 500,    -- milliseconds before key repeat start
        interval = 50,  -- milliseconds before next key repeat
        timer = 0,
        key = nil
    },
    
    keys = {},
    
    mbuttons = {},
    
    onSceneDrawn = function(self, scene)
        self:update()
    end
}

function InputManager:fireKeyboardEvent(type, key, vkey)
    local e = {
        type = type,
        key = key,
        vkey = vkey,
        handled = false
    }
    
    for _, l in ipairs(self.sortedListeners) do
        if l.self.onKeyboardEvent ~= nil then
            l.self:onKeyboardEvent(e)
            if e.handled then
                break
            end
        end
    end
end

function InputManager:fireMouseButtonEvent(type, button)
    local e = {
        type = type,
        button = button,
        handled = false
    }
    
    for _, l in ipairs(self.sortedListeners) do
        if l.self.onMouseButtonEvent ~= nil then
            l.self:onMouseButtonEvent(e)
            if e.handled then
                break
            end
        end
    end
end

function InputManager:update()
    -- update key states
    local kb = keyboard.getState()
    for k, d in pairs(kb) do
        local o = self.keys[k]
        
        if o == nil then
            o = {
                key = k,
                vkey = keyboard.getVirtualKeyCode(k),
                down = false
            }
            self.keys[k] = o
        end
        
        -- if key was previously down and now is up
        if o.down == true and d == false then
            self:fireKeyboardEvent("key up", o.key, o.vkey)
            
            -- if this is the key repeat key,
            -- reset the key repeat state
            if k == self.keyRepeat.key then
                self.keyRepeat.timer = 0
                self.keyRepeat.key = nil
            else
                -- it's not the key repeat key, so
                -- just reset the key repeat timer
                self.keyRepeat.timer = 0
            end
        end
        
        -- if key was previously up and now is down
        if o.down == false and d == true then
            self:fireKeyboardEvent("key down", o.key, o.vkey)
            
            -- reset key repeat timer and set this
            -- key as the new key repeat key
            self.keyRepeat.timer = 0
            self.keyRepeat.key = k
        end
        
        -- update key state
        o.down = d
    end
    
    -- update key repeat if enabled (delay > 0)
    if self.keyRepeat.delay > 0 and self.keyRepeat.key then
        -- for brevity
        local r = self.keyRepeat
        local o = self.keys[r.key]
        -- to be on the safe side, check if the
        -- key repeat key is really down
        if o.down then
            r.timer = r.timer + Timer:getTimeDelta()
            while r.timer >= r.delay do
                self:fireKeyboardEvent("key down", o.key, o.vkey)
                r.timer = r.timer - r.interval
            end
        else
            -- uh, something went wrong
            -- correct the mistake
            r.timer = 0
            r.key = nil
        end
    end
    
    -- update mouse button states
    local mb = mouse.getState()
    for b, d in pairs(mb) do
        local o = self.mbuttons[b]
        
        if o == nil then
            o = {
                button = b,
                down = false
            }
            self.mbuttons[b] = o
        end
        
        -- if mouse button was previously down and now is up
        if o.down == true and d == false then
            self:fireMouseButtonEvent("button up", o.button)
        end
        
        -- if mouse button was previously up and now is down
        if o.down == false and d == true then
            self:fireMouseButtonEvent("button down", o.button)
        end
        
        -- update mouse button state
        o.down = d
    end
end

function InputManager:sortListeners()
    local t = {}
    for _, l in pairs(self.listeners) do
        table.insert(t, l)
    end
    table.sort(
        t,
        function(a, b)
            return a.prio > b.prio
        end
    )
    self.sortedListeners = t
end

function InputManager:addListener(name, priority, listener)
    -- sanity checks
    assert(type(name) == "string" and #name > 0, "invalid name")
    assert(type(priority) == "number", "invalid priority")
    assert(type(listener) == "table", "invalid listener")
    assert(self.listeners[name] == nil, "listener '"..name.."' already exists")

    -- add listener
    self.listeners[name] = {
        name = name,
        prio = priority,
        self = listener
    }
    
    -- listeners table changed, re-sort
    self:sortListeners()
end

function InputManager:removeListener(name)
    -- sanity checks
    assert(type(name) == "string" and #name > 0, "invalid name")
    assert(self.listeners[name] ~= nil, "listener '"..´name.."' does not exist")

    -- remove listener
    self.listeners[name] = nil
    
    -- listeners table changed, re-sort
    self:sortListeners()
end

function InputManager:getKeyRepeatDelay()
    return self.keyRepeat.delay
end

function InputManager:setKeyRepeatDelay(delay)
    self.keyRepeat.delay = delay
end

-- register in callback manager
CallbackManager:addListener("InputManager", -900, InputManager)
