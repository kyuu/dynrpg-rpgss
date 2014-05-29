require "std"

-------------------------------------------------------------------------------
-- Adds a new sprite.
--
-- Parameters
-- [Name]       [Type]      [Description]
-- name         String      Name of the new sprite.
-- filename     String      Image filename.
-- bmode        String      Blend mode.
-- visible      Number      Boolean (1 or 0) that determines the visibility.
-- z            Number      Render priority.
-- x            Number      X position in pixels.
-- y            Number      Y position in pixels.
-- scale        Number      Scaling of the sprite in percent.
-- angle        Number      Angle of the sprite in degrees.
-------------------------------------------------------------------------------
function add_sprite(name, filename, bmode, visible, z, x, y, scale, angle)
    local sprite = Sprite.new(
        filename,
        bmode,
        visible,
        z,
        x,
        y,
        (scale or 100) / 100,
        angle
    )
    World:add_object(name, sprite)
end

-------------------------------------------------------------------------------
-- Removes an existing sprite.
--
-- [Name]       [Type]      [Description]
-- Name         Type        Description
-- name         String      Name of the sprite.
-------------------------------------------------------------------------------
function remove_sprite(name)
    World:remove_object(name)
end

-------------------------------------------------------------------------------
-- Sets the scene for a sprite.
--
-- Parameters
-- [Name]       [Type]      [Description]
-- name         String      Name of the sprite.
-- scene        String      The new scene.
-------------------------------------------------------------------------------
function set_sprite_scene(name, scene)
    local sprite = World:get_object(name)
    if sprite ~= nil then
        sprite:set_scene(scene)
    else
        error("sprite '"..name.."' does not exist")
    end
end

-------------------------------------------------------------------------------
-- Sets a new image for a sprite.
--
-- Parameters
-- [Name]       [Type]      [Description]
-- name         String      Name of the sprite.
-- filename     String      Filename of the new image.
-------------------------------------------------------------------------------
function set_sprite_image(name, filename)
    local sprite = World:get_object(name)
    if sprite ~= nil then
        sprite:set_texture(filename)
    else
        error("sprite '"..name.."' does not exist")
    end
end

-------------------------------------------------------------------------------
-- Gets the source rectangle of a sprite.
--
-- Parameters
-- [Name]       [Type]      [Description]
-- name         String      Name of the sprite.
-- var_x        Number      ID of the variable that will hold the x-position.
-- var_y        Number      ID of the variable that will hold the y-position.
-- var_w        Number      ID of the variable that will hold the width.
-- var_h        Number      ID of the variable that will hold the height.
-------------------------------------------------------------------------------
function get_sprite_source_rect(name, var_x, var_y, var_w, var_h)
    local sprite = World:get_object(name)
    if sprite ~= nil then
        local x, y, w, h = sprite:get_source_rect()
        game.variables[var_x] = x
        game.variables[var_y] = y
        game.variables[var_w] = w
        game.variables[var_h] = h
    else
        error("sprite '"..name.."' does not exist")
    end
end

-------------------------------------------------------------------------------
-- Sets the source rectangle for a sprite.
--
-- Parameters
-- [Name]       [Type]      [Description]
-- name         String      Name of the sprite.
-- x            Number      The new x-position of the source rectangle.
-- y            Number      The new y-position of the source rectangle.
-- w            Number      The new width of the source rectangle.
-- h            Number      The new height of the source rectangle.
-------------------------------------------------------------------------------
function set_sprite_source_rect(name, x, y, w, h)
    local sprite = World:get_object(name)
    if sprite ~= nil then
        sprite:set_source_rect(x, y, w, h)
    else
        error("sprite '"..name.."' does not exist")
    end
end

-------------------------------------------------------------------------------
-- Sets the blend mode for a sprite.
--
-- Parameters
-- [Name]       [Type]      [Description]
-- name         String      Name of the sprite.
-- bmode        String      The new blend mode.
-------------------------------------------------------------------------------
function set_sprite_blend_mode(name, blend_mode)
    local sprite = World:get_object(name)
    if sprite ~= nil then
        sprite:set_blend_mode(blend_mode)
    else
        error("sprite '"..name.."' does not exist")
    end
