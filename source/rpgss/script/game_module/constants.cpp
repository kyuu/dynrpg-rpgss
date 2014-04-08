#include <boost/unordered_map.hpp>
#include <boost/assign/list_of.hpp>

#define NOT_MAIN_MODULE
#include <DynRPG/DynRPG.h>

#include "constants.hpp"


namespace rpgss {
    namespace script {
        namespace game_module {

            //---------------------------------------------------------
            bool GetDirectionConstant(int direction, std::string& out_direction_str)
            {
                typedef boost::unordered_map<int, std::string> map_type;

                static map_type map = boost::assign::map_list_of
                    (RPG::DIR_UP,         "up"        )
                    (RPG::DIR_RIGHT,      "right"     )
                    (RPG::DIR_DOWN,       "down"      )
                    (RPG::DIR_LEFT,       "left"      )
                    (RPG::DIR_UP_RIGHT,   "up right"  )
                    (RPG::DIR_DOWN_RIGHT, "down right")
                    (RPG::DIR_DOWN_LEFT,  "down left" )
                    (RPG::DIR_UP_LEFT,    "up left"   );

                map_type::iterator mapped_value = map.find(direction);

                if (mapped_value == map.end()) {
                    return false;
                }

                out_direction_str = mapped_value->second;
                return true;
            }

            //---------------------------------------------------------
            bool GetDirectionConstant(const std::string& direction_str, int& out_direction)
            {
                typedef boost::unordered_map<std::string, int> map_type;

                static map_type map = boost::assign::map_list_of
                    ("up",         RPG::DIR_UP        )
                    ("right",      RPG::DIR_RIGHT     )
                    ("down",       RPG::DIR_DOWN      )
                    ("left",       RPG::DIR_LEFT      )
                    ("up right",   RPG::DIR_UP_RIGHT  )
                    ("down right", RPG::DIR_DOWN_RIGHT)
                    ("down left",  RPG::DIR_DOWN_LEFT )
                    ("up left",    RPG::DIR_UP_LEFT   );

                map_type::iterator mapped_value = map.find(direction_str);

                if (mapped_value == map.end()) {
                    return false;
                }

                out_direction = mapped_value->second;
                return true;
            }

            //---------------------------------------------------------
            bool GetFacingConstant(int facing, std::string& out_facing_str)
            {
                typedef boost::unordered_map<int, std::string> map_type;

                static map_type map = boost::assign::map_list_of
                    (RPG::FACE_UP,    "up"   )
                    (RPG::FACE_RIGHT, "right")
                    (RPG::FACE_DOWN,  "down" )
                    (RPG::FACE_LEFT,  "left" );

                map_type::iterator mapped_value = map.find(facing);

                if (mapped_value == map.end()) {
                    return false;
                }

                out_facing_str = mapped_value->second;
                return true;
            }

            //---------------------------------------------------------
            bool GetFacingConstant(const std::string& facing_str, int& out_facing)
            {
                typedef boost::unordered_map<std::string, int> map_type;

                static map_type map = boost::assign::map_list_of
                    ("up",    RPG::FACE_UP   )
                    ("right", RPG::FACE_RIGHT)
                    ("down",  RPG::FACE_DOWN )
                    ("left",  RPG::FACE_LEFT );

                map_type::iterator mapped_value = map.find(facing_str);

                if (mapped_value == map.end()) {
                    return false;
                }

                out_facing = mapped_value->second;
                return true;
            }

            //---------------------------------------------------------
            bool GetLayerConstant(int layer, std::string& out_layer_str)
            {
                typedef boost::unordered_map<int, std::string> map_type;

                static map_type map = boost::assign::map_list_of
                    (RPG::LAYER_BELOW_HERO,         "below hero"        )
                    (RPG::LAYER_SAME_LEVEL_AS_HERO, "same level as hero")
                    (RPG::LAYER_ABOVE_HERO,         "above hero"        );

                map_type::iterator mapped_value = map.find(layer);

                if (mapped_value == map.end()) {
                    return false;
                }

                out_layer_str = mapped_value->second;
                return true;
            }

            //---------------------------------------------------------
            bool GetLayerConstant(const std::string& layer_str, int& out_layer)
            {
                typedef boost::unordered_map<std::string, int> map_type;

                static map_type map = boost::assign::map_list_of
                    ("below hero",         RPG::LAYER_BELOW_HERO        )
                    ("same level as hero", RPG::LAYER_SAME_LEVEL_AS_HERO)
                    ("above hero",         RPG::LAYER_ABOVE_HERO        );

                map_type::iterator mapped_value = map.find(layer_str);

                if (mapped_value == map.end()) {
                    return false;
                }

                out_layer = mapped_value->second;
                return true;
            }

