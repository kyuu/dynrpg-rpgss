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

#ifndef RPGSS_SCRIPT_GAME_MODULE_BATTLERWRAPPER_HPP_INCLUDED
#define RPGSS_SCRIPT_GAME_MODULE_BATTLERWRAPPER_HPP_INCLUDED

#include "../lua_include.hpp"


namespace RPG {

    // forward declaration
    class Battler;

}

namespace rpgss {
    namespace script {
        namespace game_module {

            class BattlerWrapper {
            public:
                static void Push(lua_State* L, RPG::Battler* battler);
                static bool Is(lua_State* L, int index);
                static RPG::Battler* Get(lua_State* L, int index);
                static RPG::Battler* GetOpt(lua_State* L, int index);

                explicit BattlerWrapper(RPG::Battler* ptr);

                int get_databaseId() const;

                std::string get_name() const;

                bool get_isMonster() const;

                bool get_notHidden() const;

                bool get_mirrored() const;

                int get_maxHp() const;

                int get_maxMp() const;

                int  get_hp() const;
                void set_hp(int hp);

                int  get_mp() const;
                void set_mp(int mp);

                int get_attack() const;

                int get_defense() const;

                int get_intelligence() const;

                int get_agility() const;

                int get_atbValue() const;

                int get_turnsTaken() const;

                int get_screenX() const;

                int get_screenY() const;

                int getScreenPosition(lua_State* L);

            private:
                RPG::Battler* This;
            };

        } // game_module
    } // namespace script
} // namespace rpgss


#endif // RPGSS_SCRIPT_GAME_MODULE_BATTLERWRAPPER_HPP_INCLUDED