end

-------------------------------------------------------------------------------
-- Binds a sprite to a coordinate system.
--
-- Parameters
-- [Name]       [Type]      [Description]
-- name         String      Name of the sprite.
-- coord_sys    String      The new coordinate system.
-------------------------------------------------------------------------------
function bind_sprite_to(name, coord_sys)
    local sprite = World:get_object(name)
    if sprite ~= nil then
        sprite:bind_to(coord_sys)
    else
        error("sprite '"..name.."' does not exist")
    end
end

-------------------------------------------------------------------------------
-- Gets the visibility of a sprite.
--
-- Parameters
-- [Name]       [Type]      [Description]
-- name         String      Name of the sprite.
-- var          Number      ID of the variable that will hold the visibility.
-------------------------------------------------------------------------------
function get_sprite_visible(name, var)
    local sprite = World:get_object(name)
    if sprite ~= nil then
        game.variables[var] = sprite:get_visible()
    else
        error("sprite '"..name.."' does not exist")
    end
end

-------------------------------------------------------------------------------
-- Sets the visibility for a sprite.
--
-- Parameters
-- [Name]       [Type]      [Description]
-- name         String      Name of the sprite.
-- visible      Number      Boolean (0 or 1) that determines if the sprite is
--                          visible.
-------------------------------------------------------------------------------
function set_sprite_visible(name, visible)
    local sprite = World:get_object(name)
    if sprite ~= nil then
        sprite:set_visible(visible ~= 0)
    else
        error("sprite '"..name.."' does not exist")
    end
end

-------------------------------------------------------------------------------
-- Gets the z value of a sprite.
--
-- Parameters
-- [Name]       [Type]      [Description]
-- name         String      Name of the sprite.
-- var          Number      ID of the variable that will hold the z value.
-------------------------------------------------------------------------------
function get_sprite_z(name, var)
    local sprite = World:get_object(name)
    if sprite ~= nil then
        game.variables[var] = sprite:get_z()
    else
        error("sprite '"..name.."' does not exist")
    end
end

-------------------------------------------------------------------------------
-- Sets the z value for a sprite.
--
-- Parameters
-- [Name]       [Type]      [Description]
-- name         String      Name of the sprite.
-- z            Number      The new z value.
-------------------------------------------------------------------------------
function set_sprite_z(name, z)
    local sprite = World:get_object(name)
    if sprite ~= nil then
        sprite:set_z(z)
    else
        error("sprite '"..name.."' does not exist")
    end
end

-------------------------------------------------------------------------------
-- Gets the position of a sprite.
--
-- Parameters
-- [Name]       [Type]      [Description]
-- name         String      Name of the sprite.
-- var_x        Number      ID of the variable that will hold the x-position.
-- var_y        Number      ID of the variable that will hold the y-position.
-------------------------------------------------------------------------------
function get_sprite_position(name, var_x, var_y)
    local sprite = World:get_object(name)
    if sprite ~= nil then
        local x, y = sprite:get_position()
        game.variables[var_x] = x
        game.variables[var_y] = y
    else
        error("sprite '"..name.."' does not exist")
    end
end

-------------------------------------------------------------------------------
-- Sets the position for a sprite.
--
-- Parameters
-- [Name]       [Type]      [Description]
-- name         String      Name of the sprite.
-- x            Number      The new x-position.
-- y            Number      The new y-position.
-------------------------------------------------------------------------------
function set_sprite_position(name, x, y)
    local sprite = World:get_object(name)
    if sprite ~= nil then
        sprite:set_position(x, y)
    else
        error("sprite '"..name.."' does not exist")
    end
end