            //---------------------------------------------------------
            bool GetAnimationTypeConstant(int animationtype, std::string& out_animationtype_str)
            {
                typedef boost::unordered_map<int, std::string> map_type;

                static map_type map = boost::assign::map_list_of
                    (RPG::ANI_NORMAL,             "normal"            )
                    (RPG::ANI_STEPPING,           "stepping"          )
                    (RPG::ANI_FIXED_DIR_NORMAL,   "fixed dir normal"  )
                    (RPG::ANI_FIXED_DIR_STEPPING, "fixed dir stepping")
                    (RPG::ANI_FIXED_GRAPHIC,      "fixed graphic"     )
                    (RPG::ANI_SPIN_AROUND,        "spin around"       );

                map_type::iterator mapped_value = map.find(animationtype);

                if (mapped_value == map.end()) {
                    return false;
                }

                out_animationtype_str = mapped_value->second;
                return true;
            }

            //---------------------------------------------------------
            bool GetAnimationTypeConstant(const std::string& animationtype_str, int& out_animationtype)
            {
                typedef boost::unordered_map<std::string, int> map_type;

                static map_type map = boost::assign::map_list_of
                    ("normal",             RPG::ANI_NORMAL            )
                    ("stepping",           RPG::ANI_STEPPING          )
                    ("fixed dir normal",   RPG::ANI_FIXED_DIR_NORMAL  )
                    ("fixed dir stepping", RPG::ANI_FIXED_DIR_STEPPING)
                    ("fixed graphic",      RPG::ANI_FIXED_GRAPHIC     )
                    ("spin around",        RPG::ANI_SPIN_AROUND       );

                map_type::iterator mapped_value = map.find(animationtype_str);

                if (mapped_value == map.end()) {
                    return false;
                }

                out_animationtype = mapped_value->second;
                return true;
            }

            //---------------------------------------------------------
            bool GetControlModeConstant(int controlmode, std::string& out_controlmode_str)
            {
                typedef boost::unordered_map<int, std::string> map_type;

                static map_type map = boost::assign::map_list_of
                    (RPG::CONTROL_EVERYTHING,                   "control everything"                )
                    (RPG::CONTROL_EVERYTHING_EXCEPT_MOVEMENT,   "control everything except movement")
                    (RPG::CONTROL_NOTHING,                      "control nothing"                   );

                map_type::iterator mapped_value = map.find(controlmode);

                if (mapped_value == map.end()) {
                    return false;
                }

                out_controlmode_str = mapped_value->second;
                return true;
            }

            //---------------------------------------------------------
            bool GetControlModeConstant(const std::string& controlmode_str, int& out_controlmode)
            {
                typedef boost::unordered_map<std::string, int> map_type;

                static map_type map = boost::assign::map_list_of
                    ("control everything",                  RPG::CONTROL_EVERYTHING                )
                    ("control everything except movement",  RPG::CONTROL_EVERYTHING_EXCEPT_MOVEMENT)
                    ("control nothing",                     RPG::CONTROL_NOTHING                   );

                map_type::iterator mapped_value = map.find(controlmode_str);

                if (mapped_value == map.end()) {
                    return false;
                }

                out_controlmode = mapped_value->second;
                return true;
            }

            //---------------------------------------------------------
            bool GetVehicleConstant(int vehicle, std::string& out_vehicle_str)
            {
                typedef boost::unordered_map<int, std::string> map_type;

                static map_type map = boost::assign::map_list_of
                    (RPG::HV_NONE,      "none"   )
                    (RPG::HV_SKIFF,     "skiff"  )
                    (RPG::HV_SHIP,      "ship"   )
                    (RPG::HV_AIRSHIP,   "airship");

                map_type::iterator mapped_value = map.find(vehicle);

                if (mapped_value == map.end()) {
                    return false;
                }

                out_vehicle_str = mapped_value->second;
                return true;
            }

            //---------------------------------------------------------
            bool GetVehicleConstant(const std::string& vehicle_str, int& out_vehicle)
            {
                typedef boost::unordered_map<std::string, int> map_type;

                static map_type map = boost::assign::map_list_of
                    ("none",    RPG::HV_NONE   )
                    ("skiff",   RPG::HV_SKIFF  )
                    ("ship",    RPG::HV_SHIP   )
                    ("airship", RPG::HV_AIRSHIP);

                map_type::iterator mapped_value = map.find(vehicle_str);

                if (mapped_value == map.end()) {
                    return false;
                }

                out_vehicle = mapped_value->second;
                return true;
            }

            //---------------------------------------------------------
            bool GetRpgKeyConstant(int rpg_key, std::string& out_rpg_key_str)
            {
                typedef boost::unordered_map<int, std::string> map_type;

                static map_type map = boost::assign::map_list_of
                    (RPG::KEY_DOWN,     "down"    )
                    (RPG::KEY_LEFT,     "left"    )
                    (RPG::KEY_RIGHT,    "right"   )
                    (RPG::KEY_UP,       "up"      )
                    (RPG::KEY_DECISION, "decision")
                    (RPG::KEY_CANCEL,   "cancel"  )
                    (RPG::KEY_0,        "0"       )
                    (RPG::KEY_1,        "1"       )
                    (RPG::KEY_2,        "2"       )
                    (RPG::KEY_3,        "3"       )
                    (RPG::KEY_4,        "4"       )
                    (RPG::KEY_5,        "5"       )
                    (RPG::KEY_6,        "6"       )
                    (RPG::KEY_7,        "7"       )
                    (RPG::KEY_8,        "8"       )
                    (RPG::KEY_9,        "9"       )
                    (RPG::KEY_ADD,      "add"     )
                    (RPG::KEY_SUBTRACT, "subtract")
                    (RPG::KEY_MULTIPLY, "multiply")
                    (RPG::KEY_DIVIDE,   "divide"  )
                    (RPG::KEY_DECIMAL,  "decimal" );

                map_type::iterator mapped_value = map.find(rpg_key);

                if (mapped_value == map.end()) {
                    return false;
                }

                out_rpg_key_str = mapped_value->second;
                return true;
            }

            //---------------------------------------------------------
            bool GetRpgKeyConstant(const std::string& rpg_key_str, int& out_rpg_key)
            {
                typedef boost::unordered_map<std::string, int> map_type;

                static map_type map = boost::assign::map_list_of
                    ("down",     RPG::KEY_DOWN    )
                    ("left",     RPG::KEY_LEFT    )
                    ("right",    RPG::KEY_RIGHT   )
                    ("up",       RPG::KEY_UP      )
                    ("decision", RPG::KEY_DECISION)
                    ("cancel",   RPG::KEY_CANCEL  )
                    ("0",        RPG::KEY_0       )
                    ("1",        RPG::KEY_1       )
                    ("2",        RPG::KEY_2       )
                    ("3",        RPG::KEY_3       )
                    ("4",        RPG::KEY_4       )
                    ("5",        RPG::KEY_5       )
                    ("6",        RPG::KEY_6       )
                    ("7",        RPG::KEY_7       )
                    ("8",        RPG::KEY_8       )
                    ("9",        RPG::KEY_9       )
                    ("add",      RPG::KEY_ADD     )
                    ("subtract", RPG::KEY_SUBTRACT)
                    ("multiply", RPG::KEY_MULTIPLY)
                    ("divide",   RPG::KEY_DIVIDE  )
                    ("decimal",  RPG::KEY_DECIMAL );

                map_type::iterator mapped_value = map.find(rpg_key_str);

                if (mapped_value == map.end()) {
                    return false;
                }

                out_rpg_key = mapped_value->second;
                return true;
            }

            //---------------------------------------------------------
            bool GetSceneConstant(int scene, std::string& out_scene_str)
            {
                typedef boost::unordered_map<int, std::string> map_type;

                static map_type map = boost::assign::map_list_of
                    (RPG::SCENE_MAP,       "map"      )
                    (RPG::SCENE_MENU,      "menu"     )
                    (RPG::SCENE_BATTLE,    "battle"   )
                    (RPG::SCENE_SHOP,      "shop"     )
                    (RPG::SCENE_NAME,      "name"     )
                    (RPG::SCENE_FILE,      "file"     )
                    (RPG::SCENE_TITLE,     "title"    )
                    (RPG::SCENE_GAME_OVER, "game over")
                    (RPG::SCENE_DEBUG,     "debug"    );

                map_type::iterator mapped_value = map.find(scene);

                if (mapped_value == map.end()) {
                    return false;
                }

                out_scene_str = mapped_value->second;
                return true;
            }

            //---------------------------------------------------------
            bool GetSceneConstant(const std::string& scene_str, int& out_scene)
            {
                typedef boost::unordered_map<std::string, int> map_type;

                static map_type map = boost::assign::map_list_of
                    ("map",       RPG::SCENE_MAP      )
                    ("menu",      RPG::SCENE_MENU     )
                    ("battle",    RPG::SCENE_BATTLE   )
                    ("shop",      RPG::SCENE_SHOP     )
                    ("name",      RPG::SCENE_NAME     )
                    ("file",      RPG::SCENE_FILE     )
                    ("title",     RPG::SCENE_TITLE    )
                    ("game over", RPG::SCENE_GAME_OVER)
                    ("debug",     RPG::SCENE_DEBUG    );

                map_type::iterator mapped_value = map.find(scene_str);

                if (mapped_value == map.end()) {
                    return false;
                }

                out_scene = mapped_value->second;
                return true;
            }

        } // namespace game_module
    } // namespace script
} // namespace rpgss
