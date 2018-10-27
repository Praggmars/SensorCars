#include "scene.h"
#include <chrono>

#define ID_BTN_CONNECT 10
#define ID_BTN_DISCONNECT 11
#define ID_BTN_RESTART 12
#define ID_BTN_SEND 13

std::unique_ptr<car::Scene> g_scene;

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	static HWND textBox_IP;
	static HWND textBox_port;
	static HWND textBox_message;
	RECT rect;
	WCHAR ipBuffer[16];
	WCHAR portBuffer[16];
	WCHAR msgBuffer[128];

	switch (msg)
	{
	case WM_CREATE:
		GetClientRect(hwnd, &rect);
		textBox_IP = CreateWindow(L"edit", L"127.0.0.1", WS_CHILD | WS_VISIBLE | WS_BORDER, rect.right - 190, 20, 180, 30, hwnd, NULL, GetModuleHandle(NULL), NULL);
		textBox_port = CreateWindow(L"edit", L"1111", WS_CHILD | WS_VISIBLE | WS_BORDER, rect.right - 190, 60, 180, 30, hwnd, NULL, GetModuleHandle(NULL), NULL);
		CreateWindow(L"button", L"Connect", WS_CHILD | WS_VISIBLE | WS_BORDER, rect.right - 190, 100, 180, 40, hwnd, (HMENU)ID_BTN_CONNECT, GetModuleHandle(NULL), NULL);
		CreateWindow(L"button", L"Disonnect", WS_CHILD | WS_VISIBLE | WS_BORDER, rect.right - 190, 150, 180, 40, hwnd, (HMENU)ID_BTN_DISCONNECT, GetModuleHandle(NULL), NULL);
		textBox_message = CreateWindow(L"edit", L"", WS_CHILD | WS_VISIBLE | WS_BORDER, rect.right - 190, 200, 180, 30, hwnd, NULL, GetModuleHandle(NULL), NULL);
		CreateWindow(L"button", L"Send", WS_CHILD | WS_VISIBLE | WS_BORDER, rect.right - 190, 240, 180, 30, hwnd, (HMENU)ID_BTN_SEND, GetModuleHandle(NULL), NULL);
		CreateWindow(L"button", L"Restart cars", WS_CHILD | WS_VISIBLE | WS_BORDER, rect.right - 190, rect.bottom - 300, 180, 40, hwnd, (HMENU)ID_BTN_RESTART, GetModuleHandle(NULL), NULL);
		return 0;
	case WM_COMMAND:
		switch (wparam)
		{
		case ID_BTN_CONNECT:
			GetWindowText(textBox_IP, ipBuffer, 16);
			GetWindowText(textBox_port, portBuffer, 16);
			g_scene->StartConnection(ipBuffer, portBuffer);
			break;
		case ID_BTN_DISCONNECT:
			g_scene->EndConnection();
			break;
		case ID_BTN_RESTART:
			g_scene->Restart();
			break;
		case ID_BTN_SEND:
			if (g_scene->getConnection())
			{
				GetWindowText(textBox_message, msgBuffer, 128);
				g_scene->getConnection()->Send(msgBuffer);
			}
			break;
		}
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

	HWND appWindow = CreateAppWindow(hInstance, iCmdShow, width, height);
	g_scene = std::unique_ptr<car::Scene>(new car::Scene);
	if (!g_scene->Init(appWindow, width, height))
		return 0;

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