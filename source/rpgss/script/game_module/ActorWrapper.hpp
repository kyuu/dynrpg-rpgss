#ifndef RPGSS_SCRIPT_GAME_MODULE_ACTORWRAPPER_HPP_INCLUDED
#define RPGSS_SCRIPT_GAME_MODULE_ACTORWRAPPER_HPP_INCLUDED

#include "../lua_include.hpp"
#include "BattlerWrapper.hpp"


namespace RPG {

    // forward declaration
    class Actor;

}

namespace rpgss {
    namespace script {
        namespace game_module {

            class ActorWrapper : public BattlerWrapper {
            public:
                static void Push(lua_State* L, RPG::Actor* actor);
                static bool Is(lua_State* L, int index);
                static RPG::Actor* Get(lua_State* L, int index);
                static RPG::Actor* GetOpt(lua_State* L, int index);

                explicit ActorWrapper(RPG::Actor* ptr);

                std::string get_degree() const;

                std::string get_charsetFilename() const;

                int get_charsetId() const;

                std::string get_facesetFilename() const;

                int get_facesetId() const;

                int  get_level() const;
                void set_level(int level);

                int  get_exp() const;
                void set_exp(int exp);

                int  get_weaponId() const;
                void set_weaponId(int weaponId);

                int  get_shieldId() const;
                void set_shieldId(int shieldId);

                int  get_armorId() const;
                void set_armorId(int armorId);

                int  get_helmetId() const;
                void set_helmetId(int helmetId);

                int  get_accessoryId() const;
                void set_accessoryId(int accessoryId);

                bool get_twoWeapons() const;
                void set_twoWeapons(bool twoWeapons);

                bool get_lockEquipment() const;
                void set_lockEquipment(bool lockEquipment);

                bool get_autoBattle() const;
                void set_autoBattle(bool autoBattle);

                bool get_mightyGuard() const;
                void set_mightyGuard(bool mightyGuard);

            private:
                RPG::Actor* This;
            };

        } // game_module
    } // namespace script
} // namespace rpgss


#endif // RPGSS_SCRIPT_GAME_MODULE_ACTORWRAPPER_HPP_INCLUDED
