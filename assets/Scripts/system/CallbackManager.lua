require "system.class"

CallbackManager = class {
    __name = "CallbackManager",

    listeners = {},
    sortedListeners = {}
}

function CallbackManager:sortListeners()
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

function CallbackManager:addListener(name, priority, listener)
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

function CallbackManager:removeListener(name)
    -- sanity checks
    assert(type(name) == "string" and #name > 0, "invalid name")
    assert(self.listeners[name] ~= nil, "listener '"..´name.."' does not exist")
    
    -- remove listener
    self.listeners[name] = nil
    
    -- listeners table changed, re-sort
    self:sortListeners()
end

function CallbackManager:onInit()
    for _, l in ipairs(self.sortedListeners) do
        if l.self.onInit ~= nil then
            l.self:onInit()
        end
    end
end

function CallbackManager:onTitleScreen()
    for _, l in ipairs(self.sortedListeners) do
        if l.self.onTitleScreen ~= nil then
            l.self:onTitleScreen()
        end
    end
end

function CallbackManager:onNewGame()
    for _, l in ipairs(self.sortedListeners) do
        if l.self.onNewGame ~= nil then
            l.self:onNewGame()
        end
    end
end

function CallbackManager:onLoadGame(id, data)
    local mfile = io.newMemoryFile(data)
    local reader = io.newReader(mfile)

    -- read signature
    local sig = reader:readString(#CallbackManager.__name)
    if sig ~= CallbackManager.__name then
        error("signature mismatch")
    end

    -- read number of listeners
    local nlisteners = reader:readUint32()

    -- check for EOF
    if mfile.eof then
        error("EOF")
    end

    -- read listeners data
    for i = 1, nlisteners do
        -- read listener name
        local l_name = reader:readString(reader:readUint32())

        -- read listener data
        local l_data = reader:readBytes(reader:readUint32())

        -- check for EOF
        if mfile.eof then
            error("EOF")
        end

        -- call onLoadGame callback on listener, if
        -- listener exists and implements the callback
        local l = self.listeners[l_name]
        if l ~= nil and l.self.onLoadGame ~= nil then
            l.self:onLoadGame(id, l_data)
        end
    end
end

function CallbackManager:onSaveGame(id)
    local mfile = io.newMemoryFile()
    local writer = io.newWriter(mfile)

    -- write signature
    writer:writeString(CallbackManager.__name)

    -- write number of listeners
    writer:writeUint32(#self.sortedListeners)

    -- write listeners data
    for _, l in ipairs(self.sortedListeners) do
        -- write listener name
        writer:writeUint32(#l.name)
        writer:writeString(l.name)

        -- if listener implements the onSaveGame callback,
        -- call it on the listener to get its savegame data
        if l.self.onSaveGame ~= nil then
            local l_data = l.self:onSaveGame(id)
            writer:writeUint32(#l_data)
            writer:writeBytes(l_data)
        else
            -- no listener savegame data available
            writer:writeUint32(0)
        end
    end

    return mfile:copyBuffer()
end

function CallbackManager:onSceneDrawn(scene)
    for _, l in ipairs(self.sortedListeners) do
        if l.self.onSceneDrawn ~= nil then
            l.self:onSceneDrawn(scene)
        end
    end
end

function CallbackManager:onDrawCharacter(character, isHero)
    for _, l in ipairs(self.sortedListeners) do
        if l.self.onDrawCharacter ~= nil then
            if not l.self:onDrawCharacter(character, isHero) then
                return false
            end
        end
    end
    return true
end

function CallbackManager:onCharacterDrawn(character, isHero)
    for _, l in ipairs(self.sortedListeners) do
        if l.self.onCharacterDrawn ~= nil then
            l.self:onCharacterDrawn(character, isHero)
        end
    end
end

function CallbackManager:onDrawBattler(battler, isMonster, id)
    for _, l in ipairs(self.sortedListeners) do
        if l.self.onDrawBattler ~= nil then
            if not l.self:onDrawBattler(battler, isMonster, id) then
                return false
            end
        end
    end
    return true
end

function CallbackManager:onBattlerDrawn(battler, isMonster, id)
    for _, l in ipairs(self.sortedListeners) do
        if l.self.onBattlerDrawn ~= nil then
            l.self:onBattlerDrawn(battler, isMonster, id)
        end
    end
end

function CallbackManager:onSystemBackgroundDrawn(x, y, width, height)
    for _, l in ipairs(self.sortedListeners) do
        if l.self.onSystemBackgroundDrawn ~= nil then
            l.self:onSystemBackgroundDrawn(x, y, width, height)
        end
    end
end
