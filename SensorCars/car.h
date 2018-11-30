#pragma once

#include "sensor.h"
#include <array>

namespace car
{
	class Car :public gfx::Entity
	{
		std::array<Sensor, 6> m_lightSensors;
		std::array<Sensor, 2> m_distanceSensors;

		float m_maxSpeed;
		float m_maxSteering;
		float m_currentSpeed;
		float m_currentSteering;
		
	public:
		void Init(gfx::Model *model, std::vector<mth::Triangle> *modelFrame, mth::float3 position, mth::float3 rotation, mth::float4 color = { 1.0f, 0.4f, 0.2f, 1.0f });
		void InitSensors(gfx::Model *sensorModel);
		inline std::vector<mth::Triangle>& getHitbox() { return *m_modelFrame; }
		inline float getSpeed() { return m_currentSpeed; }
		inline void setSpeed(float speed) { m_currentSpeed = speed; }
		inline float getSteering() { return m_currentSteering; }
		inline void setSteering(float steering) { m_currentSteering = steering; }

		inline Sensor& getLightSensor(UINT index) { return m_lightSensors[index]; }
		inline int getLightSensorCount() { return (int)m_lightSensors.size(); }
		inline Sensor& getDistanceSensor(UINT index) { return m_distanceSensors[index]; }
		inline int getDistanceSensorCount() { return (int)m_distanceSensors.size(); }

		void RenderSensors(ID3D11DeviceContext *context, mth::matrix matrixBuffer[], gfx::Material& material);

		void UpdateLightSensors(std::vector<Hitbox>& enviroment);
		void UpdateDistanceSensors(std::vector<Hitbox>& enviroment);
		void Control_Auto();
		void Update(float deltaTime);
	};
}