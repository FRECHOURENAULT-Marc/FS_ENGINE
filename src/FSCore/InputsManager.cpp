#include "pch.h"

namespace FSC
{
    InputsManager::InputsManager()
    {

        m_KeyboardMap =
        {
            { Keyboard::BACKSPACE,       VK_BACK     },
            { Keyboard::TAB,             VK_TAB      },
            { Keyboard::RETURN,          VK_RETURN   },
            { Keyboard::PAUSE,           VK_PAUSE    },
            { Keyboard::CAPSLOCK,        VK_CAPITAL  },
            { Keyboard::ESCAPE,          VK_ESCAPE   },
            { Keyboard::SPACE,           VK_SPACE    },
            { Keyboard::PAGE_UP,         VK_PRIOR    },
            { Keyboard::PAGE_DOWN,       VK_NEXT     },
            { Keyboard::END,             VK_END      },
            { Keyboard::HOME,            VK_HOME     },
            { Keyboard::LEFT,            VK_LEFT     },
            { Keyboard::UP,              VK_UP       },
            { Keyboard::RIGHT,           VK_RIGHT    },
            { Keyboard::DOWN,            VK_DOWN     },
            { Keyboard::INSERT,          VK_INSERT   },
            { Keyboard::DELETE_,         VK_DELETE   },
            { Keyboard::LWINDOW,         VK_LWIN     },
            { Keyboard::RWINDOW,         VK_RWIN     },
            { Keyboard::NUMPAD0,         VK_NUMPAD0  },
            { Keyboard::NUMPAD1,         VK_NUMPAD1  },
            { Keyboard::NUMPAD2,         VK_NUMPAD2  },
            { Keyboard::NUMPAD3,         VK_NUMPAD3  },
            { Keyboard::NUMPAD4,         VK_NUMPAD4  },
            { Keyboard::NUMPAD5,         VK_NUMPAD5  },
            { Keyboard::NUMPAD6,         VK_NUMPAD6  },
            { Keyboard::NUMPAD7,         VK_NUMPAD7  },
            { Keyboard::NUMPAD8,         VK_NUMPAD8  },
            { Keyboard::NUMPAD9,         VK_NUMPAD9  },
            { Keyboard::NUMPAD_MULTIPLY, VK_MULTIPLY },
            { Keyboard::NUMPAD_ADD,      VK_ADD      },
            { Keyboard::NUMPAD_SUBTRACT, VK_SUBTRACT },
            { Keyboard::NUMPAD_DECIMAL,  VK_DECIMAL  },
            { Keyboard::NUMPAD_DIVIDE,   VK_DIVIDE   },
            { Keyboard::F1,              VK_F1       },
            { Keyboard::F2,              VK_F2       },
            { Keyboard::F3,              VK_F3       },
            { Keyboard::F4,              VK_F4       },
            { Keyboard::F5,              VK_F5       },
            { Keyboard::F6,              VK_F6       },
            { Keyboard::F7,              VK_F7       },
            { Keyboard::F8,              VK_F8       },
            { Keyboard::F9,              VK_F9       },
            { Keyboard::F10,             VK_F10      },
            { Keyboard::F11,             VK_F11      },
            { Keyboard::F12,             VK_F12      },
            { Keyboard::NUMLOCK,         VK_NUMLOCK  },
            { Keyboard::SCROLL_LOCK,     VK_SCROLL   },
            { Keyboard::LSHIFT,          VK_LSHIFT   },
            { Keyboard::RSHIFT,          VK_RSHIFT   },
            { Keyboard::LCTRL,           VK_LCONTROL },
            { Keyboard::RCTRL,           VK_RCONTROL },
            { Keyboard::LALT,            VK_LMENU    },
            { Keyboard::RALT,            VK_RMENU    },
            { Keyboard::A,              'A'          },
            { Keyboard::B,              'B'          },
            { Keyboard::C,              'C'          },
            { Keyboard::D,              'D'          },
            { Keyboard::E,              'E'          },
            { Keyboard::F,              'F'          },
            { Keyboard::G,              'G'          },
            { Keyboard::H,              'H'          },
            { Keyboard::I,              'I'          },
            { Keyboard::J,              'J'          },
            { Keyboard::K,              'K'          },
            { Keyboard::L,              'L'          },
            { Keyboard::M,              'M'          },
            { Keyboard::N,              'N'          },
            { Keyboard::O,              'O'          },
            { Keyboard::P,              'P'          },
            { Keyboard::Q,              'Q'          },
            { Keyboard::R,              'R'          },
            { Keyboard::S,              'S'          },
            { Keyboard::T,              'T'          },
            { Keyboard::U,              'U'          },
            { Keyboard::V,              'V'          },
            { Keyboard::W,              'W'          },
            { Keyboard::X,              'X'          },
            { Keyboard::Y,              'Y'          },
            { Keyboard::Z,              'Z'          },
            { Keyboard::_0,             '0'          },
            { Keyboard::_1,             '1'          },
            { Keyboard::_2,             '2'          },
            { Keyboard::_3,             '3'          },
            { Keyboard::_4,             '4'          },
            { Keyboard::_5,             '5'          },
            { Keyboard::_6,             '6'          },
            { Keyboard::_7,             '7'          },
            { Keyboard::_8,             '8'          },
            { Keyboard::_9,             '9'          },
            { Keyboard::²,              VK_OEM_7     },
        };

        m_MouseMap =
        {
            { Mouse::LEFT,   VK_LBUTTON  },
            { Mouse::RIGHT,  VK_RBUTTON  },
            { Mouse::MIDDLE, VK_MBUTTON  },
            { Mouse::EXTRA_1, VK_XBUTTON1 },
            { Mouse::EXTRA_2, VK_XBUTTON2 },
        };
    }

