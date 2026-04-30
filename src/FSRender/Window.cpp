#include "pch.h"

namespace FSR
{
	LRESULT CALLBACK
		MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		// Forward hwnd on because we can get messages (e.g., WM_CREATE)
		// before CreateWindow returns, and thus before mhMainWnd is valid.
		return Device::Get()->ProcessWindowMessages(hwnd, msg, wParam, lParam);
	}

	Window::Window(HINSTANCE hInstance) : m_HAppInst(hInstance)
	{
	}

	bool Window::InitMainWindow()
	{
		WNDCLASS wc;
		wc.style = CS_HREDRAW | CS_VREDRAW;
		wc.lpfnWndProc = MainWndProc;
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;
		wc.hInstance = m_HAppInst;
		wc.hIcon = (HICON)LoadImage(
			NULL,
			L"../../res/icon/FSE_Logo.ico",
			IMAGE_ICON,
			0,
			0,
			LR_LOADFROMFILE | LR_DEFAULTSIZE
		);
		wc.hCursor = LoadCursor(0, IDC_ARROW);
		wc.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
		wc.lpszMenuName = 0;
		wc.lpszClassName = L"MainWnd";

		if (!RegisterClass(&wc))
		{
			MessageBox(0, L"RegisterClass Failed.", 0, 0);
			return false;
		}

		RECT R = { 0, 0, m_ClientWidth, m_ClientHeight };
		AdjustWindowRect(&R, WS_OVERLAPPEDWINDOW, false);
		int width = R.right - R.left;
		int height = R.bottom - R.top;

		m_HMainWnd = CreateWindow(L"MainWnd", m_MainWndCaption.c_str(),
			WS_OVERLAPPEDWINDOW, 0, 0, width, height, 0, 0, m_HAppInst, 0);
		if (!m_HMainWnd)
		{
			MessageBox(0, L"CreateWindow Failed.", 0, 0);
			return false;
		}

		ShowWindow(m_HMainWnd, SW_SHOW);
		UpdateWindow(m_HMainWnd);

		return true;
	}

	void Window::SetDimension(int width, int height)
	{
		m_ClientWidth = width;
		m_ClientHeight = height;
		MoveWindow(m_HMainWnd, m_ClientPosX, m_ClientPosY, m_ClientWidth, m_ClientHeight, true);
		Device::GetSwapChain()->OnResize();
	}

	void Window::SetPosition(int x, int y)
	{
		m_ClientPosX = x;
		m_ClientPosY = y;
		MoveWindow(m_HMainWnd, m_ClientPosX, m_ClientPosY, m_ClientWidth, m_ClientHeight, true);
		Device::GetSwapChain()->OnResize();
	}

	void Window::SetWindowName(std::wstring name)
	{
		m_MainWndCaption = name;
		SetWindowText(m_HMainWnd, m_MainWndCaption.c_str());
	}

	float Window::AspectRatio()const
	{
		return static_cast<float>(m_ClientWidth) / m_ClientHeight;
	}

	void Window::UpdateScreenViewport()
	{
		m_ScreenViewport.TopLeftX = 0;
		m_ScreenViewport.TopLeftY = 0;
		m_ScreenViewport.Width = static_cast<float>(m_ClientWidth);
		m_ScreenViewport.Height = static_cast<float>(m_ClientHeight);
		m_ScreenViewport.MinDepth = 0.0f;
		m_ScreenViewport.MaxDepth = 1.0f;

		m_ScissorRect = { 0, 0, m_ClientWidth, m_ClientHeight };
	}

	HBITMAP hBitmap;
	LRESULT Window::MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		//FS_Clock& time = FS_Device::Get()->m_DeltaTime; //Non
		switch (msg)
		{
		case WM_CREATE:
		{
			hBitmap = (HBITMAP)LoadImage(
				NULL,
				L"..\\..\\res\\UI\\FSE_Loading_720p.bmp",
				IMAGE_BITMAP,
				0,
				0,
				LR_LOADFROMFILE | LR_CREATEDIBSECTION);
			return 0;
		}
		case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hwnd, &ps);

			HDC hdcMem = CreateCompatibleDC(hdc);
			SelectObject(hdcMem, hBitmap);

			BITMAP bmp;
			GetObject(hBitmap, sizeof(BITMAP), &bmp);

			BitBlt(
				hdc,
				0, 0,
				bmp.bmWidth,
				bmp.bmHeight,
				hdcMem,
				0, 0,
				SRCCOPY
			);

			DeleteDC(hdcMem);

			EndPaint(hwnd, &ps);
			return 0;
		}
		case WM_ACTIVATE:
			//if (LOWORD(wParam) == WA_INACTIVE)
			//{
			//	mAppPaused = true;
			//	time.Stop();
			//}
			//else
			//{
			//	mAppPaused = false;
			//	time.Start();
			//}
			return 0;

		case WM_SIZE:
		{
			m_ClientWidth = LOWORD(lParam);
			m_ClientHeight = HIWORD(lParam);

			if (Device::GetD3DDevice())
			{
				if (wParam == SIZE_MINIMIZED)
				{
					m_AppPaused = true;
					m_Minimized = true;
					m_Maximized = false;
				}
				else if (wParam == SIZE_MAXIMIZED)
				{
					m_AppPaused = false;
					m_Minimized = false;
					m_Maximized = true;
					Device::GetSwapChain()->OnResize();
				}
				else if (wParam == SIZE_RESTORED)
				{
					if (m_Minimized)
					{
						m_AppPaused = false;
						m_Minimized = false;
						Device::GetSwapChain()->OnResize();
					}
					else if (m_Maximized)
					{
						m_AppPaused = false;
						m_Maximized = false;
						Device::GetSwapChain()->OnResize();
					}
					else if (m_Resizing)
					{
					}
					else
					{
						Device::GetSwapChain()->OnResize();
					}
				}
			}
			return 0;
		}


		case WM_ENTERSIZEMOVE:
			m_AppPaused = true;
			m_Resizing = true;
			//time.Stop();
			return 0;

		case WM_EXITSIZEMOVE:
			m_AppPaused = false;
			m_Resizing = false;
			//time.Start();
			Device::GetSwapChain()->OnResize();
			return 0;

		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;

		case WM_MENUCHAR:
			return MAKELRESULT(0, MNC_CLOSE);

		case WM_GETMINMAXINFO:
			((MINMAXINFO*)lParam)->ptMinTrackSize.x = 200;
			((MINMAXINFO*)lParam)->ptMinTrackSize.y = 200;
			return 0;

		case WM_LBUTTONDOWN:
		case WM_MBUTTONDOWN:
		case WM_RBUTTONDOWN:
			//OnMouseDown(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
			return 0;
		case WM_LBUTTONUP:
		case WM_MBUTTONUP:
		case WM_RBUTTONUP:
			//OnMouseUp(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
			return 0;
		case WM_MOUSEMOVE:
			//OnMouseMove(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
			return 0;
		case WM_KEYUP:
			if (wParam == VK_ESCAPE)
			{
				PostQuitMessage(0);
			}
			else if ((int)wParam == VK_F2)
				int a = 0;
			return 0;
		case WM_MOUSEWHEEL:
		{
			int delta = GET_WHEEL_DELTA_WPARAM(wParam);

			if (delta > 0)
				InputsManager::OnMouseWheelUp();
			else if (delta < 0)
				InputsManager::OnMouseWheelDown();

			return 0;
		}
		case 132:
			return 0;
			//case 32:
			//	return 0;
		}

		return DefWindowProc(hwnd, msg, wParam, lParam);
	}
}

