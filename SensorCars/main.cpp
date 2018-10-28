#include "scene.h"
#include <chrono>

#define ID_BTN_CONNECT 10
#define ID_BTN_RESTART 12
#define ID_BTN_SEND 13
#define ID_TIMER1 100

std::unique_ptr<car::Scene> g_scene;
std::unique_ptr<com::Communication> g_connection;

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	static HWND textBox_IP;
	static HWND textBox_port;
	static HWND label_status;
	static HWND button_conn;
	static HWND textBox_inMsg;
	static HWND textBox_outMsg;
	std::vector<char> data;
	RECT rect;
	WCHAR ipBuffer[16];
	WCHAR portBuffer[16];
	WCHAR msgBuffer[128];

	switch (msg)
	{
	case WM_CREATE:
		GetClientRect(hwnd, &rect);

		CreateWindow(L"static", L"IP:", WS_CHILD | WS_VISIBLE, 10, 10, 50, 22, hwnd, NULL, GetModuleHandle(NULL), NULL);
		textBox_IP = CreateWindow(L"edit", L"127.0.0.1", WS_CHILD | WS_VISIBLE | WS_BORDER, 70, 10, 120, 22, hwnd, NULL, GetModuleHandle(NULL), NULL);
		CreateWindow(L"static", L"port:", WS_CHILD | WS_VISIBLE, 10, 40, 50, 22, hwnd, NULL, GetModuleHandle(NULL), NULL);
		textBox_port = CreateWindow(L"edit", L"213", WS_CHILD | WS_VISIBLE | WS_BORDER, 70, 40, 120, 22, hwnd, NULL, GetModuleHandle(NULL), NULL);
		label_status = CreateWindow(L"static", L"Offline", WS_CHILD | WS_VISIBLE, 10, 70, 50, 22, hwnd, NULL, GetModuleHandle(NULL), NULL);
		button_conn = CreateWindow(L"button", L"Connect", WS_CHILD | WS_VISIBLE | WS_BORDER, 70, 70, 120, 22, hwnd, (HMENU)ID_BTN_CONNECT, GetModuleHandle(NULL), NULL);

		CreateWindow(L"static", L"Incoming", WS_CHILD | WS_VISIBLE, 10, 100, 180, 22, hwnd, NULL, GetModuleHandle(NULL), NULL);
		textBox_inMsg = CreateWindow(L"edit", L"", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_READONLY | ES_MULTILINE, 10, 124, 180, 60, hwnd, NULL, GetModuleHandle(NULL), NULL);
		CreateWindow(L"static", L"Outgoing", WS_CHILD | WS_VISIBLE, 10, 190, 180, 22, hwnd, NULL, GetModuleHandle(NULL), NULL);
		textBox_outMsg = CreateWindow(L"edit", L"", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE, 10, 214, 180, 60, hwnd, NULL, GetModuleHandle(NULL), NULL);
		CreateWindow(L"button", L"Send", WS_CHILD | WS_VISIBLE | WS_BORDER, 10, 276, 180, 30, hwnd, (HMENU)ID_BTN_SEND, GetModuleHandle(NULL), NULL);

		CreateWindow(L"button", L"Restart cars", WS_CHILD | WS_VISIBLE | WS_BORDER, 10, rect.bottom - 300, 180, 40, hwnd, (HMENU)ID_BTN_RESTART, GetModuleHandle(NULL), NULL);

		return 0;
	case WM_TIMER:
		if (g_connection && g_connection->FetchRecvData(data))
		{
			std::wstring str;
			for (int i = 4; i < data.size() / 2; i++)
			{
				WCHAR ch = data[2 * i + 1] + data[2 * i] * 0x100;
				str += ch;
			}
			SetWindowText(textBox_inMsg, str.c_str());
		}
		return 0;
	case WM_COMMAND:
		switch (wparam)
		{
		case ID_BTN_CONNECT:
			if (g_connection)
			{
				g_connection.reset();
				KillTimer(hwnd, ID_TIMER1);
				SetWindowText(label_status, L"Offline");
				SetWindowText(button_conn, L"Connect");
			}
			else
			{
				GetWindowText(textBox_IP, ipBuffer, 16);
				GetWindowText(textBox_port, portBuffer, 16);
				try
				{
					g_connection = std::unique_ptr<com::Communication>(new com::Communication(ipBuffer, portBuffer));
					SetTimer(hwnd, ID_TIMER1, 100, NULL);
					SetWindowText(label_status, L"Online");
					SetWindowText(button_conn, L"Disconnect");
				}
				catch (std::exception& e)
				{
					auto em = e.what();
					std::wstring msg;
					for (size_t i = 0; em[i]; i++)
						msg += em[i];
					MessageBox(NULL, msg.c_str(), L"Error", MB_OK | MB_ICONERROR);
				};
			}
			break;
		case ID_BTN_RESTART:
			g_scene->Restart();
			break;
		case ID_BTN_SEND:
			if (g_connection)
			{
				GetWindowText(textBox_outMsg, msgBuffer, 128);
				g_connection->Send(msgBuffer);
			}
			break;
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