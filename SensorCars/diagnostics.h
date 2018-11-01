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
		void SendCarDiagData();
		void ActReceivedCommand(com::CommandType cmd);

	public:
		Diagnostics();
		void StartDiagnostic(LPCWSTR ip, LPCWSTR port);
		void EndDiagnostics();
		inline void setScene(car::Scene *scene) { m_scene = scene; }
		inline void setTarget(car::Car *car) { m_target = car; }

		void CreateGUI(HWND hwnd);
		void UpdateGUI();
		void CommandAction(HWND hwnd, UINT btnID);

		inline com::Communication* getConnection() { return m_connection.get(); }
	};
}