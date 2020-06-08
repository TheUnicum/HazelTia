#include "hzpch.h"
#include "Platform/D3D11/D3D11Context.h"

namespace wrl = Microsoft::WRL;

#pragma comment(lib,"d3d11.lib")
#pragma comment(lib,"D3DCompiler.lib")


namespace Hazel {

	D3D11Context::D3D11Context(void* windowHandle)
		: m_WindowHandle(static_cast<HWND*>(windowHandle))
	{
		HZ_CORE_ASSERT(windowHandle, "Window handle is null!");
	}

	void D3D11Context::Init()
	{

		auto pp = std::static_pointer_cast<D3D11Context>(GraphicsContext::Resolve(GetAPI(), m_WindowHandle));
		Microsoft::WRL::ComPtr<IDXGISwapChain> swap_i = pp->GetPP().m_pSwap;


		auto pp1 = std::static_pointer_cast<D3D11Context>(Get_Active());
		Microsoft::WRL::ComPtr<IDXGISwapChain> swap_s = pp->GetPP().m_pSwap;


		DXGI_SWAP_CHAIN_DESC sd = {};
		sd.BufferDesc.Width = 0;
		sd.BufferDesc.Height = 0;
		sd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		sd.BufferDesc.RefreshRate.Numerator = 0;
		sd.BufferDesc.RefreshRate.Denominator = 0;
		sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		sd.SampleDesc.Count = 1;
		sd.SampleDesc.Quality = 0;
		sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		sd.BufferCount = 1;
		sd.OutputWindow = *static_cast<HWND*>(m_WindowHandle);//  hWnd; //(HWND)696969;//
		sd.Windowed = TRUE;
		sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;// DXGI_SWAP_EFFECT_DISCARD;
		sd.Flags = 0;

		UINT swapCreateFlags = 0u;
		#ifdef HZ_DEBUG
		swapCreateFlags |= D3D11_CREATE_DEVICE_DEBUG;
		#endif

		// create device and front/back buffers, and swap chain and rendering context
		D3D11CreateDeviceAndSwapChain(
			nullptr,
			D3D_DRIVER_TYPE_HARDWARE,
			nullptr,
			swapCreateFlags,// 0,
			nullptr,
			0,
			D3D11_SDK_VERSION,
			&sd,
			&ppD3D.m_pSwap,
			&ppD3D.m_pDevice,
			nullptr,
			&ppD3D.m_pContext
		);

		ppD3D.m_pContext->OMSetRenderTargets(0, 0, 0);

		// gain access to texture subresource in swap chain (back buffer)
		wrl::ComPtr<ID3D11Resource> pBackBuffer = nullptr;
		ppD3D.m_pSwap->GetBuffer(0, __uuidof(ID3D11Resource), &pBackBuffer);
		ppD3D.m_pDevice->CreateRenderTargetView(pBackBuffer.Get(), nullptr, &ppD3D.m_pTarget);

		//i_data.m_pContext->OMSetRenderTargets(1, &i_data.m_pTarget, NULL);
	}

	void D3D11Context::SwapBuffers()
	{
		ppD3D.m_pSwap->Present(1u, 0u);
	}

	RendererAPI::API D3D11Context::GetAPI()
	{
		return RendererAPI::API::D3D11;
	}

	RendererAPI::API D3D11Context::MakeCurrent()
	{
		_s_active = Resolve(RendererAPI::API::OpenGL, (void*)m_WindowHandle);
		return GetAPI();

	}
}
