require "std.tween"


Sprite = {
    __classname = "Sprite",
    
    __serialize = function(obj)
        local mfile = io.newMemoryFile()
        local writer = io.newWriter(mfile)

        -- write scene
        writer:writeUint32(#obj._scene)
        writer:writeString(obj._scene)

        -- write filename
        writer:writeUint32(#obj._filename)
        writer:writeString(obj._filename)

        -- write source rect
        writer:writeInt32(obj._srect.x)
        writer:writeInt32(obj._srect.y)
        writer:writeUint32(obj._srect.w)
        writer:writeUint32(obj._srect.h)

        -- write coordinate system
        writer:writeUint32(#obj._coord_sys)
        writer:writeString(obj._coord_sys)

        -- write blend mode
        writer:writeUint32(#obj._bmode)
        writer:writeString(obj._bmode)

        -- write visible
        writer:writeBool(obj._visible)

        -- write z
        writer:writeInt32(obj._z)

        -- write position
        local x_data = serialize(obj._pos.x)
        local y_data = serialize(obj._pos.y)
        writer:writeUint32(#x_data)
        writer:writeBytes(x_data)
        writer:writeUint32(#y_data)
        writer:writeBytes(y_data)

        -- write scale
        local scale_data = serialize(obj._scale)
        writer:writeUint32(#scale_data)
        writer:writeBytes(scale_data)

        -- write angle
        local angle_data = serialize(obj._angle)
        writer:writeUint32(#angle_data)
        writer:writeBytes(angle_data)

        -- write rotation state
        writer:writeUint32(#obj._rot.dir)
        writer:writeString(obj._rot.dir)
        writer:writeDouble(obj._rot.deg)
        writer:writeDouble(obj._rot.ms)

        -- write color
        local r_data = serialize(obj._col.r)
        local g_data = serialize(obj._col.g)
        local b_data = serialize(obj._col.b)
        local a_data = serialize(obj._col.a)
        writer:writeUint32(#r_data)
        writer:writeBytes(r_data)
        writer:writeUint32(#g_data)
        writer:writeBytes(g_data)
        writer:writeUint32(#b_data)
        writer:writeBytes(b_data)
        writer:writeUint32(#a_data)
        writer:writeBytes(a_data)

        return mfile:copyBuffer()
    end,

    __deserialize = function(data)
        local mfile = io.newMemoryFile(data)
        local reader = io.newReader(mfile)

        -- create instance
        local obj = setmetatable({}, Sprite)

        -- read scene
        obj._scene = reader:readString(reader:readUint32())

        -- read filename
        obj._filename = reader:readString(reader:readUint32())

        -- load texture
        if not io.exists(obj._filename) then
            error("file '"..obj._filename.."' does not exist")
        end

        obj._texture = graphics.readImage(obj._filename)

        if obj._texture == nil then
            error("error loading image '"..obj._filename.."'")
        end

        -- read source rect
        obj._srect = {}
        obj._srect.x = reader:readInt32()
        obj._srect.y = reader:readInt32()
        obj._srect.w = reader:readUint32()
        obj._srect.h = reader:readUint32()

        -- read coordinate system
        obj._coord_sys = reader:readString(reader:readUint32())

        -- read blend mode
        obj._bmode = reader:readString(reader:readUint32())

        -- read visible
        obj._visible = reader:readBool()

        -- read z
        obj._z = reader:readInt32()

        -- read position
        local x_data = reader:readBytes(reader:readUint32())
        local y_data = reader:readBytes(reader:readUint32())
        obj._pos = {}
        obj._pos.x = deserialize(x_data)
        obj._pos.y = deserialize(y_data)

        -- read scale
        local scale_data = reader:readBytes(reader:readUint32())
        obj._scale = deserialize(scale_data)

        -- read angle
        local angle_data = reader:readBytes(reader:readUint32())
        obj._angle = deserialize(angle_data)

        -- read rotation state
        obj._rot = {}
        obj._rot.dir = reader:readString(reader:readUint32())
        obj._rot.deg = reader:readDouble()
        obj._rot.ms = reader:readDouble()

        -- read color
        local r_data = reader:readBytes(reader:readUint32())
        local g_data = reader:readBytes(reader:readUint32())
        local b_data = reader:readBytes(reader:readUint32())
        local a_data = reader:readBytes(reader:readUint32())
        obj._col = {}
        obj._col.r = deserialize(r_data)
        obj._col.g = deserialize(g_data)
        obj._col.b = deserialize(b_data)
        obj._col.a = deserialize(a_data)

        -- check for EOF
        if mfile.eof then
            error("EOF")
        end

        return obj
    end
}
Sprite.__index = Sprite

function Sprite.new(filename, bmode, visible, z, x, y, scale, angle)
    local self = setmetatable({}, Sprite)
    
    self._scene = "map"
    
    self._filename = filename
    
    if not io.exists(filename) then
        error("file '"..filename.."' does not exist")
    end
    
    self._texture = graphics.readImage(filename)
    
    if self._texture == nil then
        error("error loading image '"..filename.."'")
    end
    
    self._srect = {
        x = 0,
        y = 0,
        w = self._texture.width,
        h = self._texture.height
    }
    
    self._coord_sys = "screen"
    
    self._bmode = bmode or "mix"
    
    self._visible = visible or true
    
    self._z = z or 0
    
    self._pos = {
        x = Tween.new(x or 0),
        y = Tween.new(y or 0)
    }
    
    self._scale = Tween.new(scale or 1.0)
    
    self._angle = Tween.new(angle or 0.0)
    
    self._rot = {
        dir = "",
        deg = 0,
        ms = 0
    }
    
    self._col = {
        r = Tween.new(255),
        g = Tween.new(255),
        b = Tween.new(255),
        a = Tween.new(255)
    }
    
    return self
end

function Sprite.update(self, scene, ms)
    if scene == self._scene then
        -- update position
        self._pos.x:update(ms)
        self._pos.y:update(ms)

        -- update scaling
        self._scale:update(ms)

        -- update rotation
        if self._angle:update(ms) then -- tweening finished
            self:_rotate() -- if rotation in progress, continue
        end
        
        -- update color
        self._col.r:update(ms)
        self._col.g:update(ms)
        self._col.b:update(ms)
        self._col.a:update(ms)
    end
    
    -- don't remove us
    return false
end

function Sprite.render(self, scene)
    if scene == self._scene and self._visible then
        local x = self._pos.x:get() - (self._srect.w * self._scale:get() / 2)
        local y = self._pos.y:get() - (self._srect.h * self._scale:get() / 2)
        
        if self._coord_sys == "mouse" then
            local mouse_x, mouse_y = mouse.getPosition()
            x = mouse_x + x
            y = mouse_y + y
        elseif self._coord_sys == "map" then
            local cam_x, cam_y = game.map.getCameraPosition()
            x = x - cam_x
            y = y - cam_y
        end
        
        game.screen.draw(
            self._texture,
            self._srect.x,
            self._srect.y,
            self._srect.w,
            self._srect.h,
            x,
            y,
            self._angle:get(),
            self._scale:get(),
            graphics.packColor(
                self._col.r:get(),
                self._col.g:get(),
                self._col.b:get(),
                self._col.a:get()
            ),
            self._bmode
        )
    end
end

function Sprite.get_scene(self)
    return self._scene
end

function Sprite.set_scene(self, scene)
    self._scene = scene
end

function Sprite.get_coord_sys(self)
    return self._coord_sys
end

function Sprite.bind_to(self, coord_sys)
    self._coord_sys = coord_sys
end

function Sprite.get_texture(self)
    return self._filename
end

function Sprite.set_texture(self, filename)
    self._filename = filename
    
    -- load texture
    if not io.exists(filename) then
        error("file '"..filename.."' does not exist")
    end
    
    self._texture = graphics.readImage(filename)
    
    if self._texture == nil then
        error("error loading image '"..filename.."'")
    end
    
    -- reset source rect
    self:set_source_rect(
        0,
        0,
        self._texture.width,
        self._texture.height
    )
end

function Sprite.get_source_rect(self)
    return self._srect.x,
           self._srect.y,
           self._srect.w,
           self._srect.h
end

function Sprite.set_source_rect(self, x, y, w, h)
    -- make sure source rect is valid
    if x < 0 or
       y < 0 or
       x + w > self._texture.width or
       y + h > self._texture.height then
        error("invalid source rect")
    end
    
    self._srect.x = x
    self._srect.y = y
    self._srect.w = w
    self._srect.h = h
end

function Sprite.get_blend_mode(self)
    return self._bmode
end

function Sprite.set_blend_mode(self, blend_mode)
    self._bmode = blend_mode
end

function Sprite.get_visible(self)
    return self._visible
end

function Sprite.set_visible(self, visible)
    self._visible = visible
end

function Sprite.get_z(self)
    return self._z
end

function Sprite.set_z(self, z)
    self._z = z
end

function Sprite.get_position(self)
    return self._pos.x:get(), self._pos.y:get()
end

function Sprite.set_position(self, x, y)
    self._pos.x:set(x)
    self._pos.y:set(y)
end

function Sprite.get_scale(self)
    return self._scale:get()
end

function Sprite.set_scale(self, scale)
    -- keep scale >= 0
    if scale < 0 then
        scale = 0
    end
    
    self._scale:set(scale)
end

function Sprite.get_angle(self)
    return self._scale:get()
end

function Sprite.set_angle(self, angle)
    -- keep angle in the range [0, 360]
    angle = angle % 360
    if angle < 0 then
        angle = 360 + angle
    end
    
    self._angle:set(angle)
end

function Sprite.move_by(self, ox, oy, ms, easing)
    self._pos.x:tween_to(self._pos.x:get() + ox, ms, easing)
    self._pos.y:tween_to(self._pos.y:get() + oy, ms, easing)
end

function Sprite.move_to(self, x, y, ms, easing)
    self._pos.x:tween_to(x, ms, easing)
    self._pos.y:tween_to(y, ms, easing)
end

function Sprite.scale_to(self, scale, ms)
    -- keep scale >= 0
    if scale < 0 then
        scale = 0
    end
    
    self._scale:tween_to(scale, ms)
end

function Sprite._rotate(self)
    if self._rot.deg < 0 then -- infinite rotation in progress
        if self._rot.dir == "cw" then
            if self._angle:get() < 360 and self._angle:get() > 0 then
                self._angle:tween_to(0, (self._angle:get() / 360) * self._rot.ms)
            else
                self._angle:set(360)
                self._angle:tween_to(0, self._rot.ms)
            end
        elseif self._rot.dir == "ccw" then
            if self._angle:get() > 0 and self._angle:get() < 360 then
                self._angle:tween_to(360, ((360 - self._angle:get()) / 360) * self._rot.ms)
            else
                self._angle:set(0)
                self._angle:tween_to(360, self._rot.ms)
            end
        end
    elseif self._rot.deg > 0 then -- finite rotation in progress
        local degrees = self._rot.deg
        local ms = self._rot.ms
        if self._rot.dir == "cw" then
            if self._angle:get() > 0 and self._angle:get() >= degrees then
                self._angle:tween_to(self._angle:get() - self._rot.deg, self._rot.ms)
                self._rot.deg = 0 -- rotation finished
            elseif self._angle:get() > 0 then
                self._rot.deg = degrees - self._angle:get()
                self._rot.ms = (self._rot.deg / degrees) * ms
                self._angle:tween_to(0, ms - self._rot.ms)
            else -- current angle is 0
                self._angle:set(360)
                if degrees <= 360 then
                    self._angle:tween_to(360 - degrees, ms)
                    self._rot.deg = 0 -- rotation finished
                else
                    self._rot.deg = degrees - 360
                    self._rot.ms = (self._rot.deg / degrees) * ms
                    self._angle:tween_to(0, ms - self._rot.ms)
                end
            end
        elseif self._rot.dir == "ccw" then
            if self._angle:get() < 360 and 360 - self._angle:get() >= degrees then
                self._angle:tween_to(self._angle:get() + degrees, ms)
                self._rot.deg = 0 -- rotation finished
            elseif self._angle:get() < 360 then
                self._rot.deg = degrees - (360 - self._angle:get())
                self._rot.ms = (self._rot.deg / degrees) * ms
                self._angle:tween_to(360, ms - self._rot.ms)
            else -- current angle is 360
                self._angle:set(0)
                if degrees <= 360 then
                    self._angle:tween_to(degrees, ms)
                    self._rot.deg = 0 -- rotation finished
                else
                    self._rot.deg = degrees - 360
                    self._rot.ms = (self._rot.deg / degrees) * ms
                    self._angle:tween_to(360, ms - self._rot.ms)
                end
            end
        end
    end
end

function Sprite.rotate_by(self, degrees, ms)
    if ms > 0 and degrees ~= 0 then
        if degrees < 0 then
            self._rot.dir = "cw"
            self._rot.deg = -degrees
        else
            self._rot.dir = "ccw"
            self._rot.deg = degrees
        end
        self._rot.ms = ms
        self:_rotate()
    else
        self:set_angle(self._angle:get() + degrees)
    end
end

function Sprite.rotate_to(self, direction, angle, ms)
    -- adjust requested angle to the range [0, 360]
    angle = angle % 360
    if angle < 0 then
        angle = 360 + angle
    end
    
    if ms > 0 and self._angle:get() ~= angle then
        self._rot.dir = direction
        if direction == "cw" then
            if angle < self._angle:get() then
                self._rot.deg = self._angle:get() - angle
            else
                self._rot.deg = self._angle:get() + (360 - angle)
            end
        elseif direction == "ccw" then
            if angle > self._angle:get() then
                self._rot.deg = angle - self._angle:get()
            else
                self._rot.deg = angle + (360 - self._angle:get())
            end
        else
            self._rot.deg = 0
        end
        self._rot.ms = ms
        self:_rotate()
    else
        self:set_angle(angle)
    end
end

function Sprite.rotate_forever(self, direction, ms)
    self._rot.dir = direction
    self._rot.deg = -1 -- negative degrees means infinite rotation
    self._rot.ms = ms
    self:_rotate()
end

function Sprite.stop_rotation(self)
    self._rot.deg = 0
    self._angle:stop_tween()
end

function Sprite.get_color(self)
    return self._col.r:get(),
           self._col.g:get(),
           self._col.b:get(),
           self._col.a:get()
end

function Sprite.set_color(self, r, g, b, a)
    -- keep color channels in the range [0, 255]
    r = math.max(math.min(r, 255), 0)
    g = math.max(math.min(g, 255), 0)
    b = math.max(math.min(b, 255), 0)
    a = math.max(math.min(a, 255), 0)
    
    self._col.r:set(r)
    self._col.g:set(g)
    self._col.b:set(b)
    self._col.a:set(a)
end

function Sprite.shift_color_to(self, r, g, b, a, ms)
    -- keep color channels in the range [0, 255]
    r = math.max(math.min(r, 255), 0)
    g = math.max(math.min(g, 255), 0)
    b = math.max(math.min(b, 255), 0)
    a = math.max(math.min(a, 255), 0)
    
    self._col.r:tween_to(r, ms)
    self._col.g:tween_to(g, ms)
    self._col.b:tween_to(b, ms)
    self._col.a:tween_to(a, ms)
end
