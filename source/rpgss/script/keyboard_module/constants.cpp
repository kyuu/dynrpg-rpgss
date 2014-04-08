#include <boost/unordered_map.hpp>
#include <boost/assign/list_of.hpp>

#include "../../input/input.hpp"
#include "constants.hpp"


namespace rpgss {
    namespace script {
        namespace keyboard_module {

            //---------------------------------------------------------
            bool GetKeyConstant(int key, std::string& out_key_str)
            {
                typedef boost::unordered_map<int, std::string> map_type;

                static map_type map = boost::assign::map_list_of
                    (input::KEY_BACKSPACE,      "backspace"     )
                    (input::KEY_TAB,            "tab"           )
                    (input::KEY_CLEAR,          "clear"         )
                    (input::KEY_ENTER,          "enter"         )
                    (input::KEY_SHIFT,          "shift"         )
                    (input::KEY_CONTROL,        "ctrl"          )
                    (input::KEY_ALT,            "alt"           )
                    (input::KEY_CAPSLOCK,       "caps lock"     )
                    (input::KEY_ESCAPE,         "escape"        )
                    (input::KEY_SPACE,          "space"         )
                    (input::KEY_PAGEUP,         "page up"       )
                    (input::KEY_PAGEDOWN,       "page down"     )
                    (input::KEY_END,            "end"           )
                    (input::KEY_HOME,           "home"          )
                    (input::KEY_LEFT,           "left"          )
                    (input::KEY_UP,             "up"            )
                    (input::KEY_RIGHT,          "right"         )
                    (input::KEY_DOWN,           "down"          )
                    (input::KEY_PRINTSCREEN,    "print screen"  )
                    (input::KEY_INSERT,         "insert"        )
                    (input::KEY_DELETE,         "delete"        )
                    (input::KEY_0,              "0"             )
                    (input::KEY_1,              "1"             )
                    (input::KEY_2,              "2"             )
                    (input::KEY_3,              "3"             )
                    (input::KEY_4,              "4"             )
                    (input::KEY_5,              "5"             )
                    (input::KEY_6,              "6"             )
                    (input::KEY_7,              "7"             )
                    (input::KEY_8,              "8"             )
                    (input::KEY_9,              "9"             )
                    (input::KEY_A,              "a"             )
                    (input::KEY_B,              "b"             )
                    (input::KEY_C,              "c"             )
                    (input::KEY_D,              "d"             )
                    (input::KEY_E,              "e"             )
                    (input::KEY_F,              "f"             )
                    (input::KEY_G,              "g"             )
                    (input::KEY_H,              "h"             )
                    (input::KEY_I,              "i"             )
                    (input::KEY_J,              "j"             )
                    (input::KEY_K,              "k"             )
                    (input::KEY_L,              "l"             )
                    (input::KEY_M,              "m"             )
                    (input::KEY_N,              "n"             )
                    (input::KEY_O,              "o"             )
                    (input::KEY_P,              "p"             )
                    (input::KEY_Q,              "q"             )
                    (input::KEY_R,              "r"             )
                    (input::KEY_S,              "s"             )
                    (input::KEY_T,              "t"             )
                    (input::KEY_U,              "u"             )
                    (input::KEY_V,              "v"             )
                    (input::KEY_W,              "w"             )
                    (input::KEY_X,              "x"             )
                    (input::KEY_Y,              "y"             )
                    (input::KEY_Z,              "z"             )
                    (input::KEY_NUMPAD0,        "np0"           )
                    (input::KEY_NUMPAD1,        "np1"           )
                    (input::KEY_NUMPAD2,        "np2"           )
                    (input::KEY_NUMPAD3,        "np3"           )
                    (input::KEY_NUMPAD4,        "np4"           )
                    (input::KEY_NUMPAD5,        "np5"           )
                    (input::KEY_NUMPAD6,        "np6"           )
                    (input::KEY_NUMPAD7,        "np7"           )
                    (input::KEY_NUMPAD8,        "np8"           )
                    (input::KEY_NUMPAD9,        "np9"           )
                    (input::KEY_MULTIPLY,       "multiply"      )
                    (input::KEY_ADD,            "add"           )
                    (input::KEY_SUBTRACT,       "subtract"      )
                    (input::KEY_DECIMAL,        "decimal"       )
                    (input::KEY_DIVIDE,         "divide"        )
                    (input::KEY_F1,             "f1"            )
                    (input::KEY_F2,             "f2"            )
                    (input::KEY_F3,             "f3"            )
                    (input::KEY_F4,             "f4"            )
                    (input::KEY_F5,             "f5"            )
                    (input::KEY_F6,             "f6"            )
                    (input::KEY_F7,             "f7"            )
                    (input::KEY_F8,             "f8"            )
                    (input::KEY_F9,             "f9"            )
                    (input::KEY_F10,            "f10"           )
                    (input::KEY_F11,            "f11"           )
                    (input::KEY_F12,            "f12"           )
                    (input::KEY_NUMLOCK,        "num lock"      )
                    (input::KEY_SCROLLLOCK,     "scroll lock"   )
                    (input::KEY_LSHIFT,         "lshift"        )
                    (input::KEY_RSHIFT,         "rshift"        )
                    (input::KEY_LCONTROL,       "lctrl"         )
                    (input::KEY_RCONTROL,       "rctrl"         )
                    (input::KEY_LALT,           "lalt"          )
                    (input::KEY_RALT,           "ralt"          );

                map_type::iterator mapped_value = map.find(key);

                if (mapped_value == map.end()) {
                    return false;
                }

                out_key_str = mapped_value->second;
                return true;
            }

