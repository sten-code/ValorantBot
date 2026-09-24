#include "InputUtils.h"

#include <Windows.h>

int InputUtils::KeyToVK(const ImGuiKey key)
{
    switch (key) {
        case ImGuiKey_Tab: return VK_TAB;
        case ImGuiKey_LeftArrow: return VK_LEFT;
        case ImGuiKey_RightArrow: return VK_RIGHT;
        case ImGuiKey_UpArrow: return VK_UP;
        case ImGuiKey_DownArrow: return VK_DOWN;
        case ImGuiKey_PageUp: return VK_PRIOR;
        case ImGuiKey_PageDown: return VK_NEXT;
        case ImGuiKey_Home: return VK_HOME;
        case ImGuiKey_End: return VK_END;
        case ImGuiKey_Insert: return VK_INSERT;
        case ImGuiKey_Delete: return VK_DELETE;
        case ImGuiKey_Backspace: return VK_BACK;
        case ImGuiKey_Space: return VK_SPACE;
        case ImGuiKey_Enter: return VK_RETURN;
        case ImGuiKey_Escape: return VK_ESCAPE;

        // Left
        case ImGuiKey_LeftCtrl: return VK_LCONTROL;
        case ImGuiKey_LeftShift: return VK_LSHIFT;
        case ImGuiKey_LeftAlt: return VK_LMENU;
        case ImGuiKey_LeftSuper: return VK_LWIN;

        // Right
        case ImGuiKey_RightCtrl: return VK_RCONTROL;
        case ImGuiKey_RightShift: return VK_RSHIFT;
        case ImGuiKey_RightAlt: return VK_RMENU;
        case ImGuiKey_RightSuper: return VK_RWIN;

        case ImGuiKey_Menu: return VK_MENU;

        case ImGuiKey_0: return '0';
        case ImGuiKey_1: return '1';
        case ImGuiKey_2: return '2';
        case ImGuiKey_3: return '3';
        case ImGuiKey_4: return '4';
        case ImGuiKey_5: return '5';
        case ImGuiKey_6: return '6';
        case ImGuiKey_7: return '7';
        case ImGuiKey_8: return '8';
        case ImGuiKey_9: return '9';

        case ImGuiKey_A: return 'A';
        case ImGuiKey_B: return 'B';
        case ImGuiKey_C: return 'C';
        case ImGuiKey_D: return 'D';
        case ImGuiKey_E: return 'E';
        case ImGuiKey_F: return 'F';
        case ImGuiKey_G: return 'G';
        case ImGuiKey_H: return 'H';
        case ImGuiKey_I: return 'I';
        case ImGuiKey_J: return 'J';
        case ImGuiKey_K: return 'K';
        case ImGuiKey_L: return 'L';
        case ImGuiKey_M: return 'M';
        case ImGuiKey_N: return 'N';
        case ImGuiKey_O: return 'O';
        case ImGuiKey_P: return 'P';
        case ImGuiKey_Q: return 'Q';
        case ImGuiKey_R: return 'R';
        case ImGuiKey_S: return 'S';
        case ImGuiKey_T: return 'T';
        case ImGuiKey_U: return 'U';
        case ImGuiKey_V: return 'V';
        case ImGuiKey_W: return 'W';
        case ImGuiKey_X: return 'X';
        case ImGuiKey_Y: return 'Y';
        case ImGuiKey_Z: return 'Z';

        case ImGuiKey_F1: return VK_F1;
        case ImGuiKey_F2: return VK_F2;
        case ImGuiKey_F3: return VK_F3;
        case ImGuiKey_F4: return VK_F4;
        case ImGuiKey_F5: return VK_F5;
        case ImGuiKey_F6: return VK_F6;
        case ImGuiKey_F7: return VK_F7;
        case ImGuiKey_F8: return VK_F8;
        case ImGuiKey_F9: return VK_F9;
        case ImGuiKey_F10: return VK_F10;
        case ImGuiKey_F11: return VK_F11;
        case ImGuiKey_F12: return VK_F12;

        case ImGuiKey_Apostrophe: return VK_OEM_7;
        case ImGuiKey_Comma: return VK_OEM_COMMA;
        case ImGuiKey_Minus: return VK_OEM_MINUS;
        case ImGuiKey_Period: return VK_OEM_PERIOD;
        case ImGuiKey_Slash: return VK_OEM_2;
        case ImGuiKey_Semicolon: return VK_OEM_1;
        case ImGuiKey_Equal: return VK_OEM_PLUS;
        case ImGuiKey_LeftBracket: return VK_OEM_4;
        case ImGuiKey_Backslash: return VK_OEM_5;
        case ImGuiKey_RightBracket: return VK_OEM_6;
        case ImGuiKey_GraveAccent: return VK_OEM_3;
        case ImGuiKey_CapsLock: return VK_CAPITAL;
        case ImGuiKey_ScrollLock: return VK_SCROLL;
        case ImGuiKey_NumLock: return VK_NUMLOCK;
        case ImGuiKey_PrintScreen: return VK_SNAPSHOT;
        case ImGuiKey_Pause: return VK_PAUSE;

        case ImGuiKey_Keypad0: return VK_NUMPAD0;
        case ImGuiKey_Keypad1: return VK_NUMPAD1;
        case ImGuiKey_Keypad2: return VK_NUMPAD2;
        case ImGuiKey_Keypad3: return VK_NUMPAD3;
        case ImGuiKey_Keypad4: return VK_NUMPAD4;
        case ImGuiKey_Keypad5: return VK_NUMPAD5;
        case ImGuiKey_Keypad6: return VK_NUMPAD6;
        case ImGuiKey_Keypad7: return VK_NUMPAD7;
        case ImGuiKey_Keypad8: return VK_NUMPAD8;
        case ImGuiKey_Keypad9: return VK_NUMPAD9;
        case ImGuiKey_KeypadDecimal: return VK_DECIMAL;
        case ImGuiKey_KeypadDivide: return VK_DIVIDE;
        case ImGuiKey_KeypadMultiply: return VK_MULTIPLY;
        case ImGuiKey_KeypadSubtract: return VK_SUBTRACT;
        case ImGuiKey_KeypadAdd: return VK_ADD;
        case ImGuiKey_KeypadEnter: return VK_RETURN;
        case ImGuiKey_KeypadEqual: return VK_OEM_NEC_EQUAL;

        case ImGuiMod_Ctrl: return VK_CONTROL;
        case ImGuiMod_Shift: return VK_SHIFT;
        case ImGuiMod_Alt: return VK_MENU;
        case ImGuiMod_Super: return VK_LWIN;
        case ImGuiKey_MouseLeft: return VK_LBUTTON;
        case ImGuiKey_MouseRight: return VK_RBUTTON;
        case ImGuiKey_MouseMiddle: return VK_MBUTTON;
        case ImGuiKey_MouseX1: return VK_XBUTTON1;
        case ImGuiKey_MouseX2: return VK_XBUTTON2;

        default: return 0;
    }
}