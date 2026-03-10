#pragma once
class FS_InputsManager
{
	inline static FS_InputsManager* s_instance = nullptr;

public:
    static FS_InputsManager* Get();
    
    enum States 
    {
        NONE = 0b00,
        PRESSED = 0b01,
        UP = 0b10,
        DOWN = 0b11
    };

	FS_InputsManager();

    static void HandleInputs();

    static unordered_map<int, int> s_keyboardMap;
    static unordered_map<int, int> s_mouseMap;

    inline static States s_mouseStates[Mouse::AMOUNT];
    inline static States s_keyboardStates[Keyboard::AMOUNT];

    inline static HWND s_windowHandle = nullptr;
    inline static bool s_cursorLocked = false;
    inline static bool s_cursorVisible = true;
    inline static int s_cursorVisibilityCount = 0;

    bool GetButton(Mouse::Button button);
    bool GetButtonUp(Mouse::Button button);
    bool GetButtonDown(Mouse::Button button);

    XMINT2 GetMousePosition();
    void SetMousePosition(XMINT2 const& coordinates);

    void LockMouseCursor();
    void UnlockMouseCursor();
    bool IsMouseCursorLocked();

    void ShowMouseCursor();
    void HideMouseCursor();
    bool IsMouseCursorVisible();

    bool GetKey(Keyboard::Key key);
    bool GetKeyUp(Keyboard::Key key);
    bool GetKeyDown(Keyboard::Key key);
};

