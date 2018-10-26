#pragma once

#include "autoreleaseptr.h"
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <d3d11.h>
#include <d3dcompiler.h>

namespace gfx
{
	class Graphics
	{
		AutoReleasePtr<ID3D11Device> m_device;
		AutoReleasePtr<ID3D11DeviceContext> m_context;
		AutoReleasePtr<IDXGISwapChain> m_swapChain;
		AutoReleasePtr<ID3D11RenderTargetView> m_renderTargetView;
		AutoReleasePtr<ID3D11Texture2D> m_depthStencilBuffer;
		AutoReleasePtr<ID3D11DepthStencilView> m_depthStencilView;
		AutoReleasePtr<ID3D11DepthStencilState> m_depthStencilState;
		AutoReleasePtr<ID3D11RasterizerState> m_rasterizerState;

	public:
		bool Init(HWND hwnd, int width, int height);
		void Shutdown();

		void BeginFrame();
		void BeginFrame(float r, float g, float b);
		void EndFrame();

		inline ID3D11Device* getDevice() { return m_device; }
		inline ID3D11DeviceContext* getContext() { return m_context; }
	};
}