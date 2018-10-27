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
		if (m_isConnectionOpen)
		{
			shutdown(m_socket, SD_SEND);
			closesocket(m_socket);
			WSACleanup();
			m_recvBuffer.clear();
			m_recvThread.release();
			m_dataReady = false;
			m_isConnectionOpen = false;
		}
	}
	Communication::Communication(LPCWSTR ip, LPCWSTR port)
	{
		ParseIP_Port(ip, port);
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
		m_recvThread = std::unique_ptr<std::thread>(new std::thread([this]() {WaitForDataRecv(); }));
		m_isConnectionOpen = true;

		Send("213", 3);
	}
	void Communication::WaitForDataRecv()
	{
		while (m_isConnectionOpen)
		{
			int msgLen;
			if (m_isConnectionOpen)
				recv(m_socket, (char*)&msgLen, 4, 0);
			m_dataReady = false;
			m_recvBuffer.resize(msgLen);
			if (m_isConnectionOpen)
				recv(m_socket, m_recvBuffer.data(), msgLen, 0);
			m_dataReady = true;
		}
	}
	bool Communication::Send(const char *msg, int msgLen)
	{
		if (m_isConnectionOpen)
		{
			int tmp = 4;
			send(m_socket, (char*)&msgLen, 4, 0);
			send(m_socket, msg, msgLen, 0);
		}
		return m_isConnectionOpen;
	}
	bool Communication::FetchRecvData(std::vector<char>& data)
	{
		if (m_dataReady)
			data = m_recvBuffer;
		return m_dataReady;
	}
}