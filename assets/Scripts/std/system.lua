
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
