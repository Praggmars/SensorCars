#pragma once

#include "graphics.h"
#include "linalg.h"

namespace gfx
{
	struct VerteyType
	{
		mth::float3 position;
		mth::float3 normal;
	};

	class Model
	{
		AutoReleasePtr<ID3D11Buffer> m_vertexBuffer;
		AutoReleasePtr<ID3D11Buffer> m_indexBuffer;

		UINT m_indexCount;

	public:
		bool Init(ID3D11Device *device, void *vertices, size_t vertexSize, void *indices, size_t indexSize);
		void Shutdown();

		void SetBuffers(ID3D11DeviceContext *context);
		void Draw(ID3D11DeviceContext *context);
	};
}