-------------------------------------------------------------------------------
-- Gets the scaling of a sprite.
--
-- Parameters
-- [Name]       [Type]      [Description]
-- name         String      Name of the sprite.
-- var          Number      ID of the variable that will hold the scaling.
-------------------------------------------------------------------------------
function get_sprite_scale(name, var)
    local sprite = World:get_object(name)
    if sprite ~= nil then
        game.variables[var] = sprite:get_scale() * 100
    else
        error("sprite '"..name.."' does not exist")
    end
end

-------------------------------------------------------------------------------
-- Sets the scaling for a sprite.
--
-- Parameters
-- [Name]       [Type]      [Description]
-- name         String      Name of the sprite.
-- scale        Number      The new scaling in percent.
-------------------------------------------------------------------------------
function set_sprite_scale(name, scale)
    local sprite = World:get_object(name)
    if sprite ~= nil then
        sprite:set_scale(scale / 100)
    else
        error("sprite '"..name.."' does not exist")
    end
end

-------------------------------------------------------------------------------
-- Gets the angle of a sprite.
--
-- Parameters
-- [Name]       [Type]      [Description]
-- name         String      Name of the sprite.
-- var          Number      ID of the variable that will hold the angle.
-------------------------------------------------------------------------------
function get_sprite_angle(name, var)
    local sprite = World:get_object(name)
    if sprite ~= nil then
        game.variables[var] = sprite:get_angle()
    else
        error("sprite '"..name.."' does not exist")
    end
end

-------------------------------------------------------------------------------
-- Sets the angle for a sprite.
--
-- Parameters
-- [Name]       [Type]      [Description]
-- name         String      Name of the sprite.
-- scale        Number      The new angle.
-------------------------------------------------------------------------------
function set_sprite_angle(name, angle)
    local sprite = World:get_object(name)
    if sprite ~= nil then
        sprite:set_angle(angle)
    else
        error("sprite '"..name.."' does not exist")
    end
end

-------------------------------------------------------------------------------
-- Moves a sprite by an offset, i.e. relative to its current position.
--
-- Parameters
-- [Name]       [Type]      [Description]
-- name         String      Name of the sprite.
-- ox           Number      Offset on the x-axis.
-- oy           Number      Offset on the y-axis.
-- ms           Number      Duration in milliseconds.
-- easing       String      Type of interpolation.
-------------------------------------------------------------------------------
function move_sprite_by(name, ox, oy, ms, easing)
    local sprite = World:get_object(name)
    if sprite ~= nil then
        sprite:move_by(ox, oy, ms, easing)
    else
        error("sprite '"..name.."' does not exist")
    end
end

-------------------------------------------------------------------------------
-- Moves a sprite to a new position.
--
-- Parameters
-- [Name]       [Type]      [Description]
-- name         String      Name of the sprite.
-- x            Number      The new x-position.
-- y            Number      The new y-position.
-- ms           Number      Duration in milliseconds.
-- easing       String      Type of interpolation.
-------------------------------------------------------------------------------
function move_sprite_to(name, x, y, ms, easing)
    local sprite = World:get_object(name)
    if sprite ~= nil then
        sprite:move_to(x, y, ms, easing)
    else
        error("sprite '"..name.."' does not exist")
    end
end

-------------------------------------------------------------------------------
-- Scales a sprite.
--
-- Parameters
-- [Name]       [Type]      [Description]
-- name         String      Name of the sprite.
-- scale        Number      The new scaling in percent.
-- ms           Number      Duration in milliseconds.
-------------------------------------------------------------------------------
function scale_sprite_to(name, scale, ms)
    local sprite = World:get_object(name)
    if sprite ~= nil then
        sprite:scale_to(scale / 100, ms)
    else
        error("sprite '"..name.."' does not exist")
    end
end

-------------------------------------------------------------------------------
-- Rotates a sprite by an angle.
--
-- Parameters
-- [Name]       [Type]      [Description]
-- name         String      Name of the sprite.
-- direction    String      Direction of rotation.
-- angle        Number      Angle in degrees.
-- ms           Number      Duration in milliseconds.
-------------------------------------------------------------------------------
function rotate_sprite_by(name, direction, angle, ms)
    local sprite = World:get_object(name)
    if sprite ~= nil then
        sprite:rotate_by(direction, angle, ms)
    else
        error("sprite '"..name.."' does not exist")
    end