            //---------------------------------------------------------
            bool GetKeyConstant(const std::string& key_str, int& out_key)
            {
                typedef boost::unordered_map<std::string, int> map_type;

                static map_type map = boost::assign::map_list_of
                    ("backspace",    input::KEY_BACKSPACE   )
                    ("tab",          input::KEY_TAB         )
                    ("clear",        input::KEY_CLEAR       )
                    ("enter",        input::KEY_ENTER       )
                    ("shift",        input::KEY_SHIFT       )
                    ("ctrl",         input::KEY_CONTROL     )
                    ("alt",          input::KEY_ALT         )
                    ("caps lock",    input::KEY_CAPSLOCK    )
                    ("escape",       input::KEY_ESCAPE      )
                    ("space",        input::KEY_SPACE       )
                    ("page up",      input::KEY_PAGEUP      )
                    ("page down",    input::KEY_PAGEDOWN    )
                    ("end",          input::KEY_END         )
                    ("home",         input::KEY_HOME        )
                    ("left",         input::KEY_LEFT        )
                    ("up",           input::KEY_UP          )
                    ("right",        input::KEY_RIGHT       )
                    ("down",         input::KEY_DOWN        )
                    ("print screen", input::KEY_PRINTSCREEN )
                    ("insert",       input::KEY_INSERT      )
                    ("delete",       input::KEY_DELETE      )
                    ("0",            input::KEY_0           )
                    ("1",            input::KEY_1           )
                    ("2",            input::KEY_2           )
                    ("3",            input::KEY_3           )
                    ("4",            input::KEY_4           )
                    ("5",            input::KEY_5           )
                    ("6",            input::KEY_6           )
                    ("7",            input::KEY_7           )
                    ("8",            input::KEY_8           )
                    ("9",            input::KEY_9           )
                    ("a",            input::KEY_A           )
                    ("b",            input::KEY_B           )
                    ("c",            input::KEY_C           )
                    ("d",            input::KEY_D           )
                    ("e",            input::KEY_E           )
                    ("f",            input::KEY_F           )
                    ("g",            input::KEY_G           )
                    ("h",            input::KEY_H           )
                    ("i",            input::KEY_I           )
                    ("j",            input::KEY_J           )
                    ("k",            input::KEY_K           )
                    ("l",            input::KEY_L           )
                    ("m",            input::KEY_M           )
                    ("n",            input::KEY_N           )
                    ("o",            input::KEY_O           )
                    ("p",            input::KEY_P           )
                    ("q",            input::KEY_Q           )
                    ("r",            input::KEY_R           )
                    ("s",            input::KEY_S           )
                    ("t",            input::KEY_T           )
                    ("u",            input::KEY_U           )
                    ("v",            input::KEY_V           )
                    ("w",            input::KEY_W           )
                    ("x",            input::KEY_X           )
                    ("y",            input::KEY_Y           )
                    ("z",            input::KEY_Z           )
                    ("np0",          input::KEY_NUMPAD0     )
                    ("np1",          input::KEY_NUMPAD1     )
                    ("np2",          input::KEY_NUMPAD2     )
                    ("np3",          input::KEY_NUMPAD3     )
                    ("np4",          input::KEY_NUMPAD4     )
                    ("np5",          input::KEY_NUMPAD5     )
                    ("np6",          input::KEY_NUMPAD6     )
                    ("np7",          input::KEY_NUMPAD7     )
                    ("np8",          input::KEY_NUMPAD8     )
                    ("np9",          input::KEY_NUMPAD9     )
                    ("multiply",     input::KEY_MULTIPLY    )
                    ("add",          input::KEY_ADD         )
                    ("subtract",     input::KEY_SUBTRACT    )
                    ("decimal",      input::KEY_DECIMAL     )
                    ("divide",       input::KEY_DIVIDE      )
                    ("f1",           input::KEY_F1          )
                    ("f2",           input::KEY_F2          )
                    ("f3",           input::KEY_F3          )
                    ("f4",           input::KEY_F4          )
                    ("f5",           input::KEY_F5          )
                    ("f6",           input::KEY_F6          )
                    ("f7",           input::KEY_F7          )
                    ("f8",           input::KEY_F8          )
                    ("f9",           input::KEY_F9          )
                    ("f10",          input::KEY_F10         )
                    ("f11",          input::KEY_F11         )
                    ("f12",          input::KEY_F12         )
                    ("num lock",     input::KEY_NUMLOCK     )
                    ("scroll lock",  input::KEY_SCROLLLOCK  )
                    ("lshift",       input::KEY_LSHIFT      )
                    ("rshift",       input::KEY_RSHIFT      )
                    ("lctrl",        input::KEY_LCONTROL    )
                    ("rctrl",        input::KEY_RCONTROL    )
                    ("lalt",         input::KEY_LALT        )
                    ("ralt",         input::KEY_RALT        );

                map_type::iterator mapped_value = map.find(key_str);

                if (mapped_value == map.end()) {
                    return false;
                }

                out_key = mapped_value->second;
                return true;
            }

        } // namespace keyboard_module
    } // namespace script
} // namespace rpgss
