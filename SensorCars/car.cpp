#include "car.h"
#include <fstream>

namespace car
{
	void Car::Init(gfx::Model *model, std::vector<mth::Triangle> *modelFrame, mth::float3 position, mth::float3 rotation, mth::float4 color)
	{
		gfx::Entity::Init(model, modelFrame, color);
		this->position = position;
		this->rotation = rotation;
		m_speed = 10.0f;
		m_steering = 0.5f;
	}
	void Car::InitSensors(gfx::Model *sensorModel)
	{
		mth::float3 pos = { -0.25f, 0.15f, -0.7f };
		for (auto& s : m_lightSensors)
		{
			s.Init(sensorModel, nullptr, pos, { mth::pi / 2.0f, 0.0f, 0.0f }, this);
			pos.x += 0.1f;
		}
		pos = { -0.25f, 0.35f, -0.7f };
		for (auto& s : m_distanceSensors)
		{
			s.Init(sensorModel, nullptr, pos, { 0.0f, mth::pi, 0.0f }, this, 
				[](float dist, float shade)->float {return dist * 0.001f; });
			pos.x += 0.5f;
		}
	}
	void Car::RenderSensors(ID3D11DeviceContext *context, mth::matrix matrixBuffer[], gfx::Material &material)
	{
		mth::matrix worldMatrix = GetWorldMatrix();
		for (auto& s : m_lightSensors)
			s.Render(context, matrixBuffer, material, worldMatrix);
		for (auto& s : m_distanceSensors)
			s.Render(context, matrixBuffer, material, worldMatrix);
	}
	void Car::UpdateLightSensors(std::vector<Hitbox>& enviroment)
	{
		for (auto& s : m_lightSensors)
			s.Update(enviroment);
	}
	void Car::UpdateDistanceSensors(std::vector<Hitbox>& enviroment)
	{
		for (auto& s : m_distanceSensors)
			s.Update(enviroment);
	}
	void Car::Control_Auto(float deltaTime)
	{
		if (m_distanceSensors[0].getMeasurement() < 0.002f ||
			m_distanceSensors[1].getMeasurement() < 0.002f)
			return;
		float distanceTaken = 0.0f;
		MoveForward(-m_speed * deltaTime);
		distanceTaken += m_speed * deltaTime;
		if (m_lightSensors[1].getMeasurement() < 1.0f)
			TurnRight(distanceTaken*m_steering);
		if (m_lightSensors[4].getMeasurement() < 1.0f)
			TurnLeft(distanceTaken*m_steering);
	}
}