    InputsManager* InputsManager::Create()
    {
        if (m_Instance == nullptr)
            m_Instance = new InputsManager();

        return m_Instance;
    }

    void InputsManager::HandleInputs()
    {
        for (int i = 0; i < Keyboard::AMOUNT; i++)
            m_KeyboardStates[i] = GetAsyncKeyState(m_KeyboardMap[i]) ?
            (m_KeyboardStates[i] & PRESSED ? PRESSED : DOWN) :
            (m_KeyboardStates[i] & PRESSED ? UP : NONE);

        for (int i = 0; i < Mouse::AMOUNT; i++)
            m_MouseStates[i] = GetAsyncKeyState(m_MouseMap[i]) ?
            (m_MouseStates[i] & PRESSED ? PRESSED : DOWN) :
            (m_MouseStates[i] & PRESSED ? UP : NONE);
    }

    void InputsManager::ClearInputs()
    {
        m_WheelDelta = 0;
    }

    XMINT2 InputsManager::GetMousePosition()
    {
        POINT p;
        GetCursorPos(&p);
        ScreenToClient(m_WinHandle, &p);
        return { p.x, p.y };
    }

    void InputsManager::SetMousePosition(XMINT2 const& coordinates)
    {
        POINT p{ coordinates.x, coordinates.y };
        ClientToScreen(m_WinHandle, &p);
        SetCursorPos(p.x, p.y);
    }

    void InputsManager::OnMouseWheelUp()
    {
        m_WheelDelta = 1;
    }

    void InputsManager::OnMouseWheelDown()
    {
        m_WheelDelta = -1;
    }

    int InputsManager::GetMouseWheelDelta()
    {
        return m_WheelDelta;
    }

    bool InputsManager::GetMouseWheelUp()
    {
        return m_WheelDelta > 0;
    }

    bool InputsManager::GetMouseWheelDown()
    {
        return m_WheelDelta < 0;
    }

    void InputsManager::LockMouseCursor()
    {
        if (m_WinHandle == nullptr) return;
        m_IsCursorLocked = true;

        if (m_IsCursorLocked == false || m_WinHandle == nullptr) return;

        RECT clientRect;
        if (GetClientRect(m_WinHandle, &clientRect) == false) return;

        POINT topLeft = { clientRect.left, clientRect.top };
        POINT bottomRight = { clientRect.right, clientRect.bottom };

        ClientToScreen(m_WinHandle, &topLeft);
        ClientToScreen(m_WinHandle, &bottomRight);

        RECT const clipRect = { topLeft.x, topLeft.y, bottomRight.x, bottomRight.y };
        ClipCursor(&clipRect);
    }

    void InputsManager::UnlockMouseCursor()
    {
        m_IsCursorLocked = false;
        ClipCursor(nullptr);
    }

    void InputsManager::ShowMouseCursor()
    {
        if (m_IsCursorVisible) return;
        m_IsCursorVisible = true;

        while (m_CursorVisibilityCount < 0)
            m_CursorVisibilityCount = ShowCursor(TRUE);
    }

    void InputsManager::HideMouseCursor()
    {
        if (m_IsCursorVisible == false) return;
        m_IsCursorVisible = false;

        while (m_CursorVisibilityCount >= 0)
            m_CursorVisibilityCount = ShowCursor(FALSE);
    }

    bool InputsManager::GetButton(Mouse::Button const button)
    {
        return m_MouseStates[button] == States::PRESSED;
    }

    bool InputsManager::GetButtonUp(Mouse::Button const button)
    {
        return m_MouseStates[button] == States::UP;
    }

    bool InputsManager::GetButtonDown(Mouse::Button const button)
    {
        return m_MouseStates[button] == States::DOWN;
    }

    bool InputsManager::IsMouseCursorLocked()
    {
        return m_IsCursorLocked;
    }

    bool InputsManager::IsMouseCursorVisible()
    {
        return m_IsCursorVisible;
    }

    bool InputsManager::GetKey(Keyboard::Key const key)
    {
        return m_KeyboardStates[key] & States::PRESSED;
    }

    bool InputsManager::GetKeyUp(Keyboard::Key const key)
    {
        return m_KeyboardStates[key] == States::UP;
    }

    bool InputsManager::GetKeyDown(Keyboard::Key const key)
    {
        return m_KeyboardStates[key] == States::DOWN;
    }

}