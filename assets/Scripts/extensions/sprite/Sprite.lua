require "system"
require "utility.Tween"

Sprite = class {
    __name = "Sprite",
    
    __serialize = function(obj)
        local mfile = io.newMemoryFile()
        local writer = io.newWriter(mfile)

        -- write scene
        writer:writeUint32(#obj.scene)
        writer:writeString(obj.scene)

        -- write filename
        writer:writeUint32(#obj.filename)
        writer:writeString(obj.filename)

        -- write tile dimensions, location
        writer:writeUint32(obj.tile.w)
        writer:writeUint32(obj.tile.h)
        writer:writeUint32(obj.tile.x)
        writer:writeUint32(obj.tile.y)

        -- write coordinate system
        writer:writeUint32(#obj.coordsys)
        writer:writeString(obj.coordsys)

        -- write blend mode
        writer:writeUint32(#obj.blendmode)
        writer:writeString(obj.blendmode)

        -- write visible
        writer:writeBool(obj.visible)

        -- write z
        writer:writeInt32(obj.z)

        -- write position
        local x_data = serialize(obj.pos.x)
        local y_data = serialize(obj.pos.y)
        writer:writeUint32(#x_data)
        writer:writeBytes(x_data)
        writer:writeUint32(#y_data)
        writer:writeBytes(y_data)

        -- write scaling
        local scaling_data = serialize(obj.scaling)
        writer:writeUint32(#scaling_data)
        writer:writeBytes(scaling_data)

        -- write angle
        local angle_data = serialize(obj.angle)
        writer:writeUint32(#angle_data)
        writer:writeBytes(angle_data)

        -- write rotation state
        writer:writeUint32(#obj.rot.dir)
        writer:writeString(obj.rot.dir)
        writer:writeDouble(obj.rot.deg)
        writer:writeDouble(obj.rot.ms)

        -- write color
        local r_data = serialize(obj.color.r)
        local g_data = serialize(obj.color.g)
        local b_data = serialize(obj.color.b)
        writer:writeUint32(#r_data)
        writer:writeBytes(r_data)
        writer:writeUint32(#g_data)
        writer:writeBytes(g_data)
        writer:writeUint32(#b_data)
        writer:writeBytes(b_data)
        
        -- write opacity
        local opacity_data = serialize(obj.opacity)
        writer:writeUint32(#opacity_data)
        writer:writeBytes(opacity_data)

        return mfile:copyBuffer()
    end,

    __deserialize = function(obj_data)
        local mfile = io.newMemoryFile(obj_data)
        local reader = io.newReader(mfile)

        -- create instance
        local obj = setmetatable({}, Sprite)

        -- read scene
        obj.scene = reader:readString(reader:readUint32())

        -- read filename
        obj.filename = reader:readString(reader:readUint32())

        -- load texture
        obj.texture = Cache:image(obj.filename)

        -- read tile dimensions, location
        obj.tile = {}
        obj.tile.w = reader:readUint32()
        obj.tile.h = reader:readUint32()
        obj.tile.x = reader:readUint32()
        obj.tile.y = reader:readUint32()

        -- read coordinate system
        obj.coordsys = reader:readString(reader:readUint32())

        -- read blend mode
        obj.blendmode = reader:readString(reader:readUint32())

        -- read visible
        obj.visible = reader:readBool()

        -- read z
        obj.z = reader:readInt32()

        -- read position
        local x_data = reader:readBytes(reader:readUint32())
        local y_data = reader:readBytes(reader:readUint32())
        obj.pos = {}
        obj.pos.x = deserialize(x_data)
        obj.pos.y = deserialize(y_data)

        -- read scaling
        local scaling_data = reader:readBytes(reader:readUint32())
        obj.scaling = deserialize(scaling_data)

        -- read angle
        local angle_data = reader:readBytes(reader:readUint32())
        obj.angle = deserialize(angle_data)

        -- read rotation state
        obj.rot = {}
        obj.rot.dir = reader:readString(reader:readUint32())
        obj.rot.deg = reader:readDouble()
        obj.rot.ms = reader:readDouble()

        -- read color
        local r_data = reader:readBytes(reader:readUint32())
        local g_data = reader:readBytes(reader:readUint32())
        local b_data = reader:readBytes(reader:readUint32())
        obj.color = {}
        obj.color.r = deserialize(r_data)
        obj.color.g = deserialize(g_data)
        obj.color.b = deserialize(b_data)

        -- read opacity
        local opacity_data = reader:readBytes(reader:readUint32())
        obj.opacity = deserialize(opacity_data)

        -- check for EOF
        if mfile.eof then
            error("EOF")
        end

        return obj
    end
}

function Sprite:__init(filename, blendmode, z, x, y)
    self.scene = "map"
    
    self.filename = filename
    
    self.texture = Cache:image(filename)
    
    self.tile = {
        -- dimensions
        w = self.texture.width,
        h = self.texture.height,
        
        -- location
        x = 0,
        y = 0
    }
    
    self.coordsys = "screen"
    
    self.blendmode = blendmode or "mix"
    
    self.visible = true
    
    self.z = z or 0
    
    self.pos = {
        x = Tween:new(x or 160),
        y = Tween:new(y or 120)
    }
    
    self.scaling = Tween:new(1.0)
    
    self.angle = Tween:new(0.0)
    
    self.rot = {
        dir = "",
        deg = 0,
        ms = 0
    }
    
    self.color = {
        r = Tween:new(255),
        g = Tween:new(255),
        b = Tween:new(255)
    }
    
    self.opacity = Tween:new(255)
end

function Sprite:update(scene, ms)
    if scene == self.scene then
        -- update position
        self.pos.x:update(ms)
        self.pos.y:update(ms)

        -- update scaling
        self.scaling:update(ms)

        -- update rotation
        if self.angle:update(ms) then -- tweening finished
            self:consumeRotation() -- if rotation in progress, continue
        end
        
        -- update color
        self.color.r:update(ms)
        self.color.g:update(ms)
        self.color.b:update(ms)
        
        -- update opacity
        self.opacity:update(ms)
    end
    
    -- don't remove us
    return false
end

function Sprite:render(scene)
    if scene == self.scene and self.visible then
        local x = self.pos.x:getValue() - (self.tile.w * self.scaling:getValue() / 2)
        local y = self.pos.y:getValue() - (self.tile.h * self.scaling:getValue() / 2)
        
        if self.coordsys == "mouse" then
            local mouse_x, mouse_y = mouse.getPosition()
            x = mouse_x + x
            y = mouse_y + y
        elseif self.coordsys == "map" then
            local cam_x, cam_y = game.map.getCameraPosition()
            x = x - cam_x
            y = y - cam_y
        end
        
        game.screen.draw(
            self.texture,
            self.tile.x * self.tile.w,
            self.tile.y * self.tile.h,
            self.tile.w,
            self.tile.h,
            x,
            y,
            self.angle:getValue(),
            self.scaling:getValue(),
            graphics.packColor(
                self.color.r:getValue(),
                self.color.g:getValue(),
                self.color.b:getValue(),
                self.opacity:getValue()
            ),
            self.blendmode
        )
    end
end

function Sprite:getScene()
    return self.scene
end

function Sprite:setScene(scene)
    self.scene = scene
end

function Sprite:getBoundTo()
    return self.coordsys
end

function Sprite:bindTo(coordsys)
    self.coordsys = coordsys
end

function Sprite:getTexture()
    return self.filename
end

function Sprite:setTexture(filename)
    self.filename = filename
    
    -- load texture
    self.texture = Cache:image(filename)
    
    -- reset tile dimensions, location
    self.tile.w = self.texture.width
    self.tile.h = self.texture.height
    self.tile.x = 0
    self.tile.y = 0
end

function Sprite:getTileDimensions()
    return self.tile.w, self.tile.h
end

function Sprite:setTileDimensions(w, h)
    -- sanity checks
    assert(self.texture.width % w == 0)
    assert(self.texture.height % h == 0)
    
    -- set new tile dimensions
    self.tile.w = w
    self.tile.h = h
    
    -- reset tile location
    self.tile.x = 0
    self.tile.y = 0
end

function Sprite:getTile()
    return self.tile.x, self.tile.y
end

function Sprite:setTile(x, y)
    -- sanity checks
    assert(x >= 0 and x < self.texture.width / self.tile.w)
    assert(y >= 0 and y < self.texture.height / self.tile.h)
    
    -- set new tile location
    self.tile.x = x
    self.tile.y = y
end

function Sprite:getBlendMode()
    return self.blendmode
end

function Sprite:setBlendMode(blendmode)
    self.blendmode = blendmode
end

function Sprite:getVisible()
    return self.visible
end

function Sprite:setVisible(visible)
    self.visible = visible
end

function Sprite:getZ()
    return self.z
end

function Sprite:setZ(z)
    self.z = z
end

function Sprite:getPosition()
    return self.pos.x:getValue(), self.pos.y:getValue()
end

function Sprite:setPosition(x, y)
    self.pos.x:setValue(x)
    self.pos.y:setValue(y)
end

function Sprite:getScaling()
    return self.scaling:getValue()
end

function Sprite:setScaling(scaling)
    -- keep scaling >= 0
    if scaling < 0 then
        scaling = 0
    end
    
    self.scaling:setValue(scaling)
end

function Sprite:getAngle()
    return self.angle:getValue()
end

function Sprite:setAngle(angle)
    -- keep angle in the range [0, 360]
    angle = angle % 360
    if angle < 0 then
        angle = 360 + angle
    end
    
    self.angle:setValue(angle)
end

function Sprite:getColor()
    return self.color.r:getValue(),
           self.color.g:getValue(),
           self.color.b:getValue()
end

function Sprite:setColor(r, g, b)
    -- keep color channels in the range [0, 255]
    r = math.max(math.min(r, 255), 0)
    g = math.max(math.min(g, 255), 0)
    b = math.max(math.min(b, 255), 0)
    
    self.color.r:setValue(r)
    self.color.g:setValue(g)
    self.color.b:setValue(b)
end

function Sprite:getOpacity()
    return self.opacity:getValue()
end

function Sprite:setOpacity(opacity)
    -- keep opacity in the range [0, 255]
    opacity = math.max(math.min(opacity, 255), 0)
    
    self.opacity:setValue(opacity)
end

function Sprite:move(relative, x, y, ms, easing)
    if relative == true then
        x = self.pos.x:getValue() + x
        y = self.pos.y:getValue() + y
    end
    self.pos.x:tweenTo(x, ms, easing)
    self.pos.y:tweenTo(y, ms, easing)
end

function Sprite:scale(relative, scaling, ms)
    if relative == true then
        scaling = self.scaling:getValue() + scaling
    end

    -- keep scaling >= 0
    if scaling < 0 then
        scaling = 0
    end
    
    self.scaling:tweenTo(scaling, ms)
end

function Sprite:consumeRotation()
    if self.rot.deg < 0 then -- infinite rotation in progress
        if self.rot.dir == "cw" then
            if self.angle:getValue() < 360 and self.angle:getValue() > 0 then
                self.angle:tweenTo(0, (self.angle:getValue() / 360) * self.rot.ms)
            else
                self.angle:setValue(360)
                self.angle:tweenTo(0, self.rot.ms)
            end
        elseif self.rot.dir == "ccw" then
            if self.angle:getValue() > 0 and self.angle:getValue() < 360 then
                self.angle:tweenTo(360, ((360 - self.angle:getValue()) / 360) * self.rot.ms)
            else
                self.angle:setValue(0)
                self.angle:tweenTo(360, self.rot.ms)
            end
        end
    elseif self.rot.deg > 0 then -- finite rotation in progress
        local degrees = self.rot.deg
        local ms = self.rot.ms
        if self.rot.dir == "cw" then
            if self.angle:getValue() > 0 and self.angle:getValue() >= degrees then
                self.angle:tweenTo(self.angle:getValue() - self.rot.deg, self.rot.ms)
                -- all rotation consumed
                self.rot.dir = ""
                self.rot.deg = 0
                self.rot.ms = 0
            elseif self.angle:getValue() > 0 then
                self.rot.deg = degrees - self.angle:getValue()
                self.rot.ms = (self.rot.deg / degrees) * ms
                self.angle:tweenTo(0, ms - self.rot.ms)
            else -- current angle is 0
                self.angle:setValue(360)
                if degrees <= 360 then
                    self.angle:tweenTo(360 - degrees, ms)
                    -- all rotation consumed
                    self.rot.dir = ""
                    self.rot.deg = 0
                    self.rot.ms = 0
                else
                    self.rot.deg = degrees - 360
                    self.rot.ms = (self.rot.deg / degrees) * ms
                    self.angle:tweenTo(0, ms - self.rot.ms)
                end
            end
        elseif self.rot.dir == "ccw" then
            if self.angle:getValue() < 360 and 360 - self.angle:getValue() >= degrees then
                self.angle:tweenTo(self.angle:getValue() + degrees, ms)
                -- all rotation consumed
                self.rot.dir = ""
                self.rot.deg = 0
                self.rot.ms = 0
            elseif self.angle:getValue() < 360 then
                self.rot.deg = degrees - (360 - self.angle:getValue())
                self.rot.ms = (self.rot.deg / degrees) * ms
                self.angle:tweenTo(360, ms - self.rot.ms)
            else -- current angle is 360
                self.angle:setValue(0)
                if degrees <= 360 then
                    self.angle:tweenTo(degrees, ms)
                    -- all rotation consumed
                    self.rot.dir = ""
                    self.rot.deg = 0
                    self.rot.ms = 0
                else
                    self.rot.deg = degrees - 360
                    self.rot.ms = (self.rot.deg / degrees) * ms
                    self.angle:tweenTo(360, ms - self.rot.ms)
                end
            end
        end
    end
end

function Sprite:rotateBy(degrees, ms)
    if ms > 0 and degrees ~= 0 then
        if degrees < 0 then
            self.rot.dir = "cw"
            self.rot.deg = -degrees
        else
            self.rot.dir = "ccw"
            self.rot.deg = degrees
        end
        self.rot.ms = ms
        self:consumeRotation()
    else
        self:setAngle(self.angle:getValue() + degrees)
    end
end

function Sprite:rotateTo(direction, angle, ms)
    -- adjust requested angle to the range [0, 360]
    angle = angle % 360
    if angle < 0 then
        angle = 360 + angle
    end
    
    if ms > 0 and self.angle:getValue() ~= angle then
        self.rot.dir = direction
        if direction == "cw" then
            if angle < self.angle:getValue() then
                self.rot.deg = self.angle:getValue() - angle
            else
                self.rot.deg = self.angle:getValue() + (360 - angle)
            end
        elseif direction == "ccw" then
            if angle > self.angle:getValue() then
                self.rot.deg = angle - self.angle:getValue()
            else
                self.rot.deg = angle + (360 - self.angle:getValue())
            end
        else
            self.rot.deg = 0
        end
        self.rot.ms = ms
        self:consumeRotation()
    else
        self:setAngle(angle)
    end
end

function Sprite:rotateForever(direction, ms)
    self.rot.dir = direction
    self.rot.deg = -1 -- negative degrees means infinite rotation
    self.rot.ms = ms
    self:consumeRotation()
end

function Sprite:stopRotation()
    self.rot.dir = ""
    self.rot.deg = 0
    self.rot.ms = 0
    self.angle:setValue(self.angle:getValue())
end

function Sprite:colorize(r, g, b, ms)
    -- keep color channels in the range [0, 255]
    r = math.max(math.min(r, 255), 0)
    g = math.max(math.min(g, 255), 0)
    b = math.max(math.min(b, 255), 0)
    
    self.color.r:tweenTo(r, ms)
    self.color.g:tweenTo(g, ms)
    self.color.b:tweenTo(b, ms)
end

function Sprite:fade(relative, opacity, ms)
    if relative == true then
        opacity = self.opacity:getValue() + opacity
    end

    -- keep opacity in the range [0, 255]
    opacity = math.max(math.min(opacity, 255), 0)
    
    self.opacity:tweenTo(opacity, ms)
end

