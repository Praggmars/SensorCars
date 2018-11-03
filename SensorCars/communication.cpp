#include "communication.h"

extern HWND g_appWindow;

namespace com
{
	void Communication::ParseIP_Port(LPCWSTR ip, LPCWSTR port)
	{
		for (size_t i = 0; ip[i]; i++)
			m_ip += (char)ip[i];
		m_port = std::stoi(port);
	}
	Communication::~Communication()
	{
		EndConnection();
	}
	Communication::Communication(LPCWSTR ip, LPCWSTR port)
	{
		ParseIP_Port(ip, port);
		StartConnection();
		WCHAR msg[] = { 0x3053,0x3093,0x306B,0x3061,0x306F,0x0 };
		Send(msg);
	}
	void Communication::WaitForDataRecv()
	{
		while (m_isConnectionOpen)
		{
			std::vector<char> *recvBuffer = new std::vector<char>;
			int msgLen;
			if (m_isConnectionOpen = RecvData((char*)&msgLen, sizeof(msgLen)))
			{
				msgLen = FlipBytes(msgLen);
				recvBuffer->resize(msgLen - sizeof(msgLen));
				if (m_isConnectionOpen = RecvData(recvBuffer->data(), (int)recvBuffer->size()))
				{
					PostMessage(g_appWindow, WM_RECVMSG, (WPARAM)0, (LPARAM)recvBuffer);
				}
			}
		}
		PostMessage(g_appWindow, WM_RECVMSG, (WPARAM)0, (LPARAM)nullptr);
	}
	bool Communication::RecvData(char *buffer, int length)
	{
		int tmp = length;
		while (tmp > 0)
		{
			if (m_isConnectionOpen)
			{
				int result = recv(m_socket, &buffer[length - tmp], tmp, 0);
				if (!m_isConnectionOpen)
					return false;
				if (result > 0)
					tmp -= result;
				else
				{
					EndConnection();
					return false;
				}
			}
		}
		return true;
	}
	void Communication::StartConnection()
	{
		WSAData wsaData;
		WORD DllVersion = MAKEWORD(2, 1);
		if (WSAStartup(DllVersion, &wsaData) != 0)
			throw std::exception("Winsock startup failed");
		SOCKADDR_IN addr;
		int addrlen = sizeof(addr);
		addr.sin_addr.s_addr = inet_addr(m_ip.c_str());
		addr.sin_port = htons(m_port);
		addr.sin_family = AF_INET;
		m_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (connect(m_socket, (SOCKADDR*)&addr, addrlen) != 0)
		{
			std::string errorMessage("Could not connect. Error code: ");
			errorMessage += std::to_string(WSAGetLastError());
			throw std::exception(errorMessage.c_str());
		}
		m_isConnectionOpen = true;
		std::thread th([this]() {WaitForDataRecv(); });
		th.detach();
	}
	void Communication::EndConnection()
	{
		if (m_isConnectionOpen)
		{
			shutdown(m_socket, SD_SEND);
			closesocket(m_socket);
			WSACleanup();
			m_isConnectionOpen = false;
		}
	}
	bool Communication::Send(std::wstring msg)
	{
		if (m_isConnectionOpen)
		{
			std::vector<char> data;
			int size = 12 + 2 * (int)msg.length();
			data.reserve(size);
			PushFlipBytes(size, data);
			PushFlipBytes(MessageType::STRING, data);
			PushFlipBytes((int)msg.length() * 2, data);
			for (int i = 0; i < (int)msg.length(); i++)
				PushFlipBytes(msg[i], data);

			int tmp = (int)data.size();
			while (tmp > 0)
				tmp -= send(m_socket, &data[data.size() - tmp], tmp, 0);
		}
		return m_isConnectionOpen;
	}
	bool Communication::Send(CarDiagnosticData diagData)
	{
		if (m_isConnectionOpen)
		{
			std::vector<char> data;
			int size = 8 + sizeof(com::CarDiagnosticData);
			data.reserve(size);
			PushFlipBytes(size, data);
			PushFlipBytes(MessageType::CARDIAG, data);
			PushFlipBytes(diagData.distanceSensorSignal[0], data);
			PushFlipBytes(diagData.distanceSensorSignal[1], data);
			PushFlipBytes(diagData.lightSensorSignal[0], data);
			PushFlipBytes(diagData.lightSensorSignal[1], data);
			PushFlipBytes(diagData.lightSensorSignal[2], data);
			PushFlipBytes(diagData.lightSensorSignal[3], data);
			PushFlipBytes(diagData.lightSensorSignal[4], data);
			PushFlipBytes(diagData.lightSensorSignal[5], data);
			PushFlipBytes(diagData.position[0], data);
			PushFlipBytes(diagData.position[1], data);

			int tmp = (int)data.size();
			while (tmp > 0)
				tmp -= send(m_socket, &data[data.size() - tmp], tmp, 0);
		}
		return m_isConnectionOpen;
	}
}