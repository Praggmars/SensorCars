#pragma once

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <WinSock2.h>
#include <string>
#include <thread>
#include <memory>
#include <vector>

#pragma comment(lib,"ws2_32.lib")

#define WM_RECVMSG WM_USER+10

namespace com
{
	enum class MessageType :int
	{
		STRING = 1,
		COMMAND = 2,
		CARDIAG = 3
	};
	enum class CommandType :int
	{
		FORWARD = 1,
		BACKWARD = 2,
		NOMOVE = 3,
		LEFT = 4,
		RIGHT = 5,
		NOSTEERING = 6,
		RESET = 7,
		AUTOMODE = 8,
		SELFTEST = 9
	};
	enum class DiagType :int
	{
		DISTANCE = 1 << 0,
		LIGHT = 1 << 1,
		POSITION = 1 << 2,
		MOVEMENT = 1 << 3
	};

	class Communication
	{
		bool m_isConnectionOpen;
		std::string m_ip;
		int m_port;
		SOCKET m_socket;

	private:
		void ParseIP_Port(LPCWSTR ip, LPCWSTR port);
		void WaitForDataRecv();
		bool RecvData(char *buffer, int length);
		void StartConnection();
		void EndConnection();

	public:
		Communication(LPCWSTR ip, LPCWSTR port);
		~Communication();
		inline bool isOnline() { return m_isConnectionOpen; }
		bool Send(std::wstring msg);
		bool Send(int diagType, float *data);
	};

	template <typename T>
	T FlipBytes(T t)
	{
		T tmp;
		char *btmp = (char*)&tmp;
		char *bt = (char*)&t;
		for (int i = 0; i < sizeof(T); i++)
			btmp[i] = bt[sizeof(T) - 1 - i];
		return tmp;
	}

	template <typename T>
	void PushFlipBytes(T t, std::vector<char>& buffer)
	{
		char *b = (char*)&t;
		for (int i = sizeof(T) - 1; i >= 0; i--)
			buffer.push_back(b[i]);
	}

	template <typename T>
	void PushNoFlipBytes(T t, std::vector<char>& buffer)
	{
		char *b = (char*)&t;
		for (int i = 0; i < sizeof(T); i++)
			buffer.push_back(b[i]);
	}

	template <typename T>
	T ReadFlipBytes(int offset, std::vector<char>& buffer)
	{
		T r;
		char *b = (char*)&r;
		for (int i = 0; i < sizeof(T); i++)
			b[sizeof(T) - 1 - i] = buffer[i + offset];
		return r;
	}
}