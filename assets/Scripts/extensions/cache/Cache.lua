require "system"

Cache = class {
    __name = "Cache",

    images = {},
    
    fonts = {},
    
    windowskins = {}
}

function Cache:__init()
    error("Cache is non-instantiable")
end

function Cache:image(filename)
    -- sanity checks
    assert(type(filename) == "string" and #filename > 0, "invalid filename")
    
    -- get cached image
    local image = self.images[filename]
    
    if image == nil then -- image not yet cached
        -- load image
        image = graphics.readImage(filename)
        
         -- make sure readImage succeeded
        if image == nil then
            error("could not load image '"..filename.."'")
        end
        
        -- cache image
        self.images[filename] = image
    end
    
    return image
end

function Cache:font(filename)
    -- sanity checks
    assert(type(filename) == "string" and #filename > 0, "invalid filename")
    
    -- get cached font
    local font = self.fonts[filename]
    
    if font == nil then -- font not yet cached
        -- load font
        font = graphics.newFont(filename)
        
         -- make sure newFont succeeded
        if font == nil then
            error("could not load font '"..filename.."'")
        end
        
        -- cache font
        self.fonts[filename] = font
    end
    
    return font
end

function Cache:windowskin(filename)
    -- sanity checks
    assert(type(filename) == "string" and #filename > 0, "invalid filename")
    
    -- get cached window skin
    local windowskin = self.windowskins[filename]
    
    if windowskin == nil then -- window skin not yet cached
        -- load window skin
        windowskin = graphics.newWindowSkin(filename)
        
         -- make sure newWindowSkin succeeded
        if windowskin == nil then
            error("could not load window skin '"..filename.."'")
        end
        
        -- cache window skin
        self.windowskins[filename] = windowskin
    end
    
    return windowskin
end
