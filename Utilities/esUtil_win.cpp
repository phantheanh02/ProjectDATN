#include "stdafx.h"
#include <windows.h>
#include <chrono>
#include <thread>
#include "esUtil.h"

POINT WDpoint;

// Main window procedure
LRESULT WINAPI ESWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT  lRet = 1;

	switch (uMsg)
	{
	case WM_CREATE:
		break;

	case WM_PAINT:
	{
		ESContext* esContext = (ESContext*)(LONG_PTR)GetWindowLongPtr(hWnd, GWL_USERDATA);

		if (esContext && esContext->drawFunc)
			esContext->drawFunc(esContext);

		ValidateRect(esContext->hWnd, NULL);
	}
	break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_KEYDOWN:
	{
		ESContext* esContext = (ESContext*)(LONG_PTR)GetWindowLongPtr(hWnd, GWL_USERDATA);

		if (esContext && esContext->keyFunc)
			esContext->keyFunc(esContext, (unsigned char)wParam, true);
	}
	break;

	case WM_KEYUP:
	{
		ESContext* esContext = (ESContext*)(LONG_PTR)GetWindowLongPtr(hWnd, GWL_USERDATA);

		if (esContext && esContext->keyFunc)
			esContext->keyFunc(esContext, (unsigned char)wParam, false);
	}
	break;

	case WM_LBUTTONUP:
	{
		ESContext* esContext = (ESContext*)(LONG_PTR)GetWindowLongPtr(hWnd, GWL_USERDATA);
		POINT point;
		GetCursorPos(&point);

		if (esContext && esContext->mouseButtonFunc)
		{
			esContext->mouseButtonFunc(esContext, (int)point.x - WDpoint.x, (int)point.y - WDpoint.y, MOUSE_LEFT_BUTTON, false);
		}
	}
	break;

	case WM_RBUTTONUP:
	{
		ESContext* esContext = (ESContext*)(LONG_PTR)GetWindowLongPtr(hWnd, GWL_USERDATA);
		POINT point;
		GetCursorPos(&point);

		if (esContext && esContext->mouseButtonFunc)
		{
			esContext->mouseButtonFunc(esContext, (int)point.x - WDpoint.x, (int)point.y - WDpoint.y, MOUSE_RIGHT_BUTTON, false);
		}
	}
	break;

	case WM_LBUTTONDOWN:
	{
		ESContext* esContext = (ESContext*)(LONG_PTR)GetWindowLongPtr(hWnd, GWL_USERDATA);
		POINT point;
		GetCursorPos(&point);

		if (esContext && esContext->mouseButtonFunc)
		{
			esContext->mouseButtonFunc(esContext, (int)point.x - WDpoint.x, (int)point.y - WDpoint.y, MOUSE_LEFT_BUTTON, true);
		}
	}
	break;

	case WM_RBUTTONDOWN:
	{
		ESContext* esContext = (ESContext*)(LONG_PTR)GetWindowLongPtr(hWnd, GWL_USERDATA);
		POINT point;
		GetCursorPos(&point);

		if (esContext && esContext->mouseButtonFunc)
		{
			esContext->mouseButtonFunc(esContext, (int)point.x - WDpoint.x, (int)point.y - WDpoint.y, MOUSE_RIGHT_BUTTON, true);
		}
	}
	break;

	case WM_XBUTTONDOWN:
	{
		ESContext* esContext = (ESContext*)(LONG_PTR)GetWindowLongPtr(hWnd, GWL_USERDATA);
		POINT point;
		GetCursorPos(&point);
		WORD btn = GET_XBUTTON_WPARAM(wParam);

		if (esContext && esContext->mouseButtonFunc)
		{
			esContext->mouseButtonFunc(esContext, (int)point.x - WDpoint.x, (int)point.y - WDpoint.y, btn == XBUTTON1 ? MOUSE_BACK_BUTTON : MOUSE_FORWARD_BUTTON, true);
		}
	}
	break;

	case WM_XBUTTONUP:
	{
		ESContext* esContext = (ESContext*)(LONG_PTR)GetWindowLongPtr(hWnd, GWL_USERDATA);
		POINT point;
		GetCursorPos(&point);
		WORD btn = GET_XBUTTON_WPARAM(wParam);

		if (esContext && esContext->mouseButtonFunc)
		{
			esContext->mouseButtonFunc(esContext, (int)point.x - WDpoint.x, (int)point.y - WDpoint.y, btn == XBUTTON1 ? MOUSE_BACK_BUTTON : MOUSE_FORWARD_BUTTON, false);
		}
	}
	break;

	case WM_MOUSEWHEEL:
	{
		ESContext* esContext = (ESContext*)(LONG_PTR)GetWindowLongPtr(hWnd, GWL_USERDATA);
		short delta = GET_WHEEL_DELTA_WPARAM(wParam);
		POINT point;
		GetCursorPos(&point);

		if (esContext && esContext->mouseScrollFunc)
		{
			esContext->mouseScrollFunc(esContext, (int)point.x - WDpoint.x, (int)point.y - WDpoint.y, delta);
		}
	}
	break;

	case WM_MOUSEMOVE:
	{
		ESContext* esContext = (ESContext*)(LONG_PTR)GetWindowLongPtr(hWnd, GWL_USERDATA);
		POINT point;
		GetCursorPos(&point);

		if (esContext && esContext->mouseMoveFunc)
		{
			esContext->mouseMoveFunc(esContext, (int)point.x - WDpoint.x, (int)point.y - WDpoint.y);
		}
	}
	break;

	case WM_MOVE:
	{
		WDpoint.x = (int)LOWORD(lParam);
		WDpoint.y = (int)HIWORD(lParam);
	}
	break;

	default:
		lRet = DefWindowProc(hWnd, uMsg, wParam, lParam);
		break;
	}

	return lRet;
}

//      Create Win32 instance and window
GLboolean WinCreate(ESContext* esContext, const char* title)
{
	WNDCLASS wndclass = { 0 };
	DWORD    wStyle = 0;
	RECT     windowRect;
	HINSTANCE hInstance = GetModuleHandle(NULL);


	wndclass.style = CS_OWNDC;
	wndclass.lpfnWndProc = (WNDPROC)ESWindowProc;
	wndclass.hInstance = hInstance;
	wndclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wndclass.lpszClassName = "opengles2.0";

	if (!RegisterClass(&wndclass))
		return FALSE;

	wStyle = WS_VISIBLE | WS_POPUP | WS_BORDER | WS_SYSMENU | WS_CAPTION;

	// Adjust the window rectangle so that the client area has
	// the correct number of pixels
	windowRect.left = 0;
	windowRect.top = 0;
	windowRect.right = esContext->width;
	windowRect.bottom = esContext->height;

	AdjustWindowRect(&windowRect, wStyle, FALSE);

	esContext->hWnd = CreateWindow(
		"opengles2.0",
		title,
		wStyle,
		0,
		0,
		windowRect.right - windowRect.left,
		windowRect.bottom - windowRect.top,
		NULL,
		NULL,
		hInstance,
		NULL);

	// Set the ESContext* to the GWL_USERDATA so that it is available to the 
	// ESWindowProc
	SetWindowLongPtr(esContext->hWnd, GWL_USERDATA, (LONG)(LONG_PTR)esContext);

	if (esContext->hWnd == NULL)
		return GL_FALSE;

	ShowWindow(esContext->hWnd, TRUE);

	return GL_TRUE;
}



//      Start main windows loop
void WinLoop(ESContext* esContext)
{
	MSG msg = { 0 };
	bool done = false;
	esContext->running = true;
	std::chrono::steady_clock::time_point lastTime, curTime;
	lastTime = curTime = std::chrono::steady_clock::now();
	std::chrono::duration<float> duration;
	float deltaTime;

	while (!done && esContext->running)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
			{
				done = true;
			}
			else
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else
		{
			curTime = std::chrono::steady_clock::now();
			duration = curTime - lastTime;
			deltaTime = duration.count();
			lastTime = curTime;
			esContext->updateFunc(esContext, deltaTime);
			SendMessage(esContext->hWnd, WM_PAINT, 0, 0);
		}
	}
}
