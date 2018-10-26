#include "model.h"

namespace gfx
{
	bool Model::Init(ID3D11Device *device, void *vertices, size_t vertexSize, void *indices, size_t indexSize)
	{
		m_indexCount = (UINT)(indexSize / sizeof(UINT));
		D3D11_BUFFER_DESC bufferDesc{};
		D3D11_SUBRESOURCE_DATA initData{};
		bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bufferDesc.ByteWidth = (UINT)vertexSize;
		bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
		initData.pSysMem = vertices;
		if (FAILED(device->CreateBuffer(&bufferDesc, &initData, &m_vertexBuffer)))
			return false;
		bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		bufferDesc.ByteWidth = (UINT)indexSize;
		bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
		initData.pSysMem = indices;
		if (FAILED(device->CreateBuffer(&bufferDesc, &initData, &m_indexBuffer)))
			return false;
		return true;
	}
	void Model::Shutdown()
	{
		m_indexCount = 0;
		m_vertexBuffer.Release();
		m_indexBuffer.Release();
	}
	void Model::SetBuffers(ID3D11DeviceContext *context)
	{
		UINT stride = sizeof(VerteyType);
		UINT offset = 0;

		context->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);
		context->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	}
	void Model::Draw(ID3D11DeviceContext *context)
	{
		context->DrawIndexed(m_indexCount, 0, 0);
	}
}