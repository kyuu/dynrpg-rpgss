-------------------------------------------------------------------------------
-- Adds a new text box.
--
-- @param name          (String) Name of the new text box (e.g. "text1").
-- @param fontname      (String) Filename of the font to use for the text.
--                               The filename is relative to the game's root
--                               directory (e.g. "Fonts/msmincho.png").
-- @param z             (Number) Render priority of the text box. A text
--                               box with a higher z-value will be rendered
--                               before anything that has a lower z-value.
-- @param x             (Number) X-position in pixels.
-- @param y             (Number) Y-position in pixels.
-- @param text          (String) Text of the text box.
-------------------------------------------------------------------------------
function add_textbox(name, fontname, z, x, y, text)
    local textbox = TextBox:new(fontname, z, x, y, text)
    SceneManager:addObject(name, textbox)
end

-------------------------------------------------------------------------------
-- Removes an existing text box.
--
-- @param name          (String) Name of the text box to remove.
-------------------------------------------------------------------------------
function remove_textbox(name)
    SceneManager:removeObject(name)
end

-------------------------------------------------------------------------------
-- Sets the text of a text box.
--
-- @param name          (String) Name of the text box.
-- @param text          (String) The new text.
-------------------------------------------------------------------------------
function set_textbox_text(name, text)
    local textbox = SceneManager:getObject(name)
    textbox:setText(text)
end

-------------------------------------------------------------------------------
-- Sets the scene of a text box.
--
-- @param name          (String) Name of the text box.
-- @param scene         (String) The new scene. One of game.Scene.
-------------------------------------------------------------------------------
function set_textbox_scene(name, scene)
    local textbox = SceneManager:getObject(name)
    textbox.scene = scene
end

-------------------------------------------------------------------------------
-- Sets the coordinate system of a text box.
--
-- @param name          (String) Name of the text box.
-- @param coordsys      (String) The coordinate system to set for the text box.
--                               Possible values are "screen" and "map".
-------------------------------------------------------------------------------
function set_textbox_coordsys(name, coordsys)
    local textbox = SceneManager:getObject(name)
    textbox.coordsys = coordsys
end

-------------------------------------------------------------------------------
-- Gets if a text box is visible.
--
-- @param name          (String) Name of the text box.
-- @param var           (Number) ID of the variable that will hold the Boolean
--                               value (1 or 0) that determines if the text box
--                               is visible.
-------------------------------------------------------------------------------
function get_textbox_visible(name, var)
    local textbox = SceneManager:getObject(name)
    game.variables[var] = textbox.visible
end

-------------------------------------------------------------------------------
-- Sets if a text box is visible.
--
-- @param name          (String) Name of the text box.
-- @param visible       (Number) Boolean (1 or 0) that determines if the text
--                               box is visible.
-------------------------------------------------------------------------------
function set_textbox_visible(name, visible)
    local textbox = SceneManager:getObject(name)
    textbox.visible = visible ~= 0
end

-------------------------------------------------------------------------------
-- Gets the z-value of a text box.
--
-- @param name          (String) Name of the text box.
-- @param var           (Number) ID of the variable that will hold the z-value.
-------------------------------------------------------------------------------
function get_textbox_z(name, var)
    local textbox = SceneManager:getObject(name)
    game.variables[var] = textbox.z
end

-------------------------------------------------------------------------------
-- Sets the z-value of a text box.
--
-- @param name          (String) Name of the text box.
-- @param z             (Number) The new z-value.
-------------------------------------------------------------------------------
function set_textbox_z(name, z)
    local textbox = SceneManager:getObject(name)
    textbox.z = z
end

-------------------------------------------------------------------------------
-- Gets the position of a text box.
--
-- @param name          (String) Name of the text box.
-- @param var_x         (Number) ID of the variable that will hold the x-position.
-- @param var_y         (Number) ID of the variable that will hold the y-position.
-------------------------------------------------------------------------------
function get_textbox_position(name, var_x, var_y)
    local textbox = SceneManager:getObject(name)
    local x, y = textbox:getPosition()
    game.variables[var_x] = x
    game.variables[var_y] = y
end

-------------------------------------------------------------------------------
-- Sets the position of a text box.
--
-- @param name          (String) Name of the text box.
-- @param x             (Number) The new x-position.
-- @param y             (Number) The new y-position.
-------------------------------------------------------------------------------
function set_textbox_position(name, x, y)
    local textbox = SceneManager:getObject(name)
    textbox:setPosition(x, y)
end

-------------------------------------------------------------------------------
-- Gets the scaling of a text box.
--
-- @param name          (String) Name of the text box.
-- @param var           (Number) ID of the variable that will hold the scaling
--                               in percent.
-------------------------------------------------------------------------------
function get_textbox_scaling(name, var)
    local textbox = SceneManager:getObject(name)
    game.variables[var] = textbox:getScaling() * 100
end

-------------------------------------------------------------------------------
-- Sets the scaling of a text box.
--
-- @param name          (String) Name of the text box.
-- @param scaling       (Number) The new scaling in percent.
-------------------------------------------------------------------------------
function set_textbox_scaling(name, scaling)
    local textbox = SceneManager:getObject(name)
    textbox:setScaling(scaling / 100)
end

-------------------------------------------------------------------------------
-- Gets the text box opacity.
--
-- @param name          (String) Name of the text box.
-- @param var           (Number) ID of the variable that will hold the opacity.
-------------------------------------------------------------------------------
function get_textbox_opacity(name, var)
    local textbox = SceneManager:getObject(name)
    game.variables[var] = textbox:getOpacity()
end

-------------------------------------------------------------------------------
-- Sets the text box opacity.
--
-- @param name          (String) Name of the text box.
-- @param opacity       (Number) The new opacity.
-------------------------------------------------------------------------------
function set_textbox_opacity(name, opacity)
    local textbox = SceneManager:getObject(name)
    textbox:setOpacity(opacity)
end

-------------------------------------------------------------------------------
-- Moves a text box.
--
-- @param name          (String) Name of the text box.
-- @param relative      (Number) If 1, (x, y) will be assumed to be offsets
--                               from the current position.
-- @param x             (Number) X-coordinate or offset.
-- @param y             (Number) Y-coordinate or offset.
-- @param ms            (Number) Duration in milliseconds.
-- @param easing        (String) Type of interpolation. See Tween.ease for all
--                               possible values.
-------------------------------------------------------------------------------
function move_textbox(name, relative, x, y, ms, easing)
    local textbox = SceneManager:getObject(name)
    textbox:move((relative == 1), x, y, ms, easing)
end

-------------------------------------------------------------------------------
-- Scales a text box.
--
-- @param name          (String) Name of the text box.
-- @param relative      (Number) If 1, (x, y) will be assumed to be offsets
--                               from the current position.
-- @param scale         (Number) The new scaling in percent.
-- @param ms            (Number) Duration in milliseconds.
-------------------------------------------------------------------------------
function scale_textbox(name, relative, scaling, ms)
    local textbox = SceneManager:getObject(name)
    textbox:scale((relative == 1), scaling / 100, ms)
end

-------------------------------------------------------------------------------
-- Fades a text box.
--
-- @param name          (String) Name of the text box.
-- @param relative      (Number) If 1, (x, y) will be assumed to be offsets
--                               from the current position.
-- @param opacity       (Number) The new opacity.
-- @param ms            (Number) Duration in milliseconds.
-------------------------------------------------------------------------------
function fade_textbox(name, relative, opacity, ms)
    local textbox = SceneManager:getObject(name)
    textbox:fade((relative == 1), opacity, ms)
end
