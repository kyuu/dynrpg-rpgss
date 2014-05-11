/*
    The MIT License (MIT)

    Copyright (c) 2014 Anatoli Steinmark

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
    THE SOFTWARE.
*/

#ifndef RPGSS_SCRIPT_SCRIPT_HPP_INCLUDED
#define RPGSS_SCRIPT_SCRIPT_HPP_INCLUDED

#include "../common/RefCountedObjectPtr.hpp"
#include "lua_include.hpp"
#include "audio_module/audio_module.hpp"
#include "core_module/core_module.hpp"
#include "game_module/game_module.hpp"
#include "graphics_module/graphics_module.hpp"
#include "io_module/io_module.hpp"
#include "keyboard_module/keyboard_module.hpp"
#include "mouse_module/mouse_module.hpp"
#include "timer_module/timer_module.hpp"
#include "util_module/util_module.hpp"


#define RPGSS_STACK_TRACE_MAGIC "<7wh39hsfguihw489hw38>"


// register rpgss::RefCountedObjectPtr as container in luabridge
namespace luabridge {

    // forward declaration
    template <class T>
    struct ContainerTraits;

    template <class T>
    struct ContainerTraits <rpgss::RefCountedObjectPtr <T> >
    {
        typedef T Type;

        static T* get (rpgss::RefCountedObjectPtr <T> const& c)
        {
            return c.get ();
        }
    };

} // namespace luabridge

namespace rpgss {
    namespace script {

        bool RegisterModules(lua_State* L);

        bool DoFile(lua_State* L, const std::string& filename);
        bool Call(lua_State* L, int nargs, int nresults);

        int luax_panic_handler(lua_State* L);
        int luax_message_handler(lua_State* L);
        int luax_pcall(lua_State* L, int nargs, int nresults);
        int luax_print(lua_State* L);
        int luax_loadfile(lua_State* L);
        int luax_dofile(lua_State* L);

    } // namespace script
} // namespace rpgss


#endif // RPGSS_SCRIPT_SCRIPT_HPP_INCLUDED
