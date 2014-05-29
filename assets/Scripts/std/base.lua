

function typeof(var)
    local t = type(var)
    if t ~= "table" and t ~= "userdata" then
        return t
    end
    local mt = getmetatable(var)
    if mt ~= nil and type(mt.__classname) == "string" then
        return mt.__classname
    end
    return t
end

function serialize(obj)
    local mfile = io.newMemoryFile()
    local writer = io.newWriter(mfile)

    local classname = typeof(obj)
    local class = _G[classname]

    if class == nil or class.__serialize == nil then
        -- object not serializable
        error("Objects of type '"..classname.."' not serializable.")
    end

    local objdata = class.__serialize(obj)

    -- write class name
    writer:writeUint32(#classname)
    writer:writeString(classname)

    -- write object data
    writer:writeUint32(#objdata)
    writer:writeBytes(objdata)

    return mfile:copyBuffer()
end

function deserialize(data)
    local mfile = io.newMemoryFile(data)
    local reader = io.newReader(mfile)
    
    -- read class name
    local classname = reader:readString(reader:readUint32())
    
    -- read object data
    local objdata = reader:readBytes(reader:readUint32())
    
    assert(#classname > 0 and not mfile.eof)

    local class = _G[classname]
    
    assert(class ~= nil and class.__deserialize ~= nil)
    
    return class.__deserialize(objdata)
end
