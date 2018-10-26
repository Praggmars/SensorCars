#include "material.h"
#include "linalg.h"
#include <fstream>

namespace gfx
{
	bool LoadShaderCode(AutoReleasePtr<ID3DBlob>& shaderByteCode, LPCWSTR filename)
	{
		std::fstream f(filename, std::ios::in | std::ios::binary);
		if (!f.good())
			return false;
		auto begin = f.tellg();
		f.seekg(0, std::ios::end);
		auto end = f.tellg();
		f.seekg(0);
		if (FAILED(D3DCreateBlob((SIZE_T)(end - begin), &shaderByteCode)))
			return false;
		f.read((char*)shaderByteCode->GetBufferPointer(), shaderByteCode->GetBufferSize());
		f.close();
		return  true;
	}

	void Material::WriteBuffer(ID3D11DeviceContext *context, ID3D11Buffer *buffer, void *data, size_t size)
	{
		D3D11_MAPPED_SUBRESOURCE resource;
		if (SUCCEEDED(context->Map(buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource)))
		{
			memcpy(resource.pData, data, size);
			context->Unmap(buffer, 0);
		}
	}
	bool Material::Init(ID3D11Device *device)
	{
		m_matrixBufferSize = sizeof(mth::matrix) * 2;
		m_lightBufferSize = sizeof(float) * 8;
		m_colorBufferSize = sizeof(float) * 4;

		AutoReleasePtr<ID3DBlob> shaderByteCode;
		if (!LoadShaderCode(shaderByteCode, L"resources/vertexShader.cso"))
			return false;
		if (FAILED(device->CreateVertexShader(shaderByteCode->GetBufferPointer(), shaderByteCode->GetBufferSize(), NULL, &m_vertexShader)))
			return false;
		D3D11_INPUT_ELEMENT_DESC inputLayoutDesc[2];
		inputLayoutDesc[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 };
		inputLayoutDesc[1] = { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 };
		if (FAILED(device->CreateInputLayout(inputLayoutDesc, 2, shaderByteCode->GetBufferPointer(), shaderByteCode->GetBufferSize(), &m_inputLayout)))
			return false;

		shaderByteCode.Release();
		if (!LoadShaderCode(shaderByteCode, L"resources/pixelShader.cso"))
			return false;
		if (FAILED(device->CreatePixelShader(shaderByteCode->GetBufferPointer(), shaderByteCode->GetBufferSize(), NULL, &m_pixelShader)))
			return false;

		D3D11_BUFFER_DESC bufferDesc{};
		bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		bufferDesc.ByteWidth = (UINT)m_matrixBufferSize;
		bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		if (FAILED(device->CreateBuffer(&bufferDesc, nullptr, &m_matrixBuffer)))
			return false;
		bufferDesc.ByteWidth = (UINT)m_lightBufferSize;
		if (FAILED(device->CreateBuffer(&bufferDesc, nullptr, &m_lightBuffer)))
			return false;
		bufferDesc.ByteWidth = (UINT)m_colorBufferSize;
		if (FAILED(device->CreateBuffer(&bufferDesc, nullptr, &m_colorBuffer)))
			return false;

		return true;
	}
	void Material::Shutdown()
	{
		m_matrixBufferSize = 0;
		m_lightBufferSize = 0;
		m_colorBufferSize = 0;
		m_vertexShader.Release();
		m_pixelShader.Release();
		m_inputLayout.Release();
		m_matrixBuffer.Release();
		m_lightBuffer.Release();
		m_colorBuffer.Release();
	}
	void Material::SetShadersBuffers(ID3D11DeviceContext *context)
	{
		context->VSSetShader(m_vertexShader, nullptr, 0);
		context->IASetInputLayout(m_inputLayout);
		context->PSSetShader(m_pixelShader, nullptr, 0);

		context->VSSetConstantBuffers(0, 1, &m_matrixBuffer);
		context->PSSetConstantBuffers(0, 1, &m_lightBuffer);
		context->PSSetConstantBuffers(1, 1, &m_colorBuffer);
	}
	void Material::WriteMatrixBuffer(ID3D11DeviceContext *context, void *data)
	{
		WriteBuffer(context, m_matrixBuffer, data, m_matrixBufferSize);
	}
	void Material::WriteLightBuffer(ID3D11DeviceContext *context, void *data)
	{
		WriteBuffer(context, m_lightBuffer, data, m_lightBufferSize);
	}
	void Material::WriteColorBuffer(ID3D11DeviceContext *context, void *data)
	{
		WriteBuffer(context, m_colorBuffer, data, m_colorBufferSize);
	}
}