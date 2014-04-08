#include <cassert>

#define NOT_MAIN_MODULE
#include <DynRPG/DynRPG.h>

#include "../../Context.hpp"
#include "EventWrapper.hpp"


namespace rpgss {
    namespace script {
        namespace game_module {

            //---------------------------------------------------------
            void
            EventWrapper::Push(lua_State* L, RPG::Event* event)
            {
                assert(event);
                luabridge::push(L, EventWrapper(event));
            }

            //---------------------------------------------------------
            bool
            EventWrapper::Is(lua_State* L, int index)
            {
                assert(index != 0);

                if (index < 0) { // allow negative indices
                    index = lua_gettop(L) + index + 1;
                }

                return luabridge::Stack<EventWrapper*>::is_a(L, index);
            }

            //---------------------------------------------------------
            RPG::Event*
            EventWrapper::Get(lua_State* L, int index)
            {
                assert(index != 0);
                int top = lua_gettop(L);
                if (index < 0) { // allow negative indices
                    index = top + index + 1;
                }
                if (index > top) {
                    luaL_argerror(L, index, "Event expected, got nothing");
                    return 0;
                }
                EventWrapper* wrapper = luabridge::Stack<EventWrapper*>::get(L, index);
                if (wrapper) {
                    return wrapper->This;
                } else {
                    const char* got = lua_typename(L, lua_type(L, index));
                    const char* msg = lua_pushfstring(L, "Event expected, got %s", got);
                    luaL_argerror(L, index, msg);
                    return 0;
                }
            }

            //---------------------------------------------------------
            RPG::Event*
            EventWrapper::GetOpt(lua_State* L, int index)
            {
                assert(index != 0);
                int top = lua_gettop(L);
                if (index < 0) { // allow negative indices
                    index = top + index + 1;
                }
                if (index > top) {
                    return 0;
                }
                EventWrapper* wrapper = luabridge::Stack<EventWrapper*>::get(L, index);
                if (wrapper) {
                    return wrapper->This;
                } else {
                    return 0;
                }
            }

            //---------------------------------------------------------
            EventWrapper::EventWrapper(RPG::Event* ptr)
                : CharacterWrapper(ptr)
                , This(ptr)
            {
            }

        } // namespace game_module
    } // namespace script
} // namespace rpgss
