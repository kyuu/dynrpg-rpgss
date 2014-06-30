-------------------------------------------------------------------------------
-- Adds a new sprite.
--
-- @param name          (String) Name of the new sprite (e.g. "sprite1").
-- @param filename      (String) Filename of the image to use for the sprite.
--                               The filename is relative to the game's root
--                               directory (e.g. "Picture/sprite1.png").
-- @param blendmode     (String) Blend mode. One of graphics.BlendMode.
-- @param visible       (Number) Boolean (1 or 0) that determines if the sprite
--                               is visible.
-- @param z             (Number) Render priority of the sprite. A sprite with a
--                               higher z-value will be rendered before any
--                               other sprite with a lower z-value.
-- @param x             (Number) X-position in pixels.
-- @param y             (Number) Y-position in pixels.
-- @param scale         (Number) Scaling of the sprite in percent.
-- @param angle         (Number) Angle of the sprite in degrees.
-------------------------------------------------------------------------------
function add_sprite(name, filename, blendmode, visible, z, x, y, scale, angle)
    local sprite = Sprite:new(
        filename,
        blendmode,
        visible,
        z,
        x,
        y,
        (scale or 100) / 100,
        angle
    )
    SceneManager:addObject(name, sprite)
end

-------------------------------------------------------------------------------
-- Removes an existing sprite.
--
-- @param name          (String) Name of the sprite to remove.
-------------------------------------------------------------------------------
function remove_sprite(name)
    SceneManager:removeObject(name)
end

-------------------------------------------------------------------------------
-- Sets the scene of a sprite.
--
-- @param name          (String) Name of the sprite.
-- @param scene         (String) The new scene. One of game.Scene.
-------------------------------------------------------------------------------
function set_sprite_scene(name, scene)
    local sprite = SceneManager:getObject(name)
    if sprite ~= nil then
        sprite:setScene(scene)
    else
        error("sprite '"..name.."' does not exist")
    end
end

-------------------------------------------------------------------------------
-- Sets the image of a sprite.
--
-- @param name          (String) Name of the sprite.
-- @param filename      (String) Filename of the new image.
-------------------------------------------------------------------------------
function set_sprite_image(name, filename)
    local sprite = SceneManager:getObject(name)
    if sprite ~= nil then
        sprite:setTexture(filename)
    else
        error("sprite '"..name.."' does not exist")
    end
end

-------------------------------------------------------------------------------
-- Sets the blend mode of a sprite.
--
-- @param name          (String) Name of the sprite.
-- @param bmode         (String) The new blend mode. One of graphics.BlendMode.
-------------------------------------------------------------------------------
function set_sprite_blend_mode(name, blend_mode)
    local sprite = SceneManager:getObject(name)
    if sprite ~= nil then
        sprite:setBlendMode(blend_mode)
    else
        error("sprite '"..name.."' does not exist")
    end
end

-------------------------------------------------------------------------------
-- Binds a sprite to a coordinate system.
--
-- @param name          (String) Name of the sprite.
-- @param coordsys      (String) The coordinate system to bind the sprite to.
--                               Possible values are "screen", "mouse" or "map".
-------------------------------------------------------------------------------
function bind_sprite_to(name, coordsys)
    local sprite = SceneManager:getObject(name)
    if sprite ~= nil then
        sprite:bindTo(coordsys)
    else
        error("sprite '"..name.."' does not exist")
    end
end

-------------------------------------------------------------------------------
-- Gets the tile dimensions of a sprite.
--
-- @param name          (String) Name of the sprite.
-- @param var_w         (Number) ID of the variable that will hold the width.
-- @param var_h         (Number) ID of the variable that will hold the height.
-------------------------------------------------------------------------------
function get_sprite_tile_dimensions(name, var_w, var_h)
    local sprite = SceneManager:getObject(name)
    if sprite ~= nil then
        local w, h = sprite:getTileDimensions()
        game.variables[var_w] = w
        game.variables[var_h] = h
    else
        error("sprite '"..name.."' does not exist")
    end
end

-------------------------------------------------------------------------------
-- Sets the tile dimensions of a sprite.
--
-- @param name          (String) Name of the sprite.
-- @param w             (Number) The new tile width.
-- @param h             (Number) The new tile height.
-------------------------------------------------------------------------------
function set_sprite_tile_dimensions(name, w, h)
    local sprite = SceneManager:getObject(name)
    if sprite ~= nil then
        sprite:setTileDimensions(w, h)
    else
        error("sprite '"..name.."' does not exist")
    end
end

