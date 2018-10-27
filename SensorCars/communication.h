#pragma once

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <WinSock2.h>
#include <string>
#include <thread>
#include <memory>
#include <vector>

#pragma comment(lib,"ws2_32.lib")

namespace com
{
	class Communication
	{
		std::unique_ptr<std::thread> m_recvThread;
		//std::unique_ptr<std::thread> m_sendThread;
		bool m_dataReady;
		std::vector<char> m_recvBuffer;
		bool m_isConnectionOpen;
		std::string m_ip;
		int m_port;
		SOCKET m_socket;

	private:
		void ParseIP_Port(LPCWSTR ip, LPCWSTR port);
		void WaitForDataRecv();

	public:
		Communication(LPCWSTR ip, LPCWSTR port);
		~Communication();
		bool Send(const char* msg, int msgLen);
		bool FetchRecvData(std::vector<char>& data);
	};
}