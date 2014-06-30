require "system"
require "extensions.timer"
require "extensions.cache"
require "extensions.inputmanager"

SceneManager = class {
    __name = "SceneManager",

    fps = {
        font = Cache:font("Fonts/msmincho.png"),
        show = false,           -- display fps on screen?
        aux = game.frameRate,   -- auxiliary variable
        acc = 0,                -- fps accumulator
        cur = 0                 -- current fps
    },
    
    objects = {},
    
    onTitleScreen = function(self)
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
    end,
    
    onKeyboardEvent = function(self, e)
        if e.type == "key down" and e.key == "f10" then
            -- toggle show fps
            self.fps.show = not self.fps.show
        end
    end
}

function SceneManager:load(data)
    local mfile = io.newMemoryFile(data)
    local reader = io.newReader(mfile)

    -- read number of objects
    local nobjects = reader:readUint32()

    -- check for EOF
    if mfile.eof then
        error("end of file")
    end

    -- read objects
    for i = 1, nobjects do
        -- read object name
        local obj_name = reader:readString(reader:readUint32())

        -- read object data
        local obj_data = reader:readBytes(reader:readUint32())

        -- check for EOF
        if mfile.eof then
            error("end of file while reading object '"..(obj_name and obj_name or "?").."'")
        end

        self.objects[obj_name] = deserialize(obj_data)
    end
end

function SceneManager:save()
    local mfile = io.newMemoryFile()
    local writer = io.newWriter(mfile)

    -- write number of objects
    local nobjects = 0
    for name, obj in pairs(self.objects) do
        nobjects = nobjects + 1
    end
    writer:writeUint32(nobjects)

    -- write objects
    for name, obj in pairs(self.objects) do
        -- write object name
        writer:writeUint32(#name)
        writer:writeString(name)

        -- write object data
        local obj_data = serialize(obj)
        writer:writeUint32(#obj_data)
        writer:writeBytes(obj_data)
    end

    return mfile:copyBuffer()
end

function SceneManager:reset()
    -- clear objects
    self.objects = {}
end

function SceneManager:update(scene)
    -- get time delta
    local dt = Timer:getTimeDelta()
    
    -- during transitions frame counter doesn't change,
    -- i.e. time delta == 0. if this is the case, it
    -- makes no sense to proceed with the update
    if dt == 0 then
        return
    end
    
    -- update fps variables
    self.fps.acc = self.fps.acc + 1
    self.fps.aux = self.fps.aux - Timer:getFrameDelta()
    if self.fps.aux <= 0 then
        self.fps.aux = self.fps.aux + game.frameRate
        self.fps.cur = self.fps.acc
        self.fps.acc = 0
    end

    -- update objects
    for name, obj in pairs(self.objects) do
        if obj:update(scene, dt) then
            -- object wants to be removed
            self.objects[name] = nil
        end
    end
end

function SceneManager:render(scene)
    local render_list = {}
    
    -- find all drawable objects
    for _, obj in pairs(self.objects) do
        table.insert(render_list, obj)
    end

    -- sort them by their z-value in descending order, i.e.
    -- objects with a greater z-value will be drawn first
    table.sort(render_list, function(a, b) return a:getZ() > b:getZ() end)
    
    -- render them
    for _, obj in ipairs(render_list) do
        obj:render(scene)
    end
    
    -- draw fps if enabled
    if self.fps.show then
        game.screen.drawText(self.fps.font, 0, 0, "FPS: "..self.fps.cur)
    end
end

function SceneManager:addObject(name, object)
    -- allow number IDs, but convert them to string
    if type(name) == "number" then
        name = tostring(math.floor(number))
    end
    
    -- sanity checks
    assert(type(name) == "string" and #name > 0)
    assert(type(object) == "table")

    -- add object
    self.objects[name] = object
end

function SceneManager:removeObject(name)
    -- allow number IDs, but convert them to string
    if type(name) == "number" then
        name = tostring(math.floor(number))
    end

    -- sanity checks
    assert(type(name) == "string" and #name > 0)
    
    -- remove object
    self.objects[name] = nil
end

function SceneManager:getObject(name)
    -- allow number IDs, but convert them to string
    if type(name) == "number" then
        name = tostring(math.floor(number))
    end

    -- sanity checks
    assert(type(name) == "string" and #name > 0)

    return self.objects[name]
end

-- register in callback manager
CallbackManager:addListener("SceneManager", 0, SceneManager)

-- register in input manager
InputManager:addListener("SceneManager", 0, SceneManager)
