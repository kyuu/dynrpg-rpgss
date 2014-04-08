#ifndef RPGSS_SCRIPT_GAME_MODULE_MONSTERWRAPPER_HPP_INCLUDED
#define RPGSS_SCRIPT_GAME_MODULE_MONSTERWRAPPER_HPP_INCLUDED

#include "../lua_include.hpp"
#include "BattlerWrapper.hpp"


namespace RPG {

    // forward declaration
    class Monster;

}

namespace rpgss {
    namespace script {
        namespace game_module {

            class MonsterWrapper : public BattlerWrapper {
            public:
                static void Push(lua_State* L, RPG::Monster* monster);
                static bool Is(lua_State* L, int index);
                static RPG::Monster* Get(lua_State* L, int index);
                static RPG::Monster* GetOpt(lua_State* L, int index);

                explicit MonsterWrapper(RPG::Monster* ptr);

                int get_monsterPartyId() const;

            private:
                RPG::Monster* This;
            };

        } // game_module
    } // namespace script
} // namespace rpgss


#endif // RPGSS_SCRIPT_GAME_MODULE_ACTORWRAPPER_HPP_INCLUDED
