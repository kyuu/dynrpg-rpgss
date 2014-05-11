require "std.tween"


Sprite = {
    __classname = "Sprite",
    
    __serialize = function(obj)
        local mfile = io.newMemoryFile()
        local writer = io.newWriter(mfile)
        
        -- write sprite data
        writer:writeUint32(#obj._filename)
        writer:writeString(obj._filename)
        
        writer:writeUint32(#obj._coord_sys)
        writer:writeString(obj._coord_sys)
        
        writer:writeUint32(#obj._blend_mode)
        writer:writeString(obj._blend_mode)
        
        writer:writeBool(obj._visible)
        writer:writeInt32(obj._z)
        writer:writeInt32(obj._x:get())
        writer:writeInt32(obj._y:get())
        writer:writeDouble(obj._scale:get())
        writer:writeDouble(obj._angle:get())
        
        -- write rotation parameters (currently only forever rotation supported)
        writer:writeBool(obj._rotation.forever)
        
        writer:writeUint32(#obj._rotation.direction)
        writer:writeString(obj._rotation.direction)
        
        writer:writeUint32(obj._rotation.ms)
        
        return mfile:copyBuffer()
    end,
    
    __deserialize = function(obj_data)
        local mfile = io.newMemoryFile(obj_data)
        local reader = io.newReader(mfile)
        
        -- read sprite data
        local filename = reader:readString(reader:readUint32())
        local coord_sys = reader:readString(reader:readUint32())
        local blend_mode = reader:readString(reader:readUint32())
        local visible = reader:readBool()
        local z = reader:readInt32()
        local x = reader:readInt32()
        local y = reader:readInt32()
        local scale = reader:readDouble()
        local angle = reader:readDouble()
        
        -- create sprite
        local sprite = Sprite.new(filename, blend_mode, visible, z, x, y, scale, angle)
        
        -- read rotation parameters (currently only forever rotation supported)
        local rotation_forever = reader:readBool()
        local rotation_direction = reader:readString(reader:readUint32())
        local rotation_ms = reader:readUint32()
        
        -- init rotation
        if rotation_forever then
            sprite:_handle_infinite_rotation(rotation_direction, rotation_ms)
        end
        
        -- make sure we didn't try to read past the end of file
        if mfile.eof then
            return nil
        end
        
        return sprite
    end
}
Sprite.__index = Sprite

function Sprite.new(filename, blend_mode, visible, z, x, y, scale, angle)
    local self = setmetatable({}, Sprite)
    
    self._filename = filename
    self._texture = graphics.readImage(filename)
    self._coord_sys = "screen"
    self._blend_mode = blend_mode or "mix"
    self._visible = visible or true
    self._z = z or 0
    self._x = Tween.new(x or 0)
    self._y = Tween.new(y or 0)
    self._scale = Tween.new(scale or 1.0)
    self._angle = Tween.new(angle or 0.0)
    
    -- rotation parameters
    self._rotation = {
        forever = false,
        direction = "",
        angle = 0,
        ms = 0
    }
    
    assert(self._texture ~= nil)
    
    return self
end

function Sprite._stop_rotation(self)
    -- clear rotation parameters
    self._rotation.forever = false
    self._rotation.direction = ""
    self._rotation.angle = 0
    self._rotation.ms = 0
    -- stop any ongoing angle tweening
    self._angle:set(self._angle:get())
end

function Sprite._handle_infinite_rotation(self, direction, ms)
    self._rotation.forever = true
    self._rotation.direction = direction
    self._rotation.angle = 0
    self._rotation.ms = ms
    
    if direction == "cw" then
        if self._angle:get() < 360 and self._angle:get() > 0 then
            self._angle:tween_to(0, (self._angle:get() / 360) * ms)
        else
            self._angle:set(360)
            self._angle:tween_to(0, ms)
        end
    else
        if self._angle:get() > 0 and self._angle:get() < 360 then
            self._angle:tween_to(360, ((360 - self._angle:get()) / 360) * ms)
        else
            self._angle:set(0)
            self._angle:tween_to(360, ms)
        end
    end
end

function Sprite._handle_finite_rotation(self, direction, angle, ms)
    self._rotation.forever = false
    self._rotation.direction = ""
    self._rotation.angle = 0
    self._rotation.ms = 0

    if direction == "cw" then
        if self._angle:get() > 0 and self._angle:get() >= angle then
            self._angle:tween_to(self._angle:get() - angle, ms)
        elseif self._angle:get() > 0 then
            self._rotation.direction = "cw"
            self._rotation.angle = angle - self._angle:get()
            self._rotation.ms = (self._rotation.angle / angle) * ms
            self._angle:tween_to(0, ms - self._rotation.ms)
        else
            -- current angle is 0
            self._angle:set(360)
            if angle <= 360 then
                self._angle:tween_to(360 - angle, ms)
            else
                self._rotation.direction = "cw"
                self._rotation.angle = angle - 360
                self._rotation.ms = (self._rotation.angle / angle) * ms
                self._angle:tween_to(0, ms - self._rotation.ms)
            end
        end
    else
        if self._angle:get() < 360 and 360 - self._angle:get() >= angle then
            self._angle:tween_to(self._angle:get() + angle, ms)
        elseif self._angle:get() < 360 then
            self._rotation.direction = "ccw"
            self._rotation.angle = angle - (360 - self._angle:get())
            self._rotation.ms = (self._rotation.angle / angle) * ms
            self._angle:tween_to(360, ms - self._rotation.ms)
        else
            -- current angle is 360
            self._angle:set(0)
            if angle <= 360 then
                self._angle:tween_to(angle, ms)
            else
                self._rotation.direction = "ccw"
                self._rotation.angle = angle - 360
                self._rotation.ms = (self._rotation.angle / angle) * ms
                self._angle:tween_to(360, ms - self._rotation.ms)
            end
        end
    end
end

function Sprite.update(self, ms)
    self._x:update(ms)
    self._y:update(ms)
    self._scale:update(ms)
    
    if self._angle:update(ms) then
        if self._rotation.forever then
            self:_handle_infinite_rotation(self._rotation.direction, self._rotation.ms)
        elseif self._rotation.angle > 0 then
            self:_handle_finite_rotation(self._rotation.direction, self._rotation.angle, self._rotation.ms)
        end
    end
end

function Sprite.render(self)
    -- TODO: Allow sprites to be rendered in other scenes too.
    if game.scene == "map" and self._visible then
        local ox, oy
        if self._coord_sys == "map" then
            local cam_x, cam_y = game.map.getCameraPosition()
            ox = (self._texture.width * self._scale:get() / 2) + cam_x
            oy = (self._texture.height * self._scale:get() / 2) + cam_y
        elseif self._coord_sys == "screen" then
            ox = self._texture.width * self._scale:get() / 2
            oy = self._texture.height * self._scale:get() / 2
        end
        
        game.screen.draw(self._texture, self._x:get() - ox, self._y:get() - oy, self._angle:get(), self._scale:get(), 0xFFFFFFFF, self._blend_mode)
    end
end

function Sprite.get_texture(self)
    return self._filename
end

function Sprite.set_texture(self, filename)
    self._filename = filename
    self._texture = graphics.readImage(filename)
    assert(self._texture ~= nil)
end

function Sprite.get_blend_mode(self)
    return self._blend_mode
end

function Sprite.set_blend_mode(self, blend_mode)
    self._blend_mode = blend_mode
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
    return self._x:get(), self._y:get()
end

function Sprite.set_position(self, x, y)
    self._x:set(x)
    self._y:set(y)
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
    -- keep angle in the range 0-360
    angle = angle % 360
    if angle < 0 then
        angle = 360 + angle
    end
    
    self._angle:set(angle)
end

function Sprite.get_coordinate_system(self)
    return self._coord_sys
end

function Sprite.set_coordinate_system(self, coord_sys)
    -- invalid coordinate system constants default to "screen"
    if coord_sys == "map" then
        self._coord_sys = "map"
    else
        self._coord_sys = "screen"
    end
end

function Sprite.move_by(self, ox, oy, ms, easing)
    self._x:tween_to(self._x:get() + ox, ms, easing)
    self._y:tween_to(self._y:get() + oy, ms, easing)
end

function Sprite.move_to(self, x, y, ms, easing)
    self._x:tween_to(x, ms, easing)
    self._y:tween_to(y, ms, easing)
end

function Sprite.scale(self, scale, ms)
    -- keep scale >= 0
    if scale < 0 then
        scale = 0
    end
    
    self._scale:tween_to(scale, ms)
end

function Sprite.rotate_by(self, direction, angle, ms)
    self:_stop_rotation()
    if angle > 0 and ms > 0 then
        self:_handle_finite_rotation(direction, angle, ms)
    end
end

function Sprite.rotate_to(self, direction, angle, ms)
    self:_stop_rotation()
    
    -- adjust requested angle to the range 0-360
    angle = angle % 360
    if angle < 0 then
        angle = 360 + angle
    end
    
    if ms > 0 and self._angle:get() ~= angle then
        if direction == "cw" then
            if angle < self._angle:get() then
                self:_handle_finite_rotation(direction, self._angle:get() - angle, ms)
            else
                self:_handle_finite_rotation(direction, self._angle:get() + (360 - angle), ms)
            end
        else
            if angle > self._angle:get() then
                self:_handle_finite_rotation(direction, angle - self._angle:get(), ms)
            else
                self:_handle_finite_rotation(direction, angle + (360 - self._angle:get()), ms)
            end
        end
    else
        self:set_angle(angle)
    end
end

function Sprite.rotate_forever(self, direction, ms)
    self:_stop_rotation()
    if ms > 0 then
        self:_handle_infinite_rotation(direction, ms)
    end
end

function Sprite.stop_rotation(self)
    self:_stop_rotation()
end
