#include "hzpch.h"
#include "Platform/D3D11/D3D11Context.h"

#include "Hazel/Core/Window.h"

#include <d3dcompiler.h>
namespace wrl = Microsoft::WRL;

#pragma comment(lib,"d3d11.lib")
#pragma comment(lib,"D3DCompiler.lib")


// Temp 
#include "D3D11Shader.h" // TODO

namespace Hazel {

	D3D11Context::D3D11Context(Window& window)
		: m_window(window), m_windowHandle(static_cast<HWND*>(window.GetNativeWindow()))
	{
		HZ_CORE_ASSERT(m_windowHandle, "Window handle is null!");
	}

	void D3D11Context::Init()
	{

		//auto pp = std::static_pointer_cast<D3D11Context>(GraphicsContext::Resolve(GetAPI(), m_WindowHandle));
		//Microsoft::WRL::ComPtr<IDXGISwapChain> swap_i = pp->GetPP().m_pSwap;
		//
		//
		//auto pp1 = std::static_pointer_cast<D3D11Context>(Get_Active());
		//Microsoft::WRL::ComPtr<IDXGISwapChain> swap_s = pp->GetPP().m_pSwap;


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
		sd.OutputWindow = *m_windowHandle;//  hWnd; //(HWND)696969;//
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

	API D3D11Context::MakeCurrent()
	{
		if (_s_active.get() != this)
		{
			_s_active = Resolve(m_window);
		}
		return GetAPI();

	}

	void D3D11Context::DrawTriangle_impl()
	{
		#define GFX_THROW_INFO(x) x
		#define GFX_THROW_INFO_ONLY(x) x

		//HRESULT hr;

		struct Vertex
		{
			float x;
			float y;
		};

		// create vertex buffer (1 2d triangle at center of screen)
		const Vertex vertices[] =
		{
			{ 0.0f,0.5f },
			{ 0.5f,-0.5f },
			{ -0.5f,-0.5f },
		};
		wrl::ComPtr<ID3D11Buffer> pVertexBuffer;
		D3D11_BUFFER_DESC bd = {};
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.CPUAccessFlags = 0u;
		bd.MiscFlags = 0u;
		bd.ByteWidth = sizeof(vertices);
		bd.StructureByteStride = sizeof(Vertex);
		D3D11_SUBRESOURCE_DATA sd = {};
		sd.pSysMem = vertices;
		GFX_THROW_INFO(ppD3D.m_pDevice->CreateBuffer(&bd, &sd, &pVertexBuffer));

		// Bind vertex buffer to pipeline
		const UINT stride = sizeof(Vertex);
		const UINT offset = 0u;
		ppD3D.m_pContext->IASetVertexBuffers(0u, 1u, pVertexBuffer.GetAddressOf(), &stride, &offset);



		//create a pixel shader
		//create a vertex shader
		Ref<Shader> shader = (Shader::Create("assets/shaders/D3D/FlatColor.hlsl"));
		shader->Bind();
		auto pBlob = shader->GetpShaderBytecode();



		// input (vertex) layout (2d position only)
		wrl::ComPtr<ID3D11InputLayout> pInputLayout;
		const D3D11_INPUT_ELEMENT_DESC ied[] =
		{
			{ "Position",0,DXGI_FORMAT_R32G32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 },
		};
		GFX_THROW_INFO(ppD3D.m_pDevice->CreateInputLayout(
			ied, (UINT)std::size(ied),
			pBlob->GetBufferPointer(),
			pBlob->GetBufferSize(),
			&pInputLayout
		));

		// bind vertex layout
		ppD3D.m_pContext->IASetInputLayout(pInputLayout.Get());


		// bind render target
		ppD3D.m_pContext->OMSetRenderTargets(1u, ppD3D.m_pTarget.GetAddressOf(), nullptr);


		// Set primitive topology to triangle list (group of 3 vertices)
		ppD3D.m_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// configure viewport
		D3D11_VIEWPORT vp;
		vp.Width = 800;
		vp.Height = 600;
		vp.MinDepth = 0;
		vp.MaxDepth = 1;
		vp.TopLeftX = 0;
		vp.TopLeftY = 0;
		ppD3D.m_pContext->RSSetViewports(1u, &vp);



		GFX_THROW_INFO_ONLY(ppD3D.m_pContext->Draw((UINT)std::size(vertices), 0u));
	}

}