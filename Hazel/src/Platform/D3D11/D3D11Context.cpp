#include "hzpch.h"
#include "Platform/D3D11/D3D11Context.h"

#include "Hazel/Core/Window.h"

#include <d3dcompiler.h>
#include <DirectXMath.h>

namespace wrl = Microsoft::WRL;
namespace dx = DirectX;

#pragma comment(lib,"d3d11.lib")
#pragma comment(lib,"D3DCompiler.lib")

#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective

// Temp 
#include "D3D11Buffer.h"
#include "D3D11Shader.h" // TODO

#define GFX_THROW_INFO(x) x

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

		/////ppD3D.m_pContext->OMSetRenderTargets(0, 0, 0);

		// gain access to texture subresource in swap chain (back buffer)
		wrl::ComPtr<ID3D11Resource> pBackBuffer = nullptr;
		ppD3D.m_pSwap->GetBuffer(0, __uuidof(ID3D11Resource), &pBackBuffer);
		ppD3D.m_pDevice->CreateRenderTargetView(pBackBuffer.Get(), nullptr, &ppD3D.m_pTarget);


		// create depth stensil state
		D3D11_DEPTH_STENCIL_DESC dsDesc = {};
		dsDesc.DepthEnable = TRUE;
		dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
		// a lots of stencil desctipter ...
		wrl::ComPtr<ID3D11DepthStencilState> pDSState;
		GFX_THROW_INFO(ppD3D.m_pDevice->CreateDepthStencilState(&dsDesc, &pDSState));
		// bind depth state
		ppD3D.m_pContext->OMSetDepthStencilState(pDSState.Get(), 1u);

		//----


		//
		// create depth stensil texture
		wrl::ComPtr<ID3D11Texture2D> pDepthStencil;
		D3D11_TEXTURE2D_DESC descDepth = {};
		descDepth.Width = 800u;
		descDepth.Height = 600u;
		descDepth.MipLevels = 1u;
		descDepth.ArraySize = 1u;
		descDepth.Format = DXGI_FORMAT_D32_FLOAT; // special format for depth
		descDepth.SampleDesc.Count = 1u;
		descDepth.SampleDesc.Quality = 0u;
		descDepth.Usage = D3D11_USAGE_DEFAULT;
		descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		GFX_THROW_INFO(ppD3D.m_pDevice->CreateTexture2D(&descDepth, nullptr, &pDepthStencil));

		// create view of depth stensil texture
		D3D11_DEPTH_STENCIL_VIEW_DESC descDSV = {};
		descDSV.Format = DXGI_FORMAT_D32_FLOAT;
		descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		descDSV.Texture2D.MipSlice = 0u;
		GFX_THROW_INFO(ppD3D.m_pDevice->CreateDepthStencilView(
			pDepthStencil.Get(), &descDSV, &ppD3D.m_pDSV
		));

		// bind depth stensil view to OM
		ppD3D.m_pContext->OMSetRenderTargets(1u, ppD3D.m_pTarget.GetAddressOf(), ppD3D.m_pDSV.Get());
	}

	void D3D11Context::SwapBuffers()
	{
		ppD3D.m_pSwap->Present(1u, 0u);
	}

	void D3D11Context::ClearBuffer_impl(float red, float green, float blue)
	{
		const float color[] = { red,green,blue,1.0f };
		ppD3D.m_pContext->ClearRenderTargetView(ppD3D.m_pTarget.Get(), color);
		ppD3D.m_pContext->ClearDepthStencilView(ppD3D.m_pDSV.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0u);

	}

	API D3D11Context::MakeCurrent()
	{
		if (_s_active.get() != this)
		{
			_s_active = Resolve(m_window);
		}
		return GetAPI();

	}

	void D3D11Context::DrawTriangle_impl(float angle)
	{
		#define GFX_THROW_INFO(x) x
		#define GFX_THROW_INFO_ONLY(x) x

		//HRESULT hr;

		struct Vertex
		{
			struct
			{
				float x;
				float y;
				float z;
			} pos;
		};

		// create vertex buffer (1 2d triangle at center of screen)
		const Vertex vertices[] =
		{
		{ -1.0f,-1.0f,-1.0f	 },
		{ 1.0f,-1.0f,-1.0f	 },
		{ -1.0f,1.0f,-1.0f	 },
		{ 1.0f,1.0f,-1.0f	  },
		{ -1.0f,-1.0f,1.0f	 },
		{ 1.0f,-1.0f,1.0f	  },
		{ -1.0f,1.0f,1.0f	 },
		{ 1.0f,1.0f,1.0f	 },
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


	 uint32_t indices[] =
		{
		0,2,1, 2,3,1,
		1,3,5, 3,7,5,
		2,6,3, 3,6,7,
		4,5,7, 4,7,6,
		0,4,2, 2,4,6,
		0,1,4, 1,5,4
		};

		//wrl::ComPtr<ID3D11Buffer> pIndexBuffer;
		//D3D11_BUFFER_DESC ibd = {};
		//ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		//ibd.Usage = D3D11_USAGE_DEFAULT;
		//ibd.CPUAccessFlags = 0u;
		//ibd.MiscFlags = 0u;
		//ibd.ByteWidth = sizeof(indices);
		//ibd.StructureByteStride = sizeof(uint32_t);
		//D3D11_SUBRESOURCE_DATA isd = {};
		//isd.pSysMem = indices;
		//GFX_THROW_INFO(ppD3D.m_pDevice->CreateBuffer(&ibd, &isd, &pIndexBuffer));
		//
		//// Bind vertex buffer to pipeline
		//ppD3D.m_pContext->IASetIndexBuffer(pIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0u);

		Ref<IndexBuffer> ibuff = IndexBuffer::Create(indices, int(sizeof(indices) / sizeof(float)));
		ibuff->Bind();




		// create constant buffer for transformation matrix
		struct ConstantBuffer
		{
			//struct
			//{
			//	float element[4][4];
			//} transformation;
			//glm::mat4 transform;
			dx::XMMATRIX transformation;

		};
		const ConstantBuffer cb = // This is a CCW rotation not a transposd!!!!!
		{
			//{
			//	(3.0f / 4.0f) * std::cos(angle),	std::sin(angle),	0.0f,	0.0f,
			//	(3.0f / 4.0f) * -std::sin(angle),	std::cos(angle),	0.0f,	0.0f,
			//	0.0f,								0.0f,				1.0f,	0.0f,
			//	0.0f,								0.0f,				0.0f,	1.0f,
			//}
				//glm::perspective()

				////glm::transpose(
				//glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 3))
				////glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0, 0, 1)) 
				//////glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, .5f, 100.f)
				////glm::mat4(1.0f)


				//glm::transpose(
				//glm::perspectiveFovLH(glm::radians(60.0f), 800.0f, 600.0f, .5f, 10.f) *
				//glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 4)) *
				//glm::rotate(glm::mat4(1.0f), angle, glm::vec3(1, 0, 0))*
				//glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0, 0, 1))
				//)

				//dx::XMMatrixTranspose(
				//dx::XMMatrixTranslation(0,0.0f,0 + 4.0f) *
				//dx::XMMatrixPerspectiveLH(1.0f, 3.0f / 4.0f, 0.5f, 10.0f)
				//)

				dx::XMMatrixTranspose(
				dx::XMMatrixRotationZ(angle)*
				dx::XMMatrixRotationX(angle)*
				dx::XMMatrixTranslation(0,0.0f,0 + 4.0f)*
				dx::XMMatrixPerspectiveLH(1.0f, 3.0f / 4.0f, 0.5f, 10.0f)
				)
		};


		wrl::ComPtr<ID3D11Buffer> pConstantBuffer;
		D3D11_BUFFER_DESC cbd;
		cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cbd.Usage = D3D11_USAGE_DYNAMIC;
		cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		cbd.MiscFlags = 0u;
		cbd.ByteWidth = sizeof(cb);
		cbd.StructureByteStride = 0u;
		D3D11_SUBRESOURCE_DATA csd = {};
		csd.pSysMem = &cb;
		GFX_THROW_INFO(ppD3D.m_pDevice->CreateBuffer(&cbd, &csd, &pConstantBuffer));

		// bind constant buffer to vertex shader
		ppD3D.m_pContext->VSSetConstantBuffers(0u, 1u, pConstantBuffer.GetAddressOf());


		// lookup table for cube face colors
		struct ConstantBuffer2
		{
			struct
			{
				float r;
				float g;
				float b;
				float a;
			} face_colors[6];
		};
		const ConstantBuffer2 cb2 =
		{
			{
				{1.0f,0.0f,1.0f},
				{1.0f,0.0f,0.0f},
				{0.0f,1.0f,0.0f},
				{0.0f,0.0f,1.0f},
				{1.0f,1.0f,0.0f},
				{0.0f,1.0f,1.0f},
			}
		};
		wrl::ComPtr<ID3D11Buffer> pConstantBuffer2;
		D3D11_BUFFER_DESC cbd2;
		cbd2.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cbd2.Usage = D3D11_USAGE_DEFAULT;
		cbd2.CPUAccessFlags = 0u;
		cbd2.MiscFlags = 0u;
		cbd2.ByteWidth = sizeof(cb2);
		cbd2.StructureByteStride = 0u;
		D3D11_SUBRESOURCE_DATA csd2 = {};
		csd2.pSysMem = &cb2;
		GFX_THROW_INFO(ppD3D.m_pDevice->CreateBuffer(&cbd2, &csd2, &pConstantBuffer2));

		// bind constant buffer to pixel shader
		ppD3D.m_pContext->PSSetConstantBuffers(0u, 1u, pConstantBuffer2.GetAddressOf());




		//create a pixel shader
		//create a vertex shader
		Ref<Shader> shader = (Shader::Create("assets/shaders/D3D/FlatColor.hlsl"));
		shader->Bind();
		auto pBlob = shader->GetpShaderBytecode();



		// input (vertex) layout (2d position only)
		wrl::ComPtr<ID3D11InputLayout> pInputLayout;
		const D3D11_INPUT_ELEMENT_DESC ied[] =
		{
		{ "Position",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 },
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
		//ppD3D.m_pContext->OMSetRenderTargets(1u, ppD3D.m_pTarget.GetAddressOf(), nullptr);


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



		//GFX_THROW_INFO_ONLY(ppD3D.m_pContext->Draw((UINT)std::size(vertices), 0u));
		ppD3D.m_pContext->DrawIndexed((UINT)std::size(indices), 0, 0);
	}

}