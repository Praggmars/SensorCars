#pragma once

#include "camera.h"
#include "car.h"
#include "communication.h"

namespace car
{
	class Scene
	{
		HWND m_owner;
		HWND m_gfxScreen;
		gfx::Graphics m_gfx; 
		gfx::Material m_material;
		gfx::Model m_mdlPath;
		gfx::Model m_mdlPlain;
		gfx::Model m_mdlSensor;
		gfx::Model m_mdlCar;
		std::vector<mth::Triangle> m_mfPath;
		std::vector<mth::Triangle> m_mfPlain;
		std::vector<mth::Triangle> m_mfCar;

		std::vector<Hitbox> m_envCars;
		std::vector<Hitbox> m_envFloor;

		Car m_userCar;
		std::array<Car, 4> m_cars;
		gfx::Entity m_path;
		gfx::Entity m_plain;

		gfx::Camera m_camera;

		bool m_forward;
		bool m_back;
		bool m_left;
		bool m_right;
		bool m_autoControl;

	private:
		bool LoadEntity(ID3D11Device *device, LPCWSTR filename, gfx::Model *model, std::vector<mth::Triangle> *m_modelFrame);
		bool LoadPath(ID3D11Device *device);
		bool LoadPlain(ID3D11Device *device);
		bool LoadSensor(ID3D11Device *device);
		bool LoadCar(ID3D11Device *device);
		HWND CreateGfxScreen(RECT position);
		Hitbox CreateHitbox(std::vector<mth::Triangle> *modelFrame, gfx::Entity *entity);

		void ControlUserCarManual(float deltaTime);

		void Update(float deltaTime);
		void Render();

	public:
		bool Init(HWND owner, RECT position);

		void Frame(float deltaTime);
		void MessageHandler(MSG& msg);
		void Restart();

		inline Car& getCar(size_t index) { return m_cars[index]; }
		inline Car& getUserCar() { return m_userCar; }

		void setCarSpeed(int s);
		void setCarSteering(int s);
		void SwitchCarPilotAutoManual();
	};
}