-------------------------------------------------------------------------------
-- Gets the tile location of a sprite.
--
-- @param name          (String) Name of the sprite.
-- @param var_x         (Number) ID of the variable that will hold the
--                               x-position of the tile.
-- @param var_y         (Number) ID of the variable that will hold the
--                               y-position of the tile.
-------------------------------------------------------------------------------
function get_sprite_tile(name, var_x, var_y)
    local sprite = SceneManager:getObject(name)
    if sprite ~= nil then
        local x, y = sprite:getTile()
        game.variables[var_x] = x
        game.variables[var_y] = y
    else
        error("sprite '"..name.."' does not exist")
    end
end

-------------------------------------------------------------------------------
-- Sets the tile location of a sprite.
--
-- @param name          (String) Name of the sprite.
-- @param x             (Number) The x-position of the new tile.
-- @param y             (Number) The y-position of the new tile.
-------------------------------------------------------------------------------
function set_sprite_tile(name, x, y)
    local sprite = SceneManager:getObject(name)
    if sprite ~= nil then
        sprite:setTile(x, y)
    else
        error("sprite '"..name.."' does not exist")
    end
end

-------------------------------------------------------------------------------
-- Gets the value that determines if a sprite is visible.
--
-- @param name          (String) Name of the sprite.
-- @param var           (Number) ID of the variable that will hold the Boolean
--                               value (1 or 0) that determines if the sprite
--                               is visible.
-------------------------------------------------------------------------------
function get_sprite_visible(name, var)
    local sprite = SceneManager:getObject(name)
    if sprite ~= nil then
        game.variables[var] = sprite:getVisible()
    else
        error("sprite '"..name.."' does not exist")
    end
end

-------------------------------------------------------------------------------
-- Sets the value that determines if a sprite is visible.
--
-- @param name          (String) Name of the sprite.
-- @param visible       (Number) Boolean (1 or 0) that determines if the sprite
--                               is visible.
-------------------------------------------------------------------------------
function set_sprite_visible(name, visible)
    local sprite = SceneManager:getObject(name)
    if sprite ~= nil then
        sprite:setVisible(visible ~= 0)
    else
        error("sprite '"..name.."' does not exist")
    end
end

-------------------------------------------------------------------------------
-- Gets the z-value of a sprite.
--
-- @param name          (String) Name of the sprite.
-- @param var           (Number) ID of the variable that will hold the z-value.
-------------------------------------------------------------------------------
function get_sprite_z(name, var)
    local sprite = SceneManager:getObject(name)
    if sprite ~= nil then
        game.variables[var] = sprite:getZ()
    else
        error("sprite '"..name.."' does not exist")
    end
end

-------------------------------------------------------------------------------
-- Sets the z-value of a sprite.
--
-- @param name          (String) Name of the sprite.
-- @param z             (Number) The new z-value.
-------------------------------------------------------------------------------
function set_sprite_z(name, z)
    local sprite = SceneManager:getObject(name)
    if sprite ~= nil then
        sprite:setZ(z)
    else
        error("sprite '"..name.."' does not exist")
    end
end

-------------------------------------------------------------------------------
-- Gets the position of a sprite.
--
-- @param name          (String) Name of the sprite.
-- @param var_x         (Number) ID of the variable that will hold the x-position.
-- @param var_y         (Number) ID of the variable that will hold the y-position.
-------------------------------------------------------------------------------
function get_sprite_position(name, var_x, var_y)
    local sprite = SceneManager:getObject(name)
    if sprite ~= nil then
        local x, y = sprite:getPosition()
        game.variables[var_x] = x
        game.variables[var_y] = y
    else
        error("sprite '"..name.."' does not exist")
    end
end

-------------------------------------------------------------------------------
-- Sets the position of a sprite.
--
-- @param name          (String) Name of the sprite.
-- @param x             (Number) The new x-position.
-- @param y             (Number) The new y-position.
-------------------------------------------------------------------------------
function set_sprite_position(name, x, y)
    local sprite = SceneManager:getObject(name)
    if sprite ~= nil then
        sprite:setPosition(x, y)
    else
        error("sprite '"..name.."' does not exist")
    end
end

-------------------------------------------------------------------------------
-- Gets the scaling of a sprite.
--
-- @param name          (String) Name of the sprite.
-- @param var           (Number) ID of the variable that will hold the scaling
--                               in percent.
-------------------------------------------------------------------------------
function get_sprite_scale(name, var)
    local sprite = SceneManager:getObject(name)
    if sprite ~= nil then
        game.variables[var] = sprite:getScale() * 100
    else
        error("sprite '"..name.."' does not exist")
    end
