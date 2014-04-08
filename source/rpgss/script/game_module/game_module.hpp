#ifndef RPGSS_SCRIPT_GAME_MODULE_GAME_MODULE_HPP_INCLUDED
#define RPGSS_SCRIPT_GAME_MODULE_GAME_MODULE_HPP_INCLUDED

#include "../lua_include.hpp"
#include "CharacterWrapper.hpp"
#include "EventWrapper.hpp"
#include "HeroWrapper.hpp"
#include "BattlerWrapper.hpp"
#include "MonsterWrapper.hpp"
#include "ActorWrapper.hpp"
#include "constants.hpp"


namespace rpgss {
    namespace script {
        namespace game_module {

            bool RegisterGameModule(lua_State* L);

        } // namespace game_module
    } // namespace script
} // namespace rpgss


#endif // RPGSS_SCRIPT_GAME_MODULE_GAME_MODULE_HPP_INCLUDED
