require "std.system"


World = {
    __classname = "World"
}
World.__index = World

function World.new()
    local self = setmetatable({}, World)
    
    self._objects = {}
    
    return self
end

function World.saveState(self)
    local mfile = io.newMemoryFile()
    local writer = io.newWriter(mfile)
    
    for name, obj in pairs(self._objects) do
        local classname = typeof(obj)
        local class = _G[classname]
        
        if class ~= nil and class.__serialize ~= nil then
            -- write object's name
            writer:writeUint32(#name)
            writer:writeString(name)

            -- write object's class name
            writer:writeUint32(#classname)
            writer:writeString(classname)
            
            -- serialize object
            local obj_data = class.__serialize(obj)
            
            -- write object data
            writer:writeUint32(#obj_data)
            writer:writeBytes(obj_data)
        else
            -- object not serializable, treat as fatal error
            error("Object '"..name.."' not serializable.")
        end
    end
    
    return mfile:copyBuffer()
end

function World.loadState(self, data)
    -- clear current state
    self._objects = {}
    
    local mfile = io.newMemoryFile(data)
    local reader = io.newReader(mfile)
    
    while mfile:tell() ~= #mfile do
        -- read name
        local name = reader:readString(reader:readUint32())
        
        -- read class name
        local classname = reader:readString(reader:readUint32())
        
        -- get class
        local class = _G[classname]
        
        if class ~= nil and class.__deserialize ~= nil then
            -- read object data
            local obj_data = reader:readBytes(reader:readUint32())
            
            -- make sure we didn't try to read past the end of file
            if mfile.eof then
                error("EOF while reading object '"..name.."'.")
            end
            
            -- deserialize object
            local obj = class.__deserialize(obj_data)
            
            if obj == nil then
                error("Error deserializing object '"..name.."'.")
            end
            
            -- add object
            self._objects[name] = obj
        else
            -- object not deserializable, treat as fatal error
            error("Object '"..name.."' not deserializable.")
        end
    end
end

function World.update(self)
    -- TODO: Expose RPG::Screen::millisecondsPerFrame and use it here,
    --       because it's possible to change the frame rate in DynRPG.
    local ms = 1000 / 60

    for name, obj in pairs(self._objects) do
        if obj.update ~= nil then
            if obj:update(ms) then
                -- Object told us to remove it from the world.
                self._objects[name] = nil
            end
        end
    end
end

function World.render(self)
    local render_list = {}
    
    -- Find all drawable objects.
    for _, obj in pairs(self._objects) do
        if obj.render ~= nil then
            table.insert(render_list, obj)
        end
    end
    
    -- Sort them with respect to their z value.
    table.sort(render_list, function(a, b) return a.z < b.z end)
    
    -- Render them.
    for _, obj in ipairs(render_list) do
        obj:render()
    end
end

function World.addObject(self, name, obj)
    self._objects[name] = obj
end

function World.removeObject(self, name)
    self._objects[name] = nil
end

function World.getObject(self, name)
    return self._objects[name]
end
