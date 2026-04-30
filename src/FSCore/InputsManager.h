#pragma once

namespace FSC
{
    enum States
    {
        NONE = 0b00,
        PRESSED = 0b01,
        UP = 0b10,
        DOWN = 0b11
    };


    class InputsManager
    {
        inline static unordered_map<int, int> m_KeyboardMap;
        inline static unordered_map<int, int> m_MouseMap;

        inline static States m_MouseStates[Mouse::AMOUNT];
        inline static States m_KeyboardStates[Keyboard::AMOUNT];

        inline static HWND m_WinHandle = nullptr;

        inline static InputsManager* m_Instance = nullptr;

        inline static bool m_IsCursorLocked = false;
        inline static bool m_IsCursorVisible = true;
        inline static int m_CursorVisibilityCount = 0;
		inline static int m_WheelDelta = 0;

        InputsManager();
    public:
        static InputsManager* Create();

        static bool GetButton(Mouse::Button button);
        static bool GetButtonUp(Mouse::Button button);
        static bool GetButtonDown(Mouse::Button button);

        static XMINT2 GetMousePosition();
        static void SetMousePosition(XMINT2 const& coordinates);
       
        static bool GetKey(Keyboard::Key key);
        static bool GetKeyUp(Keyboard::Key key);
        static bool GetKeyDown(Keyboard::Key key);

        static void SetHandle(HWND handle) { m_WinHandle = handle; }

		static void OnMouseWheelUp();
		static void OnMouseWheelDown();

        static int GetMouseWheelDelta();
		static bool GetMouseWheelUp();
		static bool GetMouseWheelDown();

        static void LockMouseCursor();
        static void UnlockMouseCursor();
        static bool IsMouseCursorLocked();
        static void ShowMouseCursor();
        static void HideMouseCursor();
        static bool IsMouseCursorVisible();

        static void HandleInputs();
		static void ClearInputs();
    };

}