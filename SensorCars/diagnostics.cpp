#include "diagnostics.h"

#define ID_BTN_CONNECT 20
#define ID_BTN_SEND 21
#define ID_TIMER1 100

namespace diag
{
	void Diagnostics::SendCarDiagData()
	{
		com::CarDiagnosticData data;
		data.carID = -1;	//usercar
		for (int i = 0; i < 6; i++)
			data.lightSensorSignal[i] = m_target->getLightSensor(i).getMeasurement();
		for (int i = 0; i < 2; i++)
			data.distanceSensorSignal[i] = m_target->getDistanceSensor(i).getMeasurement();
		data.collidedWith = 0;
	}
	void Diagnostics::ActReceivedCommand(com::CommandType cmd)
	{
		switch (cmd)
		{
		case com::CommandType::AUTOMODE:
			break;
		}
	}
	Diagnostics::Diagnostics() :
		m_textBox_IP(nullptr),
		m_textBox_port(nullptr),
		m_label_status(nullptr),
		m_button_conn(nullptr),
		m_textBox_inMsg(nullptr),
		m_textBox_outMsg(nullptr),
		m_target(nullptr),
		m_scene(nullptr) {}
	void Diagnostics::StartDiagnostic(LPCWSTR ip, LPCWSTR port)
	{
		m_connection = std::unique_ptr<com::Communication>(new com::Communication(ip, port));
		SetTimer(m_window, ID_TIMER1, 100, NULL);
		SetWindowText(m_label_status, L"Online");
		SetWindowText(m_button_conn, L"Disconnect");
	}
	void Diagnostics::EndDiagnostics()
	{
		m_target = nullptr;
		KillTimer(m_window, ID_TIMER1);
		SetWindowText(m_label_status, L"Offline");
		SetWindowText(m_button_conn, L"Connect");
		m_connection.reset();
	}
	void Diagnostics::CreateGUI(HWND hwnd)
	{
		m_window = hwnd;
		RECT rect;
		GetClientRect(hwnd, &rect);

		CreateWindow(L"static", L"IP:", WS_CHILD | WS_VISIBLE, 10, 10, 50, 22, hwnd, NULL, GetModuleHandle(NULL), NULL);
		m_textBox_IP = CreateWindow(L"edit", L"127.0.0.1", WS_CHILD | WS_VISIBLE | WS_BORDER, 70, 10, 120, 22, hwnd, NULL, GetModuleHandle(NULL), NULL);
		CreateWindow(L"static", L"port:", WS_CHILD | WS_VISIBLE, 10, 40, 50, 22, hwnd, NULL, GetModuleHandle(NULL), NULL);
		m_textBox_port = CreateWindow(L"edit", L"213", WS_CHILD | WS_VISIBLE | WS_BORDER, 70, 40, 120, 22, hwnd, NULL, GetModuleHandle(NULL), NULL);
		m_label_status = CreateWindow(L"static", L"Offline", WS_CHILD | WS_VISIBLE, 10, 70, 50, 22, hwnd, NULL, GetModuleHandle(NULL), NULL);
		m_button_conn = CreateWindow(L"button", L"Connect", WS_CHILD | WS_VISIBLE | WS_BORDER, 70, 70, 120, 22, hwnd, (HMENU)ID_BTN_CONNECT, GetModuleHandle(NULL), NULL);

		CreateWindow(L"static", L"Incoming", WS_CHILD | WS_VISIBLE, 10, 100, 180, 22, hwnd, NULL, GetModuleHandle(NULL), NULL);
		m_textBox_inMsg = CreateWindow(L"edit", L"", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_READONLY | ES_MULTILINE, 10, 124, 180, 60, hwnd, NULL, GetModuleHandle(NULL), NULL);
		CreateWindow(L"static", L"Outgoing", WS_CHILD | WS_VISIBLE, 10, 190, 180, 22, hwnd, NULL, GetModuleHandle(NULL), NULL);
		m_textBox_outMsg = CreateWindow(L"edit", L"", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE, 10, 214, 180, 60, hwnd, NULL, GetModuleHandle(NULL), NULL);
		CreateWindow(L"button", L"Send", WS_CHILD | WS_VISIBLE | WS_BORDER, 10, 276, 180, 30, hwnd, (HMENU)ID_BTN_SEND, GetModuleHandle(NULL), NULL);
	}
	void Diagnostics::UpdateGUI()
	{
		if (m_connection->isOnline())
		{
			std::vector<char> data;
			if (m_connection->FetchRecvData(data))
			{
				std::wstring str;
				int msgType = ((data[4] * 0x100 + data[5]) * 0x100 + data[6]) * 0x100 + data[7];
				if (msgType == (int)com::MessageType::STRING)
				{
					for (int i = 6; i < (int)data.size() / 2; i++)
					{
						WCHAR ch = data[2 * i + 1] + data[2 * i] * 0x100;
						str += ch;
					}
					SetWindowText(m_textBox_inMsg, str.c_str());
				}
				if (msgType == (int)com::MessageType::COMMAND)
				{
					int cmd = ((data[8] * 0x100 + data[9]) * 0x100 + data[10]) * 0x100 + data[11];
					switch (cmd)
					{
					case (int)com::CommandType::FORWARD:
						m_scene->m_forward = true;
						m_scene->m_back = false;
						break;
					case (int)com::CommandType::BACKWARD:
						m_scene->m_forward = false;
						m_scene->m_back = true;
						break;
					case (int)com::CommandType::NOMOVE:
						m_scene->m_forward = false;
						m_scene->m_back = false;
						break;
					case (int)com::CommandType::LEFT:
						m_scene->m_left = true;
						m_scene->m_right = false;
						break;
					case (int)com::CommandType::RIGHT:
						m_scene->m_left = false;
						m_scene->m_right = true;
						break;
					case (int)com::CommandType::NOSTEERING:
						m_scene->m_left = false;
						m_scene->m_right = false;
						break;
					case (int)com::CommandType::RESET:
						m_scene->Restart();
						break;
					}
				}
			}
		}
		else
			EndDiagnostics();
	}
	void Diagnostics::CommandAction(HWND hwnd, UINT btnID)
	{
		switch (btnID)
		{
		case ID_BTN_CONNECT:
			if (m_connection && m_connection->isOnline())
			{
				EndDiagnostics();
			}
			else
			{
				WCHAR ipBuffer[16];
				WCHAR portBuffer[16];
				GetWindowText(m_textBox_IP, ipBuffer, 16);
				GetWindowText(m_textBox_port, portBuffer, 16);
				try
				{
					StartDiagnostic(ipBuffer, portBuffer);
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
		case ID_BTN_SEND:
			if (m_connection && m_connection->isOnline())
			{
				WCHAR msgBuffer[128];
				GetWindowText(m_textBox_outMsg, msgBuffer, 128);
				m_connection->Send(msgBuffer);
			}
			break;
		}
	}
}
