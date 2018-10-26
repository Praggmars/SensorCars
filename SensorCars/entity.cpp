#include "entity.h"

namespace gfx
{
	void Entity::Init(Model *model, std::vector<mth::Triangle>* modelFrame, mth::float4 color)
	{
		m_model = model;
		m_modelFrame = modelFrame;
		m_color = color;
	}

	void Entity::Render(ID3D11DeviceContext *context, mth::matrix matrixBuffer[], Material &material)
	{
		matrixBuffer[0] = GetWorldMatrix();
		material.WriteMatrixBuffer(context, matrixBuffer);
		material.WriteColorBuffer(context, &m_color);
		material.SetShadersBuffers(context);
		m_model->SetBuffers(context);
		m_model->Draw(context);
	}

	void Entity::Render(ID3D11DeviceContext *context, mth::matrix matrixBuffer[], Material &material, mth::matrix& offset)
	{
		matrixBuffer[0] = offset * GetWorldMatrix();
		material.WriteMatrixBuffer(context, matrixBuffer);
		material.WriteColorBuffer(context, &m_color);
		material.SetShadersBuffers(context);
		m_model->SetBuffers(context);
		m_model->Draw(context);
	}
}