end

-------------------------------------------------------------------------------
-- Sets the scaling of a sprite.
--
-- @param name          (String) Name of the sprite.
-- @param scale         (Number) The new scaling in percent.
-------------------------------------------------------------------------------
function set_sprite_scale(name, scale)
    local sprite = SceneManager:getObject(name)
    if sprite ~= nil then
        sprite:setScale(scale / 100)
    else
        error("sprite '"..name.."' does not exist")
    end
end

-------------------------------------------------------------------------------
-- Gets the angle of a sprite.
--
-- @param name          (String) Name of the sprite.
-- @param var           (Number) ID of the variable that will hold the angle in
--                               degrees.
-------------------------------------------------------------------------------
function get_sprite_angle(name, var)
    local sprite = SceneManager:getObject(name)
    if sprite ~= nil then
        game.variables[var] = sprite:getAngle()
    else
        error("sprite '"..name.."' does not exist")
    end
end

-------------------------------------------------------------------------------
-- Sets the angle of a sprite.
--
-- @param name          (String) Name of the sprite.
-- @param scale         (Number) The new angle in degrees.
-------------------------------------------------------------------------------
function set_sprite_angle(name, angle)
    local sprite = SceneManager:getObject(name)
    if sprite ~= nil then
        sprite:setAngle(angle)
    else
        error("sprite '"..name.."' does not exist")
    end
end

-------------------------------------------------------------------------------
-- Moves a sprite by an offset, i.e. relative to its current position.
--
-- @param name          (String) Name of the sprite.
-- @param ox            (Number) Offset on the x-axis.
-- @param oy            (Number) Offset on the y-axis.
-- @param ms            (Number) Duration in milliseconds.
-- @param easing        (String) Type of interpolation. See Tween.ease for all
--                               possible values.
-------------------------------------------------------------------------------
function move_sprite_by(name, ox, oy, ms, easing)
    local sprite = SceneManager:getObject(name)
    if sprite ~= nil then
        sprite:moveBy(ox, oy, ms, easing)
    else
        error("sprite '"..name.."' does not exist")
    end
end

-------------------------------------------------------------------------------
-- Moves a sprite to a new position.
--
-- @param name          (String) Name of the sprite.
-- @param x             (Number) The new x-position.
-- @param y             (Number) The new y-position.
-- @param ms            (Number) Duration in milliseconds.
-- @param easing        (String) Type of interpolation. See Tween.ease for all
--                               possible values.
-------------------------------------------------------------------------------
function move_sprite_to(name, x, y, ms, easing)
    local sprite = SceneManager:getObject(name)
    if sprite ~= nil then
        sprite:moveTo(x, y, ms, easing)
    else
        error("sprite '"..name.."' does not exist")
    end
end

-------------------------------------------------------------------------------
-- Scales a sprite.
--
-- @param name          (String) Name of the sprite.
-- @param scale         (Number) The new scaling in percent.
-- @param ms            (Number) Duration in milliseconds.
-------------------------------------------------------------------------------
function scale_sprite_to(name, scale, ms)
    local sprite = SceneManager:getObject(name)
    if sprite ~= nil then
        sprite:scaleTo(scale / 100, ms)
    else
        error("sprite '"..name.."' does not exist")
    end
end

-------------------------------------------------------------------------------
-- Rotates a sprite by an angle, relative to its current angle.
--
-- @param name          (String) Name of the sprite.
-- @param direction     (String) Direction of rotation. Possible values are
--                               "ccw" (counter-clockwise) and "cw" (clockwise).
-- @param angle         (Number) Angle in degrees. A negative value will result
--                               in a counter-clockwise rotation. Values also
--                               can be < -360 and > 360.
-- @param ms            (Number) Duration in milliseconds.
-------------------------------------------------------------------------------
function rotate_sprite_by(name, direction, angle, ms)
    local sprite = SceneManager:getObject(name)
    if sprite ~= nil then
        sprite:rotateBy(direction, angle, ms)
    else
        error("sprite '"..name.."' does not exist")
    end
end

-------------------------------------------------------------------------------
-- Rotates a sprite to an angle.
--
-- @param name          (String) Name of the sprite.
-- @param direction     (String) Direction of rotation. Possible values are
--                               "ccw" (counter-clockwise) and "cw" (clockwise).
-- @param angle         (Number) The new angle.
-- @param ms            (Number) Duration in milliseconds.
-------------------------------------------------------------------------------
function rotate_sprite_to(name, direction, angle, ms)
    local sprite = SceneManager:getObject(name)
    if sprite ~= nil then
        sprite:rotateTo(direction, angle, ms)
    else
        error("sprite '"..name.."' does not exist")
    end
