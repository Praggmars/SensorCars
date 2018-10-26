#pragma once

#include "entity.h"
#include <functional>

namespace car
{
	struct Hitbox
	{
		std::vector<mth::Triangle> *modelFrame;
		mth::Position *position;
		float shade;
	};

	class Sensor :public gfx::Entity
	{
		mth::Position *m_holder;
		float m_measurement;
		std::function<float(float dist, float shade)> m_equation;

	public:
		void Init(gfx::Model *model, std::vector<mth::Triangle> *modelFrame, mth::float3 position, mth::float3 rotation, mth::Position *holder);
		void Init(gfx::Model *model, std::vector<mth::Triangle> *modelFrame, mth::float3 position, mth::float3 rotation, mth::Position *holder, std::function<float(float dist, float shade)> equation);

		void Update(std::vector<Hitbox>& enviroment);

		inline float getMeasurement() { return m_measurement; }
	};
}