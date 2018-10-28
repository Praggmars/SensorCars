#include "communication.h"

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
			int msgLen;
			int tmp = sizeof(msgLen);
			while (tmp > 0)
				if (m_isConnectionOpen)
				{
					char len[4];
					int result = recv(m_socket, &len[sizeof(msgLen) - tmp], tmp, 0);
					msgLen = ((len[0] * 0x100 + len[1]) * 0x100 + len[2]) * 0x100 + len[3];
					if (result > 0)
						tmp -= result;
					else
					{
						EndConnection();
						return;
					}
				}
			m_dataReady = false;
			m_recvBuffer.resize(msgLen);
			tmp = msgLen - sizeof(msgLen);
			while (tmp > 0)
				if (m_isConnectionOpen)
				{
					int result = recv(m_socket, &m_recvBuffer[msgLen - tmp], tmp, 0);
					if (result > 0)
						tmp -= result;
					else
					{
						EndConnection();
						return;
					}
				}
			m_dataReady = true;
		}
	}
	void Communication::StartConnection()
	{
		m_dataReady = false;
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
			throw std::exception("Could not connect");
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
			m_recvBuffer.clear();
			m_dataReady = false;
			m_isConnectionOpen = false;
		}
	}
	bool Communication::Send(std::wstring msg)
	{
		if (m_isConnectionOpen)
		{
			std::vector<char> data;
			unsigned size = 8 + 2 * msg.length();
			data.reserve(size);
			char* b = (char*)&size;
			for (int i = sizeof(size) - 1; i >= 0; i--)
				data.push_back(b[i]);
			size -= 8;
			for (int i = sizeof(size) - 1; i >= 0; i--)
				data.push_back(b[i]);
			for (int i = 0; i < (int)msg.length(); i++)
			{
				char* b = (char*)&msg[i];
				data.push_back(b[1]);
				data.push_back(b[0]);
			}

			int tmp = data.size();
			while (tmp > 0)
				tmp -= send(m_socket, &data[data.size() - tmp], tmp, 0);
		}
		return m_isConnectionOpen;
	}
	bool Communication::FetchRecvData(std::vector<char>& data)
	{
		if (m_dataReady)
		{
			data = m_recvBuffer;
			m_dataReady = false;
			return true;
		}
		return false;
	}
}