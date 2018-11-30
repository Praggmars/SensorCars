#pragma once

#include "scene.h"
#include "communication.h"

namespace diag
{
	class Diagnostics
	{
		HWND m_textBox_IP;
		HWND m_textBox_port;
		HWND m_label_status;
		HWND m_button_conn;
		HWND m_textBox_inMsg;
		HWND m_textBox_outMsg;
		HWND m_window;

		car::Car *m_target;
		car::Scene *m_scene;
		std::unique_ptr<com::Communication> m_connection;

	private:
		void ConnectToServer();
		void DisconnectFromServer();
		void ProcessMessage(std::vector<char>& data);
		void RecvString(std::vector<char>& data);
		void RecvCommand(std::vector<char>& data);
		void RecvDiag(std::vector<char>& data);
		void SendDiagnosticsData(int diagType);

	public:
		Diagnostics();
		void StartDiagnostic(LPCWSTR ip, LPCWSTR port);
		void EndDiagnostics();
		inline void setScene(car::Scene *scene) { m_scene = scene; }
		inline void setTarget(car::Car *car) { m_target = car; }

		void CreateGUI(HWND hwnd);
		void CommandAction(HWND hwnd, UINT btnID);
		void MessageReceived(std::vector<char> *buffer);

		inline com::Communication* getConnection() { return m_connection.get(); }
	};
}