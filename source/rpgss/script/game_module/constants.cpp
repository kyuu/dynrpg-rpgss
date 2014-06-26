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
            bool GetMoveTypeConstant(int movetype, std::string& out_movetype_str)
            {
                typedef boost::unordered_map<int, std::string> map_type;

                static map_type map = boost::assign::map_list_of
                    (RPG::MT_MOVE_UP,                   "move up"               )
                    (RPG::MT_MOVE_RIGHT,                "move right"            )
                    (RPG::MT_MOVE_DOWN,                 "move down"             )
                    (RPG::MT_MOVE_LEFT,                 "move left"             )
                    (RPG::MT_MOVE_UP_RIGHT,             "move up-right"         )
                    (RPG::MT_MOVE_UP_LEFT,              "move up-left"          )
                    (RPG::MT_MOVE_DOWN_RIGHT,           "move down-right"       )
                    (RPG::MT_MOVE_DOWN_LEFT,            "move down-left"        )
                    (RPG::MT_MOVE_RANDOMLY,             "move randomly"         )
                    (RPG::MT_MOVE_TOWARD_HERO,          "move toward hero"      )
                    (RPG::MT_MOVE_AWAY_FROM_HERO,       "move away from hero"   )
                    (RPG::MT_MOVE_FORWARD,              "move forward"          )
                    (RPG::MT_FACE_UP,                   "face up"               )
                    (RPG::MT_FACE_RIGHT,                "face right"            )
                    (RPG::MT_FACE_DOWN,                 "face down"             )
                    (RPG::MT_FACE_LEFT,                 "face left"             )
                    (RPG::MT_FACE_RANDOMLY,             "face randomly"         )
                    (RPG::MT_FACE_TOWARD_HERO,          "face toward hero"      )
                    (RPG::MT_FACE_AWAY_FROM_HERO,       "face away from hero"   )
                    (RPG::MT_TURN_RIGHT,                "turn right"            )
                    (RPG::MT_TURN_LEFT,                 "turn left"             )
                    (RPG::MT_TURN_AROUND,               "turn around"           )
                    (RPG::MT_TURN_RANDOMLY,             "turn randomly"         )
                    (RPG::MT_WAIT,                      "wait"                  )
                    (RPG::MT_BEGIN_JUMP,                "begin jump"            )
                    (RPG::MT_END_JUMP,                  "end jump"              )
                    (RPG::MT_LOCK_FACING,               "lock facing"           )
                    (RPG::MT_UNLOCK_FACING,             "unlock facing"         )
                    (RPG::MT_INCREASE_SPEED,            "increase speed"        )
                    (RPG::MT_DECREASE_SPEED,            "decrease speed"        )
                    (RPG::MT_INCREASE_FREQUENCY,        "increase frequency"    )
                    (RPG::MT_DECREASE_FREQUENCY,        "decrease frequency"    )
                    (RPG::MT_INCREASE_TRANSPARENCY,     "increase transparency" )
                    (RPG::MT_DECREASE_TRANSPARENCY,     "decrease transparency" )
                    (RPG::MT_PHASING_MODE_ON,           "phasing mode on"       )
                    (RPG::MT_PHASING_MODE_OFF,          "phasing mode off"      )
                    (RPG::MT_STOP_ANIMATION,            "stop animation"        )
                    (RPG::MT_RESUME_ANIMATION,          "resume animation"      )
                    (RPG::MT_SWITCH_ON,                 "switch on"             )
                    (RPG::MT_SWITCH_OFF,                "switch off"            )
                    (RPG::MT_CHANGE_GRAPHIC,            "change graphic"        )
                    (RPG::MT_PLAY_SE,                   "play se"               );

                map_type::iterator mapped_value = map.find(movetype);

                if (mapped_value == map.end()) {
                    return false;
                }

                out_movetype_str = mapped_value->second;
                return true;
            }

            //---------------------------------------------------------
            bool GetMoveTypeConstant(const std::string& movetype_str, int& out_movetype)
            {
                typedef boost::unordered_map<std::string, int> map_type;

                static map_type map = boost::assign::map_list_of
                    ("move up",              RPG::MT_MOVE_UP                )
                    ("move right",           RPG::MT_MOVE_RIGHT             )
                    ("move down",            RPG::MT_MOVE_DOWN              )
                    ("move left",            RPG::MT_MOVE_LEFT              )
                    ("move up-right",        RPG::MT_MOVE_UP_RIGHT          )
                    ("move up-left",         RPG::MT_MOVE_UP_LEFT           )
                    ("move down-right",      RPG::MT_MOVE_DOWN_RIGHT        )
                    ("move down-left",       RPG::MT_MOVE_DOWN_LEFT         )
                    ("move randomly",        RPG::MT_MOVE_RANDOMLY          )
                    ("move toward hero",     RPG::MT_MOVE_TOWARD_HERO       )
                    ("move away from hero",  RPG::MT_MOVE_AWAY_FROM_HERO    )
                    ("move forward",         RPG::MT_MOVE_FORWARD           )
                    ("face up",              RPG::MT_FACE_UP                )
                    ("face right",           RPG::MT_FACE_RIGHT             )
                    ("face down",            RPG::MT_FACE_DOWN              )
                    ("face left",            RPG::MT_FACE_LEFT              )
                    ("face randomly",        RPG::MT_FACE_RANDOMLY          )
                    ("face toward hero",     RPG::MT_FACE_TOWARD_HERO       )
                    ("face away from hero",  RPG::MT_FACE_AWAY_FROM_HERO    )
                    ("turn right",           RPG::MT_TURN_RIGHT             )
                    ("turn left",            RPG::MT_TURN_LEFT              )
                    ("turn around",          RPG::MT_TURN_AROUND            )
                    ("turn randomly",        RPG::MT_TURN_RANDOMLY          )
                    ("wait",                 RPG::MT_WAIT                   )
                    ("begin jump",           RPG::MT_BEGIN_JUMP             )
                    ("end jump",             RPG::MT_END_JUMP               )
                    ("lock facing",          RPG::MT_LOCK_FACING            )
                    ("unlock facing",        RPG::MT_UNLOCK_FACING          )
                    ("increase speed",       RPG::MT_INCREASE_SPEED         )
                    ("decrease speed",       RPG::MT_DECREASE_SPEED         )
                    ("increase frequency",   RPG::MT_INCREASE_FREQUENCY     )
                    ("decrease frequency",   RPG::MT_DECREASE_FREQUENCY     )
                    ("increase transparency",RPG::MT_INCREASE_TRANSPARENCY  )
                    ("decrease transparency",RPG::MT_DECREASE_TRANSPARENCY  )
                    ("phasing mode on",      RPG::MT_PHASING_MODE_ON        )
                    ("phasing mode off",     RPG::MT_PHASING_MODE_OFF       )
                    ("stop animation",       RPG::MT_STOP_ANIMATION         )
                    ("resume animation",     RPG::MT_RESUME_ANIMATION       )
                    ("switch on",            RPG::MT_SWITCH_ON              )
                    ("switch off",           RPG::MT_SWITCH_OFF             )
                    ("change graphic",       RPG::MT_CHANGE_GRAPHIC         )
                    ("play se",              RPG::MT_PLAY_SE                );

                map_type::iterator mapped_value = map.find(movetype_str);

                if (mapped_value == map.end()) {
                    return false;
                }

                out_movetype = mapped_value->second;
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

            //---------------------------------------------------------
            bool GetMenuSceneConstant(int menuscene, std::string& out_menuscene_str)
            {
                typedef boost::unordered_map<int, std::string> map_type;

                static map_type map = boost::assign::map_list_of
                    (0, "main"        )
                    (1, "item"        )
                    (2, "use item"    )
                    (3, "skill"       )
                    (4, "use skill"   )
                    (5, "teleport"    )
                    (6, "equip"       )
                    (7, "quit"        )
                    (8, "status"      )
                    (9, "order"       );

                map_type::iterator mapped_value = map.find(menuscene);

                if (mapped_value == map.end()) {
                    return false;
                }

                out_menuscene_str = mapped_value->second;
                return true;
            }

            //---------------------------------------------------------
            bool GetMenuSceneConstant(const std::string& menuscene_str, int& out_menuscene)
            {
                typedef boost::unordered_map<std::string, int> map_type;

                static map_type map = boost::assign::map_list_of
                    ("main",        0)
                    ("item",        1)
                    ("use item",    2)
                    ("skill",       3)
                    ("use skill",   4)
                    ("teleport",    5)
                    ("equip",       6)
                    ("quit",        7)
                    ("status",      8)
                    ("order",       9);

                map_type::iterator mapped_value = map.find(menuscene_str);

                if (mapped_value == map.end()) {
                    return false;
                }

                out_menuscene = mapped_value->second;
                return true;
            }

            //---------------------------------------------------------
            bool GetBattleLayoutConstant(int battlelayout, std::string& out_battlelayout_str)
            {
                typedef boost::unordered_map<int, std::string> map_type;

                static map_type map = boost::assign::map_list_of
                    (RPG::BL_TRADITIONAL, "traditional")
                    (RPG::BL_ALTERNATIVE, "alternative")
                    (RPG::BL_GAUGE,       "gauge"      );

                map_type::iterator mapped_value = map.find(battlelayout);

                if (mapped_value == map.end()) {
                    return false;
                }

                out_battlelayout_str = mapped_value->second;
                return true;
            }

            //---------------------------------------------------------
            bool GetBattleLayoutConstant(const std::string& battlelayout_str, int& out_battlelayout)
            {
                typedef boost::unordered_map<std::string, int> map_type;

                static map_type map = boost::assign::map_list_of
                    ("traditional", RPG::BL_TRADITIONAL)
                    ("alternative", RPG::BL_ALTERNATIVE)
                    ("gauge",       RPG::BL_GAUGE      );

                map_type::iterator mapped_value = map.find(battlelayout_str);

                if (mapped_value == map.end()) {
                    return false;
                }

                out_battlelayout = mapped_value->second;
                return true;
            }

            //---------------------------------------------------------
            bool GetAtbModeConstant(int atbmode, std::string& out_atbmode_str)
            {
                typedef boost::unordered_map<int, std::string> map_type;

                static map_type map = boost::assign::map_list_of
                    (RPG::ATBM_ACTIVE, "active")
                    (RPG::ATBM_WAIT,   "wait"  );

                map_type::iterator mapped_value = map.find(atbmode);

                if (mapped_value == map.end()) {
                    return false;
                }

                out_atbmode_str = mapped_value->second;
                return true;
            }

            //---------------------------------------------------------
            bool GetAtbModeConstant(const std::string& atbmode_str, int& out_atbmode)
            {
                typedef boost::unordered_map<std::string, int> map_type;

                static map_type map = boost::assign::map_list_of
                    ("active", RPG::ATBM_ACTIVE)
                    ("wait",   RPG::ATBM_WAIT  );

                map_type::iterator mapped_value = map.find(atbmode_str);

                if (mapped_value == map.end()) {
                    return false;
                }

                out_atbmode = mapped_value->second;
                return true;
            }

        } // namespace game_module
    } // namespace script
} // namespace rpgss
