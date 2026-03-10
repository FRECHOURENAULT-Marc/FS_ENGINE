#pragma once

class FS_Window final
{
public:
	FS_Window(HINSTANCE hInstance);

	bool InitMainWindow();
	bool IsClosed() { return m_isClosed; };
	bool IsPaused() { return mAppPaused; };

	void SetDimension(int width, int height);
	void SetPosition(int x, int y);
	void SetWindowName(std::wstring name);

	float AspectRatio()const;
	int Width() { return mClientWidth; };
	int Height() { return mClientHeight; };
	constexpr bool IsFullScreen()const { return mFullscreenState; };
	constexpr HINSTANCE GetAppInstance()const { return mhAppInst; };
	constexpr HWND GetWindowHandle()const { return mhMainWnd; };

	void UpdateScreenViewport();

	constexpr D3D12_VIEWPORT GetViewPort()const { return mScreenViewport; };
	constexpr D3D12_RECT GetScissorRect()const { return mScissorRect; };

	bool m_isClosed = false;

private:

	LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	friend class FS_Device;

	//<Windows----
	HINSTANCE	mhAppInst = nullptr; // application instance handle
	HWND		mhMainWnd = nullptr; // main window handle
	std::wstring mMainWndCaption = L"d3d App"; // utilis� dans InitMainWindow() et CalculateFrameStats()
	int			mClientWidth = 800; // utilis� CreateSwapChain(), AspectRatio(), InitMainWindow(), OnResize() et MsgProc()
	int			mClientHeight = 600; // utilis� CreateSwapChain(), AspectRatio(), InitMainWindow(), OnResize() et MsgProc()
	int			mClientPosX = 0;
	int			mClientPosY = 0;
	bool		mAppPaused = false;  // is the application paused?
	bool		mMinimized = false;  // is the application minimized?
	bool		mMaximized = false;  // is the application maximized?
	bool		mResizing = false;   // are the resize bars being dragged?
	bool		mFullscreenState = false;// fullscreen enabled

	D3D12_VIEWPORT mScreenViewport; // utilis� dans OnResize() et Draw()
	D3D12_RECT mScissorRect; // utilis� dans OnResize() et Draw()
	//----Windows>
};