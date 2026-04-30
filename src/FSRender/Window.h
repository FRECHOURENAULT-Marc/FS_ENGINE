#pragma once

namespace FSR
{
	class Window
	{
	public:
		Window(HINSTANCE hInstance);

		bool InitMainWindow();
		bool IsClosed() { return m_isClosed; };
		bool IsPaused() { return m_AppPaused; };

		void SetDimension(int width, int height);
		void SetPosition(int x, int y);
		void SetWindowName(std::wstring name);
		void SetClosed(bool closed) { m_isClosed = closed; };

		float AspectRatio()const;
		const int& Width() { return m_ClientWidth; };
		const int& Height() { return m_ClientHeight; };
		XMINT2 Size() { return { m_ClientWidth, m_ClientHeight }; };
		constexpr bool IsFullScreen()const { return m_FullscreenState; };
		constexpr HINSTANCE GetAppInstance()const { return m_HAppInst; };
		constexpr HWND GetWindowHandle()const { return m_HMainWnd; };

		void UpdateScreenViewport();

		constexpr D3D12_VIEWPORT GetViewPort()const { return m_ScreenViewport; };
		constexpr D3D12_RECT GetScissorRect()const { return m_ScissorRect; };

	private:

		LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

		friend class Device;


		HINSTANCE	m_HAppInst = nullptr;
		HWND		m_HMainWnd = nullptr;
		std::wstring m_MainWndCaption = L"FS-Engine"; 
		int			m_ClientWidth = 1280;
		int			m_ClientHeight = 720;
		int			m_ClientPosX = 0;
		int			m_ClientPosY = 0;
		bool		m_AppPaused = false;
		bool		m_Minimized = false;
		bool		m_Maximized = false;
		bool		m_Resizing = false; 
		bool		m_FullscreenState = false;

		D3D12_VIEWPORT m_ScreenViewport;
		D3D12_RECT m_ScissorRect;

		bool m_isClosed = false;
	};
}