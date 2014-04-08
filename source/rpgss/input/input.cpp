#define NOT_MAIN_MODULE
#include <DynRPG/DynRPG.h>

#include "input.hpp"


namespace rpgss {
    namespace input {

        static int KeyToVKey[] = {
            VK_BACK,
            VK_TAB,
            VK_CLEAR,
            VK_RETURN,
            VK_SHIFT,
            VK_CONTROL,
            VK_MENU,
            VK_CAPITAL,
            VK_ESCAPE,
            VK_SPACE,
            VK_PRIOR,
            VK_NEXT,
            VK_END,
            VK_HOME,
            VK_LEFT,
            VK_UP,
            VK_RIGHT,
            VK_DOWN,
            VK_SNAPSHOT,
            VK_INSERT,
            VK_DELETE,
            '0',
            '1',
            '2',
            '3',
            '4',
            '5',
            '6',
            '7',
            '8',
            '9',
            'A',
            'B',
            'C',
            'D',
            'E',
            'F',
            'G',
            'H',
            'I',
            'J',
            'K',
            'L',
            'M',
            'N',
            'O',
            'P',
            'Q',
            'R',
            'S',
            'T',
            'U',
            'V',
            'W',
            'X',
            'Y',
            'Z',
            VK_NUMPAD0,
            VK_NUMPAD1,
            VK_NUMPAD2,
            VK_NUMPAD3,
            VK_NUMPAD4,
            VK_NUMPAD5,
            VK_NUMPAD6,
            VK_NUMPAD7,
            VK_NUMPAD8,
            VK_NUMPAD9,
            VK_MULTIPLY,
            VK_ADD,
            VK_SUBTRACT,
            VK_DECIMAL,
            VK_DIVIDE,
            VK_F1,
            VK_F2,
            VK_F3,
            VK_F4,
            VK_F5,
            VK_F6,
            VK_F7,
            VK_F8,
            VK_F9,
            VK_F10,
            VK_F11,
            VK_F12,
            VK_NUMLOCK,
            VK_SCROLL,
            VK_LSHIFT,
            VK_RSHIFT,
            VK_LCONTROL,
            VK_RCONTROL,
            VK_LMENU,
            VK_RMENU,
        };

        static int MouseButtonToVKey[] = {
            VK_LBUTTON,
            VK_RBUTTON,
            VK_MBUTTON,
            0x05,
            0x06,
        };

        //-----------------------------------------------------------------
        bool IsKeyPressed(int key)
        {
            if (key >= 0 && key < NUMKEYS_) {
                return (GetAsyncKeyState(KeyToVKey[key]) & 0x8000);
            }
            return false;
        }

        //-----------------------------------------------------------------
        bool IsAnyKeyPressed()
        {
            for (int key = 0; key < NUMKEYS_; ++key) {
                if (IsKeyPressed(key)) {
                    return true;
                }
            }
            return false;
        }

        //-----------------------------------------------------------------
        core::Vec2i GetMousePosition()
        {
            POINT mousePos;
            GetCursorPos(&mousePos);
            ScreenToClient(RPG::screen->getCanvasHWND(), &mousePos);
            if(!RPG::screen->fullScreen) {
                mousePos.x -= 2;
                mousePos.y -= 2;
                if(RPG::screen->largeWindow) {
                    mousePos.x /= 2;
                    mousePos.y /= 2;
                }
            }
            return core::Vec2i(mousePos.x, mousePos.y);
        }

        //-----------------------------------------------------------------
        bool IsMouseButtonPressed(int mbutton)
        {
            if (mbutton >= 0 && mbutton < NUMMOUSEBUTTONS_) {
                return (GetAsyncKeyState(MouseButtonToVKey[mbutton]) & 0x8000);
            }
            return false;
        }

        //-----------------------------------------------------------------
        bool IsAnyMouseButtonPressed()
        {
            for (int mb = 0; mb < NUMMOUSEBUTTONS_; ++mb) {
                if (IsMouseButtonPressed(mb)) {
                    return true;
                }
            }
            return false;
        }

    } // namespace input
} // namespace rpgss
