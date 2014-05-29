require "std.base"


World = {
    __classname = "World",
    
    _objects = {}
}

function World.reset(self)
    self._objects = {}
end

function World.save(self)
    local mfile = io.newMemoryFile()
    local writer = io.newWriter(mfile)
    
    -- write number of objects
    local nobjects = 0
    for name, obj in pairs(self._objects) do
        nobjects = nobjects + 1
    end
    writer:writeUint32(nobjects)
    
    -- write objects
    for name, obj in pairs(self._objects) do
        -- write object name
        writer:writeUint32(#name)
        writer:writeString(name)

        -- write object data
        local objdata = serialize(obj)
        writer:writeUint32(#objdata)
        writer:writeBytes(objdata)
    end

    return mfile:copyBuffer()
end

function World.load(self, data)
    local mfile = io.newMemoryFile(data)
    local reader = io.newReader(mfile)

    -- read number of objects
    local nobjects = reader:readUint32()

    -- read objects
    for i = 1, nobjects do
        -- read object name
        local name = reader:readString(reader:readUint32())

        -- read object data
        local objdata = reader:readBytes(reader:readUint32())

        -- check for EOF
        if mfile.eof then
            error("EOF")
        end

        self._objects[name] = deserialize(objdata)
    end
end

function World.update(self, scene)
    -- TODO: Expose RPG::Screen::millisecondsPerFrame and use it here,
    --       because it's possible to change the frame rate in DynRPG.
    local ms = 1000 / 60

    for name, obj in pairs(self._objects) do
        if obj.update ~= nil then
            if obj:update(scene, ms) then
                -- Object told us to remove it from the world.
                self._objects[name] = nil
            end
        end
    end
end

function World.render(self, scene)
    local render_list = {}
    
    -- Find all drawable objects.
    for _, obj in pairs(self._objects) do
        if obj.render ~= nil then
            table.insert(render_list, obj)
        end
    end
    
    -- Sort them by their z value in descending order, i.e.
    -- objects with a greater z value will be drawn first.
    table.sort(render_list, function(a, b) return a:get_z() > b:get_z() end)
    
    -- Render them.
    for _, obj in ipairs(render_list) do
        obj:render(scene)
    end
end

function World.add_object(self, name, obj)
    self._objects[name] = obj
end

function World.remove_object(self, name)
    self._objects[name] = nil
end

function World.get_object(self, name)
    return self._objects[name]
end
