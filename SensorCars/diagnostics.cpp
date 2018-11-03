#include "diagnostics.h"

#define ID_BTN_CONNECT 20
#define ID_BTN_SEND 21

namespace diag
{
	void Diagnostics::DisconnectFromServer()
	{
		EndDiagnostics();
	}
	void Diagnostics::ConnectToServer()
	{
		try
		{
			WCHAR ipBuffer[16];
			WCHAR portBuffer[16];
			GetWindowText(m_textBox_IP, ipBuffer, 16);
			GetWindowText(m_textBox_port, portBuffer, 16);
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
	void Diagnostics::ProcessMessage(std::vector<char>& data)
	{
		int msgType = com::ReadFlipBytes<int>(0, data);
		switch (msgType)
		{
		case (int)com::MessageType::STRING:
			RecvString(data);
			break;
		case (int)com::MessageType::COMMAND:
			RecvCommand(data);
			break;
		case (int)com::MessageType::CARDIAG:
			RecvDiag(data);
			break;
		}
	}
	void Diagnostics::RecvString(std::vector<char>& data)
	{
		std::wstring str;
		for (int i = 4; i < (int)data.size() / 2; i++)
		{
			WCHAR ch = data[2 * i + 1] + data[2 * i] * 0x100;
			str += ch;
		}
		SetWindowText(m_textBox_inMsg, str.c_str());
	}
	void Diagnostics::RecvCommand(std::vector<char>& data)
	{
		int cmd = com::ReadFlipBytes<int>(4, data);
		switch (cmd)
		{
		case (int)com::CommandType::FORWARD:
			m_scene->setCarSpeed(1);
			break;
		case (int)com::CommandType::BACKWARD:
			m_scene->setCarSpeed(-1);
			break;
		case (int)com::CommandType::NOMOVE:
			m_scene->setCarSpeed(0);
			break;
		case (int)com::CommandType::LEFT:
			m_scene->setCarSteering(-1);
			break;
		case (int)com::CommandType::RIGHT:
			m_scene->setCarSteering(1);
			break;
		case (int)com::CommandType::NOSTEERING:
			m_scene->setCarSteering(0);
			break;
		case (int)com::CommandType::RESET:
			m_scene->Restart();
			break;
		case (int)com::CommandType::AUTOMODE:
			m_scene->SwitchCarPilotAutoManual();
			break;
		}
	}
	void Diagnostics::RecvDiag(std::vector<char>& data)
	{
		com::CarDiagnosticData diagData;
		diagData.distanceSensorSignal[0] = m_scene->getUserCar().getDistanceSensor(0).getMeasurement();
		diagData.distanceSensorSignal[1] = m_scene->getUserCar().getDistanceSensor(1).getMeasurement();
		diagData.lightSensorSignal[0] = m_scene->getUserCar().getLightSensor(0).getMeasurement();
		diagData.lightSensorSignal[1] = m_scene->getUserCar().getLightSensor(1).getMeasurement();
		diagData.lightSensorSignal[2] = m_scene->getUserCar().getLightSensor(2).getMeasurement();
		diagData.lightSensorSignal[3] = m_scene->getUserCar().getLightSensor(3).getMeasurement();
		diagData.lightSensorSignal[4] = m_scene->getUserCar().getLightSensor(4).getMeasurement();
		diagData.lightSensorSignal[5] = m_scene->getUserCar().getLightSensor(5).getMeasurement();
		diagData.position[0] = m_scene->getUserCar().position.x;
		diagData.position[1] = m_scene->getUserCar().position.z;

		m_connection->Send(diagData);
		m_connection->Send(std::to_wstring(diagData.distanceSensorSignal[0] < diagData.distanceSensorSignal[1] ?
			diagData.distanceSensorSignal[0] : diagData.distanceSensorSignal[1]));
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
		SetWindowText(m_label_status, L"Online");
		SetWindowText(m_button_conn, L"Disconnect");
	}
	void Diagnostics::EndDiagnostics()
	{
		m_target = nullptr;
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
	void Diagnostics::CommandAction(HWND hwnd, UINT btnID)
	{
		switch (btnID)
		{
		case ID_BTN_CONNECT:
			if (m_connection && m_connection->isOnline())
				DisconnectFromServer();
			else
				ConnectToServer();
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
	void Diagnostics::MessageReceived(std::vector<char> *buffer)
	{
		if (buffer == nullptr)
		{
			EndDiagnostics();
		}
		else
		{
			ProcessMessage(*buffer);
			delete buffer;
		}
	}
}
