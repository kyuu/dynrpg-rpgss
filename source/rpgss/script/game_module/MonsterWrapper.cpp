#include <cassert>

#define NOT_MAIN_MODULE
#include <DynRPG/DynRPG.h>

#include "../../Context.hpp"
#include "MonsterWrapper.hpp"


namespace rpgss {
    namespace script {
        namespace game_module {

            //---------------------------------------------------------
            void
            MonsterWrapper::Push(lua_State* L, RPG::Monster* monster)
            {
                assert(monster);
                luabridge::push(L, MonsterWrapper(monster));
            }

            //---------------------------------------------------------
            bool
            MonsterWrapper::Is(lua_State* L, int index)
            {
                assert(index != 0);

                if (index < 0) { // allow negative indices
                    index = lua_gettop(L) + index + 1;
                }

                return luabridge::Stack<MonsterWrapper*>::is_a(L, index);
            }

            //---------------------------------------------------------
            RPG::Monster*
            MonsterWrapper::Get(lua_State* L, int index)
            {
                assert(index != 0);
                int top = lua_gettop(L);
                if (index < 0) { // allow negative indices
                    index = top + index + 1;
                }
                if (index > top) {
                    luaL_argerror(L, index, "Monster expected, got nothing");
                    return 0;
                }
                MonsterWrapper* wrapper = luabridge::Stack<MonsterWrapper*>::get(L, index);
                if (wrapper) {
                    return wrapper->This;
                } else {
                    const char* got = lua_typename(L, lua_type(L, index));
                    const char* msg = lua_pushfstring(L, "Monster expected, got %s", got);
                    luaL_argerror(L, index, msg);
                    return 0;
                }
            }

            //---------------------------------------------------------
            RPG::Monster*
            MonsterWrapper::GetOpt(lua_State* L, int index)
            {
                assert(index != 0);
                int top = lua_gettop(L);
                if (index < 0) { // allow negative indices
                    index = top + index + 1;
                }
                if (index > top) {
                    return 0;
                }
                MonsterWrapper* wrapper = luabridge::Stack<MonsterWrapper*>::get(L, index);
                if (wrapper) {
                    return wrapper->This;
                } else {
                    return 0;
                }
            }

            //---------------------------------------------------------
            MonsterWrapper::MonsterWrapper(RPG::Monster* ptr)
                : BattlerWrapper(ptr)
                , This(ptr)
            {
            }

            //---------------------------------------------------------
            int
            MonsterWrapper::get_monsterPartyId() const
            {
                return This->id + 1;
            }

        } // namespace game_module
    } // namespace script
} // namespace rpgss
