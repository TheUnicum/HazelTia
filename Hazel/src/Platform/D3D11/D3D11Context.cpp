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
#include "D3D11Texture.h" // TODO
#include "D3D11ConstantBuffer.h"
// Test
#include "Hazel/VetexGeometryFactory/Cube.h"
#include "Hazel/VetexGeometryFactory/Cone.h"
#include "Hazel/VetexGeometryFactory/Prism.h"
#include "Hazel/VetexGeometryFactory/Plane.h"
#include "Hazel/VetexGeometryFactory/Sphere.h"
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective

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

		IsWindow(*m_windowHandle);

		RECT rect;
		GetClientRect(*m_windowHandle, &rect);


		DXGI_SWAP_CHAIN_DESC sd = {};
		sd.BufferDesc.Width = 0;
		sd.BufferDesc.Height = 0;
		sd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		sd.BufferDesc.RefreshRate.Numerator = 0;
		sd.BufferDesc.RefreshRate.Denominator = 1;
		sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		sd.SampleDesc.Count = 1;
		sd.SampleDesc.Quality = 0;
		sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		sd.BufferCount = 1;
		sd.OutputWindow = (HWND)*m_windowHandle;//  hWnd; //(HWND)696969;//
		sd.Windowed = TRUE;
		sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;// DXGI_SWAP_EFFECT_DISCARD;
		sd.Flags = 0;

		UINT swapCreateFlags = 0u;
		//#ifdef HZ_DEBUG
		swapCreateFlags |= D3D11_CREATE_DEVICE_DEBUG;
		//#endif


		// create device and front/back buffers, and swap chain and rendering context
		HRESULT hz = D3D11CreateDeviceAndSwapChain(
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

		HZ_CORE_ASSERT(hz == S_OK, "Failed {} to create Device and SwapChain!", hz);
		//std::cout << c << std::endl;


		/////ppD3D.m_pContext->OMSetRenderTargets(0, 0, 0);

		// gain access to texture subresource in swap chain (back buffer)
		wrl::ComPtr<ID3D11Texture2D> pBackBuffer = nullptr;
		auto hr = ppD3D.m_pSwap->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);

		//wrl::ComPtr<ID3D11Resource> pBackBuffer = nullptr;
		//auto hr = ppD3D.m_pSwap->GetBuffer(0, __uuidof(ID3D11Resource), (LPVOID*)&pBackBuffer);
		//ppD3D.m_pSwap->GetBuffer(0, __uuidof(ID3D11Texture2D), &pBackBuffer);
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


		// Change culling Mode
		D3D11_RASTERIZER_DESC descRast = {};
		descRast.FillMode = D3D11_FILL_SOLID;
		//descRast.CullMode = D3D11_CULL_FRONT;
		descRast.CullMode = D3D11_CULL_BACK;
		descRast.FrontCounterClockwise = true;

		ppD3D.m_pDevice->CreateRasterizerState(&descRast, &ppD3D.m_pRS);
		ppD3D.m_pContext->RSSetState(ppD3D.m_pRS.Get());
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


	void D3D11Context::CmdClear_impl()
	{
		ClearBuffer_impl(.2f, 0.1f, 0.1f);

		//HZ_CORE_ASSERT(false, "Function Still NOT implemented!");
	}

	void D3D11Context::CmdDrawArrays_impl(uint32_t vertexCount, uint32_t offset)
	{
		HZ_CORE_ASSERT(false, "Function Still NOT implemented!");
	}

	void D3D11Context::CmdDrawArraysInstanced_impl(uint32_t vertexCount, uint32_t instanceCount)
	{
		HZ_CORE_ASSERT(false, "Function Still NOT implemented!");
	}

	void D3D11Context::CmdDrawIndexted_impl(uint32_t indexCount, uint32_t offset)
	{
		ppD3D.m_pContext->DrawIndexed((UINT)(indexCount), offset, 0);
	}

	void D3D11Context::CmdDrawIndextedInstanced_impl(uint32_t indexCount, uint32_t instanceCount)
	{
		HZ_CORE_ASSERT(false, "Function Still NOT implemented!");
	}


	void D3D11Context::DrawTriangle_impl(float angle)
	{
		#define GFX_THROW_INFO(x) x
		#define GFX_THROW_INFO_ONLY(x) x
		ClearBuffer_impl(.1f, 0.1f, 0.1f);

		// generator
		struct VertexPos
		{
			glm::vec3 pos;
			glm::vec2 tex;
		};


		//auto c = Prism::MakeTesselated<VertexPos>(24);
		//auto c = Sphere::Make<VertexPos>();
		auto c = Plane::Make<VertexPos>();
		c.Transform(glm::scale(glm::mat4(1.0f), glm::vec3(3.0f, 3.0f, 3.0f)));

		//c.Transform(glm::rotate(glm::mat4(1.0f), PI / 2, glm::vec3(1.0f, 0.0f, .0f)));
		Ref<VertexBuffer> vb = VertexBuffer::Create((float*)&c.vertices[0], uint32_t(sizeof(VertexPos) * c.vertices.size()));
		vb->BindTemp(sizeof(VertexPos));


		Ref<IndexBuffer> ibuff = IndexBuffer::Create(&c.indices[0], (uint32_t)c.indices.size());
		ibuff->Bind();

		// create constant buffer for transformation matrix
		struct ConstantBuffer
		{
			dx::XMMATRIX transformation;

		};
		const ConstantBuffer cb = // This is a CCW rotation not a transposd!!!!!
		{
				dx::XMMatrixTranspose(
				dx::XMMatrixRotationZ(angle) *
				dx::XMMatrixRotationX(angle) *
				dx::XMMatrixTranslation(0,0.0f,0 + 4.0f) *
				dx::XMMatrixPerspectiveLH(1.0f, 3.0f / 4.0f, 0.5f, 10.0f)
				)
		};

		Ref<ConstanBuffer> cBuff = ConstanBuffer::Create(cb);
		cBuff->SetSlot(0, 1);
		cBuff->Bind();


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
		Ref<ConstanBuffer> cBuff2 = ConstanBuffer::Create(cb2);
		cBuff2->SetSlot(0, 2);
		cBuff2->Bind();



		//Ref<Shader> shader = (Shader::Resolve("assets/shaders/D3D/FlatColor.hlsl"));
		Ref<Shader> shader = (Shader::Resolve("assets/shaders/D3D/Texture.hlsl"));
		auto pBlob = shader->GetpShaderBytecode();
		shader->Bind();


		// input (vertex) layout (2d position only)
		wrl::ComPtr<ID3D11InputLayout> pInputLayout;
		const D3D11_INPUT_ELEMENT_DESC ied[] =
		{
		{ "Position",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 },
		{ "TexCoord",0,DXGI_FORMAT_R32G32_FLOAT,0,12,D3D11_INPUT_PER_VERTEX_DATA,0 },

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



		// Texture
		Ref<Texture2D> texture = Texture2D::Create("assets/textures/Checkerboard.png");
		texture->Bind(0);

		//GFX_THROW_INFO_ONLY(ppD3D.m_pContext->Draw((UINT)std::size(vertices), 0u));
		//ppD3D.m_pContext->DrawIndexed((UINT)std::size(indices), 0, 0);
		ppD3D.m_pContext->DrawIndexed((UINT)(c.indices.size()), 0, 0);
	}

	void D3D11Context::DrawTriangle_impl2(float angle)
	{
		#define GFX_THROW_INFO(x) x
		#define GFX_THROW_INFO_ONLY(x) x
		ClearBuffer_impl(.1f, 0.1f, 0.1f);

		// generator
		struct VertexPos
		{
			glm::vec2 pos;
			//glm::vec2 tex;
		};
		const std::vector<VertexPos> vertices = {
			{{ 0.0f,   0.5f}},//, {0.0, 0.0} },
			{{ 0.5f,  -0.5f}},//, {0.0, 0.0} },
			{{-0.5f,  -0.5f}}//, { 0.0, 0.0 }}
		};
		const std::vector<uint32_t> indices =
		{
				0, 1, 2
		};

		//c.Transform(glm::rotate(glm::mat4(1.0f), PI / 2, glm::vec3(1.0f, 0.0f, .0f)));
		Ref<VertexBuffer> vb = VertexBuffer::Create((float*)&vertices[0], uint32_t(sizeof(VertexPos) * vertices.size()));
		vb->BindTemp(sizeof(VertexPos));

		Ref<IndexBuffer> ibuff = IndexBuffer::Create((uint32_t*)&indices[0], (uint32_t)indices.size());
		ibuff->Bind();


		Ref<Shader> shader = (Shader::Resolve("assets/shaders/D3D/Mattia.hlsl"));
		auto pBlob = shader->GetpShaderBytecode();
		shader->Bind();


		// input (vertex) layout (2d position only)
		wrl::ComPtr<ID3D11InputLayout> pInputLayout;
		const D3D11_INPUT_ELEMENT_DESC ied[] =
		{
		{ "Position",0,DXGI_FORMAT_R32G32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 },
		//{ "TexCoord",0,DXGI_FORMAT_R32G32_FLOAT,0,12,D3D11_INPUT_PER_VERTEX_DATA,0 },

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

		ppD3D.m_pContext->DrawIndexed((UINT)(indices.size()), 0, 0);
	}


}