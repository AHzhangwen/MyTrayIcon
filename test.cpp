#include <windows.h>
#include <shellapi.h>
#include <tchar.h>
#include "resource.h"
#include <iostream>
#include <chrono>
#include <thread>
using namespace std::chrono_literals;

// Define the menu item IDs
#define ID_EXIT 1
#define ID_CONSOLE 2
#define ID_RESTART 3

// Handle to the window
HWND hWnd;
HWND hwndConsole;

// Tray icon data structure
NOTIFYICONDATA trayIconData;

// Handle to the icon
HICON hIcon;

void changeConsoleStatus(HWND h)
{
	if (IsWindowVisible(h))
	{
		ShowWindow(h, SW_HIDE);
	}
	else
	{
		ShowWindow(h, SW_SHOW);
	}
}

// Tray icon message handler
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE:
		// Load the icon
		hIcon = (HICON)LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON1), IMAGE_ICON, 16, 16, LR_DEFAULTCOLOR);
		// Set up the tray icon data
		trayIconData.cbSize = sizeof(NOTIFYICONDATA);
		trayIconData.hWnd = hWnd;
		trayIconData.uID = IDI_ICON1;
		trayIconData.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
		trayIconData.hIcon = hIcon;
		trayIconData.uCallbackMessage = WM_APP;
		_tcscpy_s(trayIconData.szTip, _T("My Tray Icon"));

		// Add the tray icon
		Shell_NotifyIcon(NIM_ADD, &trayIconData);
		break;

	case WM_APP:
		// Handle tray icon events
		switch (lParam)
		{
		case WM_RBUTTONUP:
			// Show the context menu
			POINT pt;
			GetCursorPos(&pt);
			HMENU hMenu = CreatePopupMenu();
			InsertMenu(hMenu, 0, MF_BYPOSITION | MF_STRING, ID_EXIT, _T("Exit"));
			InsertMenu(hMenu, 1, MF_BYPOSITION | MF_STRING, ID_CONSOLE, _T("Console"));
			InsertMenu(hMenu, 2, MF_BYPOSITION | MF_STRING, ID_RESTART, _T("Restart"));
			SetForegroundWindow(hWnd);
			TrackPopupMenu(hMenu, TPM_RIGHTALIGN | TPM_BOTTOMALIGN, pt.x, pt.y, 0, hWnd, NULL);
			DestroyMenu(hMenu);
			break;
		}
		break;

	case WM_COMMAND:
		// Handle menu item selections
		switch (LOWORD(wParam))
		{
		case ID_CONSOLE:
			changeConsoleStatus(hwndConsole);
			break;
		case ID_RESTART:
		{
			TCHAR szModule[MAX_PATH];
			GetModuleFileName(NULL, szModule, MAX_PATH);

			SHELLEXECUTEINFO sei = { 0 };
			sei.cbSize = sizeof(SHELLEXECUTEINFO);
			sei.lpFile = szModule;
			sei.nShow = SW_SHOW;
			ShellExecuteEx(&sei);
			DestroyWindow(hWnd);
		}
		break;

		case ID_EXIT:
			// Exit the application
			Shell_NotifyIcon(NIM_DELETE, &trayIconData);
			DestroyWindow(hWnd);
			PostQuitMessage(0);
			break;
		}
		break;

	case WM_DESTROY:
		// Clean up
		Shell_NotifyIcon(NIM_DELETE, &trayIconData);
		DestroyIcon(hIcon);
		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}

void MyMain();
int WINAPI main(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	hwndConsole = GetConsoleWindow();	//获取程序启动时的窗口
	if (hwndConsole) {
		ShowWindow(hwndConsole, SW_HIDE);	//设置指定窗口的显示状态
	}


	WNDCLASS wndClass = { 0 };
	wndClass.lpfnWndProc = WndProc;
	wndClass.hInstance = hInstance;
	wndClass.lpszClassName = _T("MyTrayIcon");
	RegisterClass(&wndClass);
	hWnd = CreateWindow(wndClass.lpszClassName, _T("My Tray Icon"), WS_OVERLAPPEDWINDOW, 0, 0, 0, 0, 0, NULL, NULL, hInstance, NULL);
	// Show the window
	ShowWindow(hWnd, nCmdShow);

	std::thread(MyMain).detach();

	// Start the message loop
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return (int)msg.wParam;
}



void MyMain()
{
	int i = 0;
	while (1)
	{
		printf(" %d\a", i++);
		std::this_thread::sleep_for(1000ms);
	}
}
