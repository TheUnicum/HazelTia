#pragma once

#include "Hazel/Renderer/GraphicsContext.h"

#define NOMINMAX
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

		virtual API GetAPI() const override { return API::D3D11; };
		virtual std::string GetAPI_TEXT() const override { return "D3D11"; };
		virtual API MakeCurrent() override;
	private:
		Window& m_window;
		HWND* m_windowHandle;

		struct Struct_ppD3D
		{
			Microsoft::WRL::ComPtr<ID3D11Device> m_pDevice = nullptr;
			Microsoft::WRL::ComPtr<IDXGISwapChain> m_pSwap = nullptr;
			Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_pContext = nullptr;
			Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_pTarget = nullptr;
			Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_pDSV = nullptr;
			Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_pRS = nullptr;
		};
		Struct_ppD3D ppD3D;
	public:
		Struct_ppD3D& GetPP() { return ppD3D; }

		// API functions
	public:
		void CmdClear_impl() override;
		void CmdDrawArrays_impl(uint32_t vertexCount, uint32_t offset = 0) override;
		void CmdDrawArraysInstanced_impl(uint32_t vertexCount, uint32_t instanceCount) override;
		void CmdDrawIndexted_impl(uint32_t indexCount, uint32_t offset = 0) override;
		void CmdDrawIndextedInstanced_impl(uint32_t indexCount, uint32_t instanceCount) override;



		virtual void DrawTriangle_impl(float angle) override;
		virtual void DrawTriangle_impl2(float angle);

		virtual void ClearBuffer_impl(float red, float green, float blue) override;
	};


}
