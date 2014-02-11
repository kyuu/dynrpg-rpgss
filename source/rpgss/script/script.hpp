#ifndef RPGSS_SCRIPT_SCRIPT_HPP_INCLUDED
#define RPGSS_SCRIPT_SCRIPT_HPP_INCLUDED

#include "lua.hpp"
#include "rpg.hpp"

#define RPGSS_STACK_TRACE_MAGIC "<7wh39hsfguihw489hw38>"


namespace rpgss {
    namespace script {

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