end

-------------------------------------------------------------------------------
-- Starts rotating a sprite forever.
--
-- @param name          (String) Name of the sprite.
-- @param direction     (String) Direction of rotation. Possible values are
--                               "ccw" (counter-clockwise) and "cw" (clockwise).
-- @param ms            (Number) Duration in milliseconds a full rotation
--                               should take.
-------------------------------------------------------------------------------
function rotate_sprite_forever(name, direction, ms)
    local sprite = SceneManager:getObject(name)
    if sprite ~= nil then
        sprite:rotateForever(direction, ms)
    else
        error("sprite '"..name.."' does not exist")
    end
end

-------------------------------------------------------------------------------
-- Stops any ongoing rotation of a sprite.
--
-- @param name          (String) Name of the sprite.
-------------------------------------------------------------------------------
function stop_sprite_rotation(name)
    local sprite = SceneManager:getObject(name)
    if sprite ~= nil then
        sprite:stopRotation()
    else
        error("sprite '"..name.."' does not exist")
    end
end

-------------------------------------------------------------------------------
-- Gets the sprite color.
--
-- @param name          (String) Name of the sprite.
-- @param var_r         (Number) ID of the variable that will hold the red channel.
-- @param var_g         (Number) ID of the variable that will hold the green channel.
-- @param var_b         (Number) ID of the variable that will hold the blue channel.
-------------------------------------------------------------------------------
function get_sprite_color(name, var_r, var_g, var_b)
    local sprite = SceneManager:getObject(name)
    if sprite ~= nil then
        local r, g, b = sprite:getColor()
        game.variables[var_r] = r
        game.variables[var_g] = g
        game.variables[var_b] = b
    else
        error("sprite '"..name.."' does not exist")
    end
end

-------------------------------------------------------------------------------
-- Sets the sprite color.
--
-- @param name          (String) Name of the sprite.
-- @param r             (Number) The new red channel of the color.
-- @param g             (Number) The new green channel of the color.
-- @param b             (Number) The new blue channel of the color.
-------------------------------------------------------------------------------
function set_sprite_color(name, r, g, b)
    local sprite = SceneManager:getObject(name)
    if sprite ~= nil then
        sprite:setColor(r, g, b)
    else
        error("sprite '"..name.."' does not exist")
    end
end

-------------------------------------------------------------------------------
-- Shifts the sprite color.
--
-- @param name          (String) Name of the sprite.
-- @param r             (Number) The new red channel of the color.
-- @param g             (Number) The new green channel of the color.
-- @param b             (Number) The new blue channel of the color.
-- @param ms            (Number) Duration in milliseconds.
-------------------------------------------------------------------------------
function shift_sprite_color_to(name, r, g, b, ms)
    local sprite = SceneManager:getObject(name)
    if sprite ~= nil then
        sprite:shiftColorTo(r, g, b, ms)
    else
        error("sprite '"..name.."' does not exist")
    end
end

-------------------------------------------------------------------------------
-- Gets the sprite opacity.
--
-- @param name          (String) Name of the sprite.
-- @param var           (Number) ID of the variable that will hold the opacity.
-------------------------------------------------------------------------------
function get_sprite_opacity(name, var)
    local sprite = SceneManager:getObject(name)
    if sprite ~= nil then
        local opacity = sprite:getOpacity()
        game.variables[var] = opacity
    else
        error("sprite '"..name.."' does not exist")
    end
end

-------------------------------------------------------------------------------
-- Sets the sprite opacity.
--
-- @param name          (String) Name of the sprite.
-- @param opacity       (Number) The new opacity.
-------------------------------------------------------------------------------
function set_sprite_opacity(name, opacity)
    local sprite = SceneManager:getObject(name)
    if sprite ~= nil then
        sprite:setOpacity(opacity)
    else
        error("sprite '"..name.."' does not exist")
    end
end

-------------------------------------------------------------------------------
-- Shifts the sprite opacity.
--
-- @param name          (String) Name of the sprite.
-- @param opacity       (Number) The new opacity.
-- @param ms            (Number) Duration in milliseconds.
-------------------------------------------------------------------------------
function shift_sprite_opacity_to(name, opacity, ms)
    local sprite = SceneManager:getObject(name)
    if sprite ~= nil then
        sprite:shiftOpacityTo(opacity, ms)
    else
        error("sprite '"..name.."' does not exist")
    end
end
