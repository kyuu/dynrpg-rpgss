-------------------------------------------------------------------------------
-- Changes the lightmap opacity.
--
-- @param opacity       (Number) Target opacity value.
-- @param ms            (Number) Optional duration in milliseconds. If > 0,
--                               the opacity will be interpolated over time.
-------------------------------------------------------------------------------
function change_lightmap_opacity(opacity, ms)
    LightmapManager:changeOpacity(opacity, ms)
end
