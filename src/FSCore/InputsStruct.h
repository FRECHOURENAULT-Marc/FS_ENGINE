#pragma once

struct Keyboard 
{
    enum Key 
    {
        F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12,

        A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z,

        _0, _1, _2, _3, _4, _5, _6, _7, _8, _9,
        NUMPAD0, NUMPAD1, NUMPAD2, NUMPAD3, NUMPAD4, NUMPAD5, NUMPAD6, NUMPAD7, NUMPAD8, NUMPAD9,
        NUMPAD_DIVIDE, NUMPAD_MULTIPLY, NUMPAD_SUBTRACT, NUMPAD_ADD, NUMPAD_RETURN, NUMPAD_DECIMAL,

        NUMLOCK, CAPSLOCK, SCROLL_LOCK,
        PAUSE,

        ²,

        LCTRL, RCTRL,
        LSHIFT, RSHIFT,
        LALT, RALT,
        LWINDOW, RWINDOW,

        UP, LEFT, DOWN, RIGHT,

        ESCAPE, TAB, SPACE, BACKSPACE, RETURN,
        INSERT, DELETE_,
        HOME, END,
        PAGE_UP, PAGE_DOWN,

        AMOUNT,
    };
};

struct Mouse 
{
    enum Button 
    {
        LEFT, RIGHT, MIDDLE,
        EXTRA_1, EXTRA_2,
        AMOUNT,
    };
};
