#pragma once

#include "model.h"
#include "material.h"
#include "geometry.h"
#include <vector>

namespace gfx
{
	class Entity :public mth::Position
	{
	protected:
		Model *m_model;
		std::vector<mth::Triangle> *m_modelFrame;
		mth::float4 m_color;

	public:
		void Init(Model *model, std::vector<mth::Triangle> *modelFrame, mth::float4 color);
		inline void setColor(mth::float4 color) { m_color = color; }
		inline mth::float4 getColor() { return m_color; }

		void Render(ID3D11DeviceContext *context, mth::matrix matrixBuffer[], Material& material);
		void Render(ID3D11DeviceContext *context, mth::matrix matrixBuffer[], Material& material, mth::matrix& offset);
	};
}