#include "scene.h"
#include "diagnostics.h"
#include <chrono>

#define ID_BTN_RESTART 10
#define ID_BTN_AUTOMODE 11
#define ID_BTN_SELFTEST 12

HWND g_appWindow;
std::unique_ptr<car::Scene> g_scene;
diag::Diagnostics g_diagnostics;

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	RECT rect;

	switch (msg)
	{
	case WM_CREATE:
		GetClientRect(hwnd, &rect);
		g_diagnostics.CreateGUI(hwnd);
		CreateWindow(L"button", L"Restart cars", WS_CHILD | WS_VISIBLE | WS_BORDER, 10, rect.bottom - 300, 90, 40, hwnd, (HMENU)ID_BTN_RESTART, GetModuleHandle(NULL), NULL);
		CreateWindow(L"button", L"Auto mode", WS_CHILD | WS_VISIBLE | WS_BORDER, 110, rect.bottom - 300, 80, 40, hwnd, (HMENU)ID_BTN_AUTOMODE, GetModuleHandle(NULL), NULL);
		CreateWindow(L"button", L"Self test", WS_CHILD | WS_VISIBLE | WS_BORDER, 10, rect.bottom - 340, 180, 30, hwnd, (HMENU)ID_BTN_SELFTEST, GetModuleHandle(NULL), NULL);
		return 0;
	case WM_RECVMSG:
		g_diagnostics.MessageReceived((std::vector<char>*)lparam);
		return 0;
	case WM_COMMAND:
		switch (wparam)
		{
		case ID_BTN_RESTART:
			g_scene->Restart();
			break;
		case ID_BTN_AUTOMODE:
			g_scene->SwitchCarPilotAutoManual();
			break;
		case ID_BTN_SELFTEST:
			g_scene->StartSelfTest();
			break;
		default:
			g_diagnostics.CommandAction(hwnd, (UINT)wparam);
		}
		return 0;
	case WM_SIZE:
		GetClientRect(hwnd, &rect);
		rect.left += 200;
		if (rect.left < rect.right&&rect.top < rect.bottom)
		{
			g_scene.reset();
			g_scene = std::unique_ptr<car::Scene>(new car::Scene);
			if (!g_scene->Init(hwnd, rect))
				PostQuitMessage(0);
		}
		if (g_scene)
			g_diagnostics.setScene(g_scene.get());
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, msg, wparam, lparam);
}

HWND CreateAppWindow(HINSTANCE hInstance, INT iCmdShow, int width, int height)
{
	WNDCLASSEX wc{};
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = WndProc;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hIconSm = wc.hIcon;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.lpszClassName = L"SensorCars";
	wc.cbSize = sizeof(WNDCLASSEX);
	RegisterClassEx(&wc);

	RECT rect{};
	rect.right = width;
	rect.bottom = height;
	AdjustWindowRectEx(&rect, WS_OVERLAPPEDWINDOW, FALSE, WS_EX_OVERLAPPEDWINDOW);
	HWND appWindow = CreateWindowEx(WS_EX_OVERLAPPEDWINDOW, L"SensorCars", L"SensorCars", WS_OVERLAPPEDWINDOW,
		(GetSystemMetrics(SM_CXSCREEN) - (rect.right - rect.left)) / 2, (GetSystemMetrics(SM_CYSCREEN) - (rect.bottom - rect.top)) / 2,
		rect.right - rect.left, rect.bottom - rect.top, nullptr, nullptr, hInstance, nullptr);
	ShowWindow(appWindow, iCmdShow);
	UpdateWindow(appWindow);

	return appWindow;
}

INT WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR szCmdLine, INT iCmdShow)
{
	int width = 1280;
	int height = 720;

	g_appWindow = CreateAppWindow(hInstance, iCmdShow, width, height);
	/*RECT rect;
	g_scene = std::unique_ptr<car::Scene>(new car::Scene);
	GetClientRect(appWindow, &rect);
	rect.left += 200;
	if (!g_scene->Init(appWindow, rect))
		return 0;*/

	MSG msg{};
	auto prevTime = std::chrono::steady_clock::now();
	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			g_scene->MessageHandler(msg);
		}
		else
		{
			auto currentTime = std::chrono::steady_clock::now();
			g_scene->Frame(std::chrono::duration<float>(currentTime - prevTime).count());
			prevTime = currentTime;
		}
	}
	g_scene.release();
	return (INT)msg.wParam;
}