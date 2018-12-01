#include "sensor.h"
#include <fstream>

namespace car
{
	void Sensor::Init(gfx::Model *model, std::vector<mth::Triangle> *modelFrame, mth::float3 position, mth::float3 rotation, mth::Position *holder)
	{
		Init(model, modelFrame, position, rotation, holder, [](float dist, float shade)->float {return (shade + 0.01f) / dist; });
	}
	void Sensor::Init(gfx::Model *model, std::vector<mth::Triangle> *modelFrame, mth::float3 position, mth::float3 rotation, mth::Position *holder, std::function<float(float dist, float shade)> equation)
	{
		gfx::Entity::Init(model, modelFrame, { 0.2f, 0.3f, 0.9f, 1.0f });
		this->position = position;
		this->rotation = rotation;
		m_holder = holder;
		m_equation = equation;
	}
	void Sensor::Update(std::vector<Hitbox>& enviroment)
	{
		float mindist = 1000.0f;
		float materialBrightness = 0.0f;
		mth::float4x4 relativeMatrix = m_holder->GetWorldMatrix();
		mth::Position pos;
		for (Hitbox&e : enviroment)
		{
			mth::matrix hbMatrix = e.position->GetWorldMatrix();
			for (mth::Triangle& tri : *(e.modelFrame))
			{
				mth::Triangle t(hbMatrix*tri.vertices[0],
					hbMatrix*tri.vertices[1],
					hbMatrix*tri.vertices[2]);
				pos.position = relativeMatrix * position;
				pos.rotation = m_holder->rotation + rotation;
				float dist = mth::DirectionalDistanceFromTriangle(pos, t);
				if (dist < mindist && dist > 0.1f)
				{
					mindist = dist;
					materialBrightness = e.shade;
				}
			}
		}
		m_measurement = m_equation(mindist, materialBrightness);
		m_color = { m_measurement, m_measurement, m_measurement, 1.0f };
	}
}