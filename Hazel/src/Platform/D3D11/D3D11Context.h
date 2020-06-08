#pragma once

#include "Hazel/Renderer/GraphicsContext.h"

#include <Windows.h> // TODO: make windows chili filter min max
#include <d3d11.h>
#include <wrl.h>

namespace Hazel {

	class D3D11Context : public GraphicsContext
	{
	public:
		D3D11Context(Window& windowHandle);

		virtual void Init() override;
		virtual void SwapBuffers() override;

		virtual RendererAPI::API GetAPI() override;
		virtual RendererAPI::API MakeCurrent() override;
	private:
		Window& m_window;
		HWND* m_windowHandle;

		struct Struct_ppD3D
		{
			Microsoft::WRL::ComPtr<ID3D11Device> m_pDevice = nullptr;
			Microsoft::WRL::ComPtr<IDXGISwapChain> m_pSwap = nullptr;
			Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_pContext = nullptr;
			Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_pTarget = nullptr;
		};
		Struct_ppD3D ppD3D;
		Struct_ppD3D& GetPP() { return ppD3D; }

		// API functions
	public:
		virtual void DrawTriangle_impl() override;
	};


}