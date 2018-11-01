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
	enum class MessageType :int
	{
		STRING,
		COMMAND,
		CARDIAG
	};
	enum class CommandType :int
	{
		FORWARD,
		BACKWARD,
		NOMOVE,
		LEFT,
		RIGHT,
		NOSTEERING,
		RESET,
		AUTOMODE
	};
	struct CarDiagnosticData
	{
		int carID;
		float distanceSensorSignal[2];
		float lightSensorSignal[6];
		int collidedWith;
	};

	class Communication
	{
		bool m_dataReady;
		std::vector<char> m_recvBuffer;
		bool m_isConnectionOpen;
		std::string m_ip;
		int m_port;
		SOCKET m_socket;

	private:
		void ParseIP_Port(LPCWSTR ip, LPCWSTR port);
		void WaitForDataRecv();
		void StartConnection();
		void EndConnection();

	public:
		Communication(LPCWSTR ip, LPCWSTR port);
		~Communication();
		inline bool isOnline() { return m_isConnectionOpen; }
		bool Send(std::wstring msg);
		bool Send(CarDiagnosticData data);
		bool FetchRecvData(std::vector<char>& data);
	};
}