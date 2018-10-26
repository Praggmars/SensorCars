#include "scene.h"
#include <chrono>

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	switch (msg)
	{
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
	car::Scene scene;
	if (!scene.Init(appWindow, width, height))
		return 0;

	MSG msg{};
	auto prevTime = std::chrono::steady_clock::now();
	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			scene.MessageHandler(msg);
		}
		else
		{
			auto currentTime = std::chrono::steady_clock::now();
			scene.Frame(std::chrono::duration<float>(currentTime - prevTime).count());
			prevTime = currentTime;
		}
	}
	return (INT)msg.wParam;
}