#ifndef RPGSS_SCRIPT_GRAPHICS_MODULE_GRAPHICS_MODULE_HPP_INCLUDED
#define RPGSS_SCRIPT_GRAPHICS_MODULE_GRAPHICS_MODULE_HPP_INCLUDED

#include "../../graphics/graphics.hpp"
#include "../lua_include.hpp"
#include "ImageWrapper.hpp"
#include "FontWrapper.hpp"
#include "WindowSkinWrapper.hpp"
#include "constants.hpp"


namespace rpgss {
    namespace script {
        namespace graphics_module {

            bool RegisterGraphicsModule(lua_State* L);

        } // graphics_module
    } // namespace script
} // namespace rpgss


#endif // RPGSS_SCRIPT_GRAPHICS_MODULE_GRAPHICS_MODULE_HPP_INCLUDED
