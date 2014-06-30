do
    local pn = ...
    local t = {}
    for _, fn in ipairs(io.enumerate("Scripts/"..pn:gsub("%.", "/"))) do
        local i, j = fn:find("%.[^%.]*$")
        if i then
            fn = fn:sub(1, i - 1)
        end
        if fn ~= "init" then
            t[fn] = 1
        end
    end
    for mn, _ in pairs(t) do
        require(pn.."."..mn)
    end
end