end

-------------------------------------------------------------------------------
-- Rotates a sprite to an angle.
--
-- Parameters
-- [Name]       [Type]      [Description]
-- name         String      Name of the sprite.
-- direction    String      Direction of rotation.
-- angle        Number      The new angle.
-- ms           Number      Duration in milliseconds.
-------------------------------------------------------------------------------
function rotate_sprite_to(name, direction, angle, ms)
    local sprite = World:get_object(name)
    if sprite ~= nil then
        sprite:rotate_to(direction, angle, ms)
    else
        error("sprite '"..name.."' does not exist")
    end
end

-------------------------------------------------------------------------------
-- Initiates an infinite sprite rotation.
--
-- Parameters
-- [Name]       [Type]      [Description]
-- name         String      Name of the sprite.
-- direction    String      Direction of rotation.
-- ms           Number      Duration in milliseconds for a full rotation.
-------------------------------------------------------------------------------
function rotate_sprite_forever(name, direction, ms)
    local sprite = World:get_object(name)
    if sprite ~= nil then
        sprite:rotate_forever(direction, ms)
    else
        error("sprite '"..name.."' does not exist")
    end
end

-------------------------------------------------------------------------------
-- Stops an ongoing sprite rotation, if any.
--
-- Parameters
-- [Name]       [Type]      [Description]
-- name         String      Name of the sprite.
-------------------------------------------------------------------------------
function stop_sprite_rotation(name)
    local sprite = World:get_object(name)
    if sprite ~= nil then
        sprite:stop_rotation()
    else
        error("sprite '"..name.."' does not exist")
    end
end

-------------------------------------------------------------------------------
-- Gets the color a sprite will be multiplied with when rendered.
--
-- Parameters
-- [Name]       [Type]      [Description]
-- name         String      Name of the sprite.
-- var_r        Number      ID of the variable that will hold the red channel.
-- var_g        Number      ID of the variable that will hold the green channel.
-- var_b        Number      ID of the variable that will hold the blue channel.
-- var_a        Number      ID of the variable that will hold the alpha channel.
-------------------------------------------------------------------------------
function get_sprite_color(name, var_r, var_g, var_b, var_a)
    local sprite = World:get_object(name)
    if sprite ~= nil then
        local r, g, b, a = sprite:get_color()
        game.variables[var_r] = r
        game.variables[var_g] = g
        game.variables[var_b] = b
        game.variables[var_a] = a
    else
        error("sprite '"..name.."' does not exist")
    end
end

-------------------------------------------------------------------------------
-- Sets the color a sprite will be multiplied with when rendered.
--
-- Parameters
-- [Name]       [Type]      [Description]
-- name         String      Name of the sprite.
-- r            Number      The new red channel of the color.
-- g            Number      The new green channel of the color.
-- b            Number      The new blue channel of the color.
-- a            Number      The new alpha channel of the color.
-------------------------------------------------------------------------------
function set_sprite_color(name, r, g, b, a)
    local sprite = World:get_object(name)
    if sprite ~= nil then
        sprite:set_color(r, g, b, a)
    else
        error("sprite '"..name.."' does not exist")
    end
end

-------------------------------------------------------------------------------
-- Shifts the color a sprite will be multiplied with when rendered.
--
-- Parameters
-- [Name]       [Type]      [Description]
-- name         String      Name of the sprite.
-- r            Number      The new red channel of the color.
-- g            Number      The new green channel of the color.
-- b            Number      The new blue channel of the color.
-- a            Number      The new alpha channel of the color.
-- ms           Number      Duration in milliseconds.
-------------------------------------------------------------------------------
function shift_sprite_color_to(name, r, g, b, a, ms)
    local sprite = World:get_object(name)
    if sprite ~= nil then
        sprite:shift_color_to(r, g, b, a, ms)
    else
        error("sprite '"..name.."' does not exist")
    end
end
