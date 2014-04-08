#ifndef RPGSS_SCRIPT_GAME_MODULE_HEROWRAPPER_HPP_INCLUDED
#define RPGSS_SCRIPT_GAME_MODULE_HEROWRAPPER_HPP_INCLUDED

#include "../lua_include.hpp"
#include "CharacterWrapper.hpp"


namespace RPG {

    // forward declaration
    class Hero;

}

namespace rpgss {
    namespace script {
        namespace game_module {

            class HeroWrapper : public CharacterWrapper {
            public:
                static void Push(lua_State* L, RPG::Hero* hero);
                static bool Is(lua_State* L, int index);
                static RPG::Hero* Get(lua_State* L, int index);
                static RPG::Hero* GetOpt(lua_State* L, int index);

                explicit HeroWrapper(RPG::Hero* ptr);

                std::string get_controlMode() const;
                void        set_controlMode(const std::string& controlmode_str);

                std::string get_vehicle() const;

            private:
                RPG::Hero* This;
            };

        } // game_module
    } // namespace script
} // namespace rpgss


#endif // RPGSS_SCRIPT_GAME_MODULE_HEROWRAPPER_HPP_INCLUDED
