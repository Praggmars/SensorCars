#pragma once

#include "graphics.h"

namespace gfx
{
	class Material
	{
		AutoReleasePtr<ID3D11VertexShader> m_vertexShader;
		AutoReleasePtr<ID3D11PixelShader> m_pixelShader;
		AutoReleasePtr<ID3D11InputLayout> m_inputLayout;
		AutoReleasePtr<ID3D11Buffer> m_matrixBuffer;
		AutoReleasePtr<ID3D11Buffer> m_lightBuffer;
		AutoReleasePtr<ID3D11Buffer> m_colorBuffer;

		size_t m_matrixBufferSize;
		size_t m_lightBufferSize;
		size_t m_colorBufferSize;

	private:
		void WriteBuffer(ID3D11DeviceContext *context, ID3D11Buffer *buffer, void *data, size_t size);

	public:
		bool Init(ID3D11Device *device);
		void Shutdown();

		void SetShadersBuffers(ID3D11DeviceContext *context);

		void WriteMatrixBuffer(ID3D11DeviceContext *context, void *data);
		void WriteLightBuffer(ID3D11DeviceContext *context, void *data);
		void WriteColorBuffer(ID3D11DeviceContext *context, void *data);
	};
}