#include "hzpch.h"
#include "D3D11Buffer.h"


#define GFX_THROW_INFO(x) x


namespace Hazel {

	Hazel::D3D11IndexBuffer::D3D11IndexBuffer(uint32_t* indices, uint32_t count)
		: m_Count(count), _c((D3D11Context&)GraphicsContext::Get_Active()) // possible to store as a normal context and add 
	{
		//INFOMAN(gfx);

		D3D11_BUFFER_DESC ibd = {};
		ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		ibd.Usage = D3D11_USAGE_DEFAULT;
		ibd.CPUAccessFlags = 0u;
		ibd.MiscFlags = 0u;
		ibd.ByteWidth = count * sizeof(uint32_t);// UINT(count * sizeof(unsigned short));
		ibd.StructureByteStride = sizeof(uint32_t); //sizeof(unsigned short);
		D3D11_SUBRESOURCE_DATA isd = {};
		isd.pSysMem = indices;
		GFX_THROW_INFO(_c.GetPP().m_pDevice->CreateBuffer(&ibd, &isd, &pIndexBuffer));
	}

	D3D11IndexBuffer::~D3D11IndexBuffer()
	{
	}

	void D3D11IndexBuffer::Bind() const
	{
		_c.GetPP().m_pContext->IASetIndexBuffer(pIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0u);
	}

	void D3D11IndexBuffer::Unbind() const
	{
	}

}