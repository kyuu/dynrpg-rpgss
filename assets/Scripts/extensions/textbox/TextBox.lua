require "system"
require "utility"
require "extensions.cache"

TextBox = class {
    __name = "TextBox",
    
    palette = {
        0xFFFFFFFF, -- 1st color: white
        0xFF0000FF, -- 2nd color: red
        0x008000FF, -- 3rd color: green
        0x0000FFFF, -- 4th color: blue
        0xFFFF00FF  -- 5th color: yellow
    },
    
    scratchpad = graphics.newImage(
        game.screen.width,
        game.screen.height
    )
}

function TextBox:__init(fontname, z, x, y, text)
    self.text = ""
    self.contents = nil

    self.scene = "map"
    
    self.coordsys = "screen"

    self.font = Cache:font(fontname)
    
    self.visible = true
    
    self.z = z or 0
    
    self.pos = {
        x = Tween:new(x or 160),
        y = Tween:new(y or 120)
    }
    
    self.scaling = Tween:new(1.0)
    
    self.opacity = Tween:new(255)
    
    if text then
        self:setText(text)
    end
end

function TextBox:update(scene, dt)
    if scene == self.scene then
        -- update position
        self.pos.x:update(dt)
        self.pos.y:update(dt)

        -- update scaling
        self.scaling:update(dt)

        -- update opacity
        self.opacity:update(dt)
    end
    
    -- don't remove us
    return false
end

function TextBox:render(scene)
    if scene == self.scene and self.visible and self.contents then
        local x = self.pos.x:getValue() - (self.contents.width  * self.scaling:getValue() / 2)
        local y = self.pos.y:getValue() - (self.contents.height * self.scaling:getValue() / 2)
        
        if self.coordsys == "map" then
            local cx, cy = game.map.getCameraPosition()
            x = x - cx
            y = y - cy
        end
        
        game.screen.draw(
            self.contents,
            x,
            y,
            0.0,
            self.scaling:getValue(),
            graphics.packColor(
                255,
                255,
                255,
                self.opacity:getValue()
            )
        )
    end
end

function TextBox:parseText(text)
    text = text:gsub(
        "\\v%[%d+%]",
        function(m)
            local id = tonumber(m:gmatch("%d+")())
            return tostring(game.variables[id])
        end
    )
    
    text = text:gsub(
        "\\n%[%d+%]",
        function(m)
            local id = tonumber(m:gmatch("%d+")())
            return game.actors[id].name
        end
    )
    
    text = text:gsub(
        "\n",
        "\\br"
    )
    
    local actions = {}
    
    for m, i, j in text:gmatch("(()\\c%[%d+%]())") do
        local id = tonumber(m:gmatch("%d+")())
        table.insert(
            actions,
            {
                type = "change color",
                i = i,
                j = j,
                color = TextBox.palette[id]
            }
        )
    end
    
    for m, i, j in text:gmatch("(()\\br())") do
        table.insert(
            actions,
            {
                type = "new line",
                i = i,
                j = j
            }
        )
    end
    
    table.sort(
        actions,
        function(a, b)
            return a.i < b.i
        end
    )
    
    local chunks = {}
    
    do
        local i = 1
        for _, action in ipairs(actions) do
            if i < action.i then
                table.insert(
                    chunks,
                    {
                        type = "text",
                        text = text:sub(i, action.i-1)
                    }
                )
            end
            table.insert(
                chunks,
                action
            )
            i = action.j
        end
        if i <= #text then
            table.insert(
                chunks,
                {
                    type = "text",
                    text = text:sub(i, #text)
                }
            )
        end
    end
    
    return chunks
end

function TextBox:setText(text)
    assert(type(text) == "string", "invalid text")
    
    if #text == 0 then
        self.text = ""
        self.contents = nil
        return
    end
    
    local scratchpad = TextBox.scratchpad
    scratchpad:clear(0x00000000)

    local chunks = self:parseText(text)
    
    local cur = {
        color = TextBox.palette[1],
        x = 0,
        y = 0,
        j = 0,
    }
    
    for _, chunk in ipairs(chunks) do
        if chunk.type == "text" then
            scratchpad:drawText(
                self.font,
                cur.x,
                cur.y,
                chunk.text,
                1.0,
                cur.color
            )
            cur.x = cur.x + self.font:getTextWidth(chunk.text)
            cur.j = math.max(cur.j, cur.x)
        elseif chunk.type == "new line" then
            cur.x = 0
            cur.y = cur.y + self.font.maxCharHeight
        elseif chunk.type == "change color" then
            cur.color = chunk.color
        end
    end
    
    self.text = text
    self.contents = scratchpad:copyRect(
        0,
        0,
        math.min(cur.j, scratchpad.width),
        math.min(cur.y + self.font.maxCharHeight, scratchpad.height)
    )
end

function TextBox:getPosition()
    return self.pos.x:getValue(), self.pos.y:getValue()
end

function TextBox:setPosition(x, y)
    self.pos.x:setValue(x)
    self.pos.y:setValue(y)
end

function TextBox:getScaling()
    return self.scaling:getValue()
end

function TextBox:setScaling(scaling)
    -- keep scaling >= 0
    if scaling < 0 then
        scaling = 0
    end
    
    self.scaling:setValue(scaling)
end

function TextBox:getOpacity()
    return self.opacity:getValue()
end

function TextBox:setOpacity(opacity)
    -- keep opacity in the range [0, 255]
    opacity = math.max(math.min(opacity, 255), 0)
    
    self.opacity:setValue(opacity)
end

function TextBox:move(relative, x, y, ms, easing)
    if relative == true then
        x = self.pos.x:getValue() + x
        y = self.pos.y:getValue() + y
    end
    self.pos.x:tweenTo(x, ms, easing)
    self.pos.y:tweenTo(y, ms, easing)
end

function TextBox:scale(relative, scaling, ms)
    if relative == true then
        scaling = self.scaling:getValue() + scaling
    end

    -- keep scaling >= 0
    if scaling < 0 then
        scaling = 0
    end
    
    self.scaling:tweenTo(scaling, ms)
end

function TextBox:fade(relative, opacity, ms)
    if relative == true then
        opacity = self.opacity:getValue() + opacity
    end

    -- keep opacity in the range [0, 255]
    opacity = math.max(math.min(opacity, 255), 0)
    
    self.opacity:tweenTo(opacity, ms)
end
