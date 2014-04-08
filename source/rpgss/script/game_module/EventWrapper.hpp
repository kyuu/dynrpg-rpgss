#ifndef RPGSS_SCRIPT_GAME_MODULE_EVENTWRAPPER_HPP_INCLUDED
#define RPGSS_SCRIPT_GAME_MODULE_EVENTWRAPPER_HPP_INCLUDED

#include "../lua_include.hpp"
#include "CharacterWrapper.hpp"


namespace RPG {

    // forward declaration
    class Event;

}

namespace rpgss {
    namespace script {
        namespace game_module {

            class EventWrapper : public CharacterWrapper {
            public:
                static void Push(lua_State* L, RPG::Event* event);
                static bool Is(lua_State* L, int index);
                static RPG::Event* Get(lua_State* L, int index);
                static RPG::Event* GetOpt(lua_State* L, int index);

                explicit EventWrapper(RPG::Event* ptr);

            private:
                RPG::Event* This;
            };

        } // game_module
    } // namespace script
} // namespace rpgss


#endif // RPGSS_SCRIPT_GAME_MODULE_EVENTWRAPPER_HPP_INCLUDED
