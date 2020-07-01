#include "hzpch.h"
#include "D3D11PipelineSpec.h"

namespace wrl = Microsoft::WRL;

namespace Hazel {

	D3D11PipelineSpec::D3D11PipelineSpec(const PipelineCreateInfo& createInfo)
		: PipelineSpecification(GraphicsContext::Get_Active()), _c((D3D11Context&)this->_ctx)
	{
		m_spec = createInfo;
		//m_Pipeline = CreateRef<Pipeline>(_c);
		//ReCreate();
	}

	D3D11PipelineSpec::~D3D11PipelineSpec()
	{
	}

	void D3D11PipelineSpec::Bind() const
	{
		m_spec.shader->Bind(); // Nothing in vulkan
		//std::dynamic_pointer_cast<OpenGLVertexLayout>(m_spec.vertexLayout)->Bind();
		//m_Pipeline->Bind();
		
		//auto x = m_spec.vertexLayout->GetD3DLayout();

		auto pBlob = m_spec.shader->GetpShaderBytecode();
		// input (vertex) layout (2d position only)
		wrl::ComPtr<ID3D11InputLayout> pInputLayout;
		const D3D11_INPUT_ELEMENT_DESC ied[] =
		{
		//{ "Position",0,DXGI_FORMAT_R32G32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 },
		//{ "Color",0,DXGI_FORMAT_R32G32B32_FLOAT,0,8,D3D11_INPUT_PER_VERTEX_DATA,0 }

		{ "TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 },
		{ "TEXCOORD",1,DXGI_FORMAT_R32G32B32_FLOAT,0,8,D3D11_INPUT_PER_VERTEX_DATA,0 }

		};
		_c.GetPP().m_pDevice->CreateInputLayout(
			ied, (UINT)std::size(ied),
			pBlob->GetBufferPointer(),
			pBlob->GetBufferSize(),
			&pInputLayout
		);

		// bind vertex layout
		_c.GetPP().m_pContext->IASetInputLayout(pInputLayout.Get());

		_c.GetPP().m_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		//// configure viewport
		D3D11_VIEWPORT vp;
		vp.Width = 800;
		vp.Height = 600;
		vp.MinDepth = 0;
		vp.MaxDepth = 1;
		vp.TopLeftX = 0;
		vp.TopLeftY = 0;
		_c.GetPP().m_pContext->RSSetViewports(1u, &vp);


	}

	void D3D11PipelineSpec::ReCreate() const
	{
	}

}