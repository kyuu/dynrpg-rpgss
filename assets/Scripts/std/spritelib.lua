require "std.world"
require "std.sprite"


function spritelib_add(name, filename, blend_mode, visible, z, x, y, scale, angle)
    local sprite = Sprite.new(filename, blend_mode, visible, z, x, y, scale / 100, angle)
    world:addObject(name, sprite)
end

function spritelib_remove(name)
    world:removeObject(name)
end

function spritelib_set_texture(name, filename)
    local sprite = world:getObject(name)
    if sprite ~= nil then
        sprite:set_texture(filename)
    else
        error("Sprite '"..name.."' does not exist.")
    end
end

function spritelib_set_blend_mode(name, blend_mode)
    local sprite = world:getObject(name)
    if sprite ~= nil then
        sprite:set_blend_mode(blend_mode)
    else
        error("Sprite '"..name.."' does not exist.")
    end
end

function spritelib_set_coordinate_system(name, coord_sys)
    local sprite = world:getObject(name)
    if sprite ~= nil then
        sprite:set_coordinate_system(coord_sys)
    else
        error("Sprite '"..name.."' does not exist.")
    end
end

function spritelib_get_visible(name, var)
    local sprite = world:getObject(name)
    if sprite ~= nil then
        game.variables[var] = sprite:get_visible()
    else
        error("Sprite '"..name.."' does not exist.")
    end
end

function spritelib_set_visible(name, visible)
    local sprite = world:getObject(name)
    if sprite ~= nil then
        sprite:set_visible(visible ~= 0)
    else
        error("Sprite '"..name.."' does not exist.")
    end
end

function spritelib_get_z(name, var)
    local sprite = world:getObject(name)
    if sprite ~= nil then
        game.variables[var] = sprite:get_z()
    else
        error("Sprite '"..name.."' does not exist.")
    end
end

function spritelib_set_z(name, z)
    local sprite = world:getObject(name)
    if sprite ~= nil then
        sprite:set_z(z)
    else
        error("Sprite '"..name.."' does not exist.")
    end
end

function spritelib_get_position(name, var_x, var_y)
    local sprite = world:getObject(name)
    if sprite ~= nil then
        local x, y = sprite:get_position()
        game.variables[var_x] = x
        game.variables[var_y] = y
    else
        error("Sprite '"..name.."' does not exist.")
    end
end

function spritelib_set_position(name, x, y)
    local sprite = world:getObject(name)
    if sprite ~= nil then
        sprite:set_position(x, y)
    else
        error("Sprite '"..name.."' does not exist.")
    end
end

function spritelib_get_scale(name, var)
    local sprite = world:getObject(name)
    if sprite ~= nil then
        game.variables[var] = sprite:get_scale() * 100
    else
        error("Sprite '"..name.."' does not exist.")
    end
end

function spritelib_set_scale(name, scale)
    local sprite = world:getObject(name)
    if sprite ~= nil then
        sprite:set_scale(scale / 100)
    else
        error("Sprite '"..name.."' does not exist.")
    end
end

function spritelib_get_angle(name, var)
    local sprite = world:getObject(name)
    if sprite ~= nil then
        game.variables[var] = sprite:get_angle()
    else
        error("Sprite '"..name.."' does not exist.")
    end
end

function spritelib_set_angle(name, angle)
    local sprite = world:getObject(name)
    if sprite ~= nil then
        sprite:set_angle(angle)
    else
        error("Sprite '"..name.."' does not exist.")
    end
end

function spritelib_move_by(name, ox, oy, ms, easing)
    local sprite = world:getObject(name)
    if sprite ~= nil then
        sprite:move_by(ox, oy, ms, easing)
    else
        error("Sprite '"..name.."' does not exist.")
    end
end

function spritelib_move_to(name, x, y, ms, easing)
    local sprite = world:getObject(name)
    if sprite ~= nil then
        sprite:move_to(x, y, ms, easing)
    else
        error("Sprite '"..name.."' does not exist.")
    end
end

function spritelib_scale(name, scale, ms)
    local sprite = world:getObject(name)
    if sprite ~= nil then
        sprite:scale(scale / 100, ms)
    else
        error("Sprite '"..name.."' does not exist.")
    end
end

function spritelib_rotate_by(name, direction, angle, ms)
    local sprite = world:getObject(name)
    if sprite ~= nil then
        sprite:rotate_by(direction, angle, ms)
    else
        error("Sprite '"..name.."' does not exist.")
    end
end

function spritelib_rotate_to(name, direction, angle, ms)
    local sprite = world:getObject(name)
    if sprite ~= nil then
        sprite:rotate_to(direction, angle, ms)
    else
        error("Sprite '"..name.."' does not exist.")
    end
end

function spritelib_rotate_forever(name, direction, ms)
    local sprite = world:getObject(name)
    if sprite ~= nil then
        sprite:rotate_forever(direction, ms)
    else
        error("Sprite '"..name.."' does not exist.")
    end
end

function spritelib_stop_rotation(name)
    local sprite = world:getObject(name)
    if sprite ~= nil then
        sprite:stop_rotation()
    else
        error("Sprite '"..name.."' does not exist.")
    end
end
