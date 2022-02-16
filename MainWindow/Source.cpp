#define _CRT_SECURE_NO_WARNINGS
#include<Windows.h>
#include<windowsx.h>
#include<stdio.h>
#include<CommCtrl.h>
#include"resource.h"

//Tracking tooltip:

#pragma comment(lib,"comctl32.lib")
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

TOOLINFO g_toolItem = { 0 };
HWND g_hwndTrackingTT;
BOOL g_TrackingMouse = FALSE;
TRACKMOUSEEVENT tme = { sizeof(TRACKMOUSEEVENT) };

CONST CHAR g_szClassName[] = "MyWindowClass";
CONST CHAR g_szTitle[] = "MyWindowClass";


LRESULT CALLBACK WndProc(HWND hwnd, UINT nMsg, WPARAM wParam, LPARAM lParam);
VOID CreateToolTipFprRect(HWND hwnd);
HWND CreateTrackingTooltip(HWND hwnd);

INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInst, LPSTR lpCmdLine, INT nCmdShow)
{
	//1) Регистрация класса окна:
	WNDCLASSEX wc;
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.lpfnWndProc = WndProc;
	wc.style = 0;
	//wc.hIcon = LoadIcon(hInstance,MAKEINTRESOURCE( IDI_ICON_MISFITS));
	//wc.hIconSm = LoadIcon(hInstance,MAKEINTRESOURCE( IDI_ICON_STAR));
	wc.hIcon = (HICON)LoadImage(hInstance, "Star_Wars.ico", IMAGE_ICON, 0, 0, LR_DEFAULTSIZE | LR_LOADFROMFILE);
	wc.hIconSm = (HICON)LoadImage(hInstance, "moon.ico", IMAGE_ICON, 0, 0, LR_DEFAULTSIZE | LR_LOADFROMFILE);
	wc.hCursor = LoadCursor(hInstance, MAKEINTRESOURCE(IDC_CURSOR1));
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wc.hInstance = hInstance;
	wc.lpszMenuName = NULL;
	wc.lpszClassName = g_szClassName;
	if (!RegisterClassEx(&wc))
	{
		MessageBox(NULL, "Class registration failed", "Error", MB_OK | MB_ICONERROR);
		return 0;
	}

	//2)Создание окна:
	INT screen_width = GetSystemMetrics(SM_CXSCREEN);
	INT screen_height = GetSystemMetrics(SM_CYSCREEN);
	INT window_width = screen_width - screen_width / 2;
	INT window_height = screen_height - screen_height / 2;
	INT window_start_x = screen_width / 4;
	INT window_start_y = screen_height / 4;
	HWND hwnd = CreateWindowEx
	(
		WS_EX_CLIENTEDGE,
		g_szClassName,                  //Имя класса окна
		//g_szClassName,                //Заголовок окна
		g_szTitle,                      //Заголовок окна
		WS_OVERLAPPEDWINDOW,            //Для главного окна всегда задается такой стиль
		window_start_x, window_start_y, //Начальная позиция окна на экране
		window_width, window_height,    //Исходный размер окна
		NULL,                           //Родительское окно отсутствует
		NULL,                           //Меню тоже отсутствует
		hInstance,                      //EXE-файд окна
		NULL
	);
	if (hwnd == NULL)
	{
		MessageBox(NULL, "Window creation failed", "Error", MB_OK | MB_ICONERROR);
		return 0;
	}
	ShowWindow(hwnd, nCmdShow);//Задаем режим окна (развернуто на весь экран, свернуто на панель задач и.т.д
	UpdateWindow(hwnd);//Прорисовка окна

	//3)Запуск цикла сообщений:
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0) > 0)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_CREATE:
	{
		//Tooltip - всплывающая подсказка:
		InitCommonControls();
		//CreateToolTipFprRect(hwnd);
		g_hwndTrackingTT = CreateTrackingTooltip(hwnd);
	}
	break;
	case WM_MOUSELEAVE:
	{
		SendMessage(g_hwndTrackingTT, TTM_TRACKACTIVATE, (WPARAM)FALSE, (LPARAM)&g_toolItem);
		g_TrackingMouse = FALSE;
		return FALSE;
	}
		break;
	case WM_MOUSEMOVE:
	{
		//CreateToolTipFprRect(hwnd);
		//SendMessage(hwndTT, TTM_ADDTOOL, 0, (LPARAM)(LPTOOLINFO)&ti);
		static int oldX, oldY;
		int newX, newY;
		if (!g_TrackingMouse)
		{
			tme.cbSize = sizeof(TRACKMOUSEEVENT);
			tme.dwFlags = TME_LEAVE;
			tme.hwndTrack = hwnd;

			TrackMouseEvent(&tme);
			SendMessage(g_hwndTrackingTT, TTM_TRACKACTIVATE, (WPARAM)TRUE, (LPARAM)&g_toolItem);
			g_TrackingMouse = TRUE;
		}
		newX = GET_X_LPARAM(lParam);
		newY = GET_Y_LPARAM(lParam);

		if (newX != oldX || newY != oldY)
		{
			oldX = newX;
			oldY = newY;

			CHAR coords[20]{};
			sprintf(coords, "%dx%d", newX, newY);
			g_toolItem.lpszText = coords;
			SendMessage(g_hwndTrackingTT, TTM_SETTOOLINFO, 0, (LPARAM)&g_toolItem);

			POINT pt = { newX, newY };
			ClientToScreen(hwnd, &pt);
			SendMessage(g_hwndTrackingTT, TTM_TRACKPOSITION, 0, (LPARAM)MAKEWORD(pt.x, pt.y));
		}
		SendMessage(g_hwndTrackingTT, TTM_TRACKACTIVATE, (WPARAM)TRUE, (LPARAM)&g_toolItem);
		return FALSE;
	}
	break;
	case WM_COMMAND:
		break;
	case WM_SIZE:
	case WM_MOVE:
	{
		CONST INT SIZE = 256;
		CHAR buffer[SIZE] = {};
		RECT rect;  //Прямоугольник
		GetWindowRect(hwnd, &rect);
		int window_start_x = rect.left;
		int window_start_y = rect.top;
		int window_width = rect.right - rect.left;
		int window_height = rect.bottom - rect.top;
		sprintf(buffer, "%s - pisotion: %dx%d, size: %dx%d", g_szTitle, window_start_x, window_start_y, window_width, window_height);
		SendMessage(hwnd, WM_SETTEXT, 0, (LPARAM)buffer);
	}
	break;
	case WM_CLOSE:
		if (MessageBox(hwnd, "Вы действительно хотите выйти?", "Question", MB_YESNO | MB_ICONQUESTION) == IDYES)
			DestroyWindow(hwnd);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default: return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
	return 0;
}

VOID CreateToolTipFprRect(HWND hwnd)
{
	HWND hwndTT = CreateWindowEx
	(
		WS_EX_TOPMOST | WS_EX_TOOLWINDOW,
		TOOLTIPS_CLASS,
		NULL, //"My first Tooltip",
		WS_POPUP | TTS_NOPREFIX | TTS_ALWAYSTIP,  //WS_POPUP      - всплывающее окно 
											  //TTS_NOPREFIX  - игнорировать символ &
											  //TTS_ALWAYSTIP - подсказка появляется даже над неактивным окн
		CW_USEDEFAULT, CW_USEDEFAULT,
		CW_USEDEFAULT, CW_USEDEFAULT,
		hwnd,
		NULL, //(HMENU)IDC_TOOLTIP,
		GetModuleHandle(NULL),//Получаем HINSTANCE (EXE-файл) программы
		NULL
	);
	if (hwndTT == NULL)
	{
		MessageBox(hwnd, "Tooltop window creation failed", "Error", MB_OK | MB_ICONERROR);
		return;
	}

	SetWindowPos(hwndTT, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);

	TOOLINFO ti;
	ZeroMemory(&ti, sizeof(TOOLINFO));
	ti.cbSize = sizeof(TOOLINFO);
	ti.uFlags = TTF_SUBCLASS;
	ti.hwnd = hwnd;
	ti.hinst = GetModuleHandle(NULL);
	ti.lpszText = (LPSTR)"This is a tooltip";
	GetClientRect(hwnd, &ti.rect);
	SendMessage(hwndTT, TTM_ADDTOOL, 0, (LPARAM)(LPTOOLINFO)&ti);
}
HWND CreateTrackingTooltip(HWND hwnd)
{
	HWND hwndTT = CreateWindowEx
	(
		WS_EX_TOPMOST,
		TOOLTIPS_CLASS,
		NULL,
		WS_POPUP | TTS_NOPREFIX | TTS_ALWAYSTIP,
		CW_USEDEFAULT, CW_USEDEFAULT,
		CW_USEDEFAULT, CW_USEDEFAULT,
		hwnd,
		NULL,
		GetModuleHandle(NULL),
		NULL
	);
	if (hwndTT == NULL)
	{
		MessageBox(NULL, "Tooltip Window creation failed", "Error", MB_OK | MB_ICONERROR);
		return NULL;
	}
	g_toolItem.cbSize = sizeof(TOOLINFO);
	g_toolItem.uFlags = TTF_IDISHWND | TTF_TRACK | TTF_ABSOLUTE;
	g_toolItem.hwnd = hwnd;
	g_toolItem.hinst = GetModuleHandle(NULL);
	g_toolItem.lpszText = (LPSTR)"Tracking tooltip";
	g_toolItem.uId = (UINT_PTR)hwnd;
	GetClientRect(hwnd, &g_toolItem.rect);
	SendMessage(g_hwndTrackingTT, TTM_ADDTOOL, 0, (LPARAM)(LPTOOLINFO)&g_toolItem);
}