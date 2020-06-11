#include "hzpch.h"
#include "D3D11Buffer.h"


#define GFX_THROW_INFO(x) x


namespace Hazel {

	/////////////////////////////////////////////////////////////////////////////
	// VertexBuffer /////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////

	D3D11VertexBuffer::D3D11VertexBuffer(uint32_t size)
		: VertexBuffer(GraphicsContext::Get_Active()), _c((D3D11Context&)this->_ctx)
	{
	}

	D3D11VertexBuffer::D3D11VertexBuffer(std::string& tag, uint32_t size)
		: VertexBuffer(GraphicsContext::Get_Active()), _c((D3D11Context&)this->_ctx), m_tag(tag)
	{
	}

	D3D11VertexBuffer::D3D11VertexBuffer(float* vertices, uint32_t size)
		: VertexBuffer(GraphicsContext::Get_Active()), _c((D3D11Context&)this->_ctx)
	{
		//stride((UINT)vbuf.GetLayout().Size()),
		//	tag(tag)
		//{
		//	INFOMAN(gfx);

		D3D11_BUFFER_DESC bd = {};
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.CPUAccessFlags = 0u;
		bd.MiscFlags = 0u;
		bd.ByteWidth = size;
		bd.StructureByteStride = 0;//-------stride: not strictly necessary!!!
		D3D11_SUBRESOURCE_DATA sd = {};
		sd.pSysMem = vertices;
		GFX_THROW_INFO(_c.GetPP().m_pDevice->CreateBuffer(&bd, &sd, &pVertexBuffer));
	}

	D3D11VertexBuffer::D3D11VertexBuffer(std::string& tag, float* vertices, uint32_t size)
		: VertexBuffer(GraphicsContext::Get_Active()), _c((D3D11Context&)this->_ctx), m_tag(tag)
	{		
		//stride((UINT)vbuf.GetLayout().Size()),
		//	tag(tag)
		//{
		//	INFOMAN(gfx);

		D3D11_BUFFER_DESC bd = {};
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.CPUAccessFlags = 0u;
		bd.MiscFlags = 0u;
		bd.ByteWidth = size;
		bd.StructureByteStride = 0;//-------stride: not strictly necessary!!!
		D3D11_SUBRESOURCE_DATA sd = {};
		sd.pSysMem = vertices;
		GFX_THROW_INFO(_c.GetPP().m_pDevice->CreateBuffer(&bd, &sd, &pVertexBuffer));
	}

	D3D11VertexBuffer::~D3D11VertexBuffer()
	{
	}

	void D3D11VertexBuffer::Bind() const
	{
		const UINT stride = 0u;
		const UINT offset = 0u;
		_c.GetPP().m_pContext->IASetVertexBuffers(0u, 1u, pVertexBuffer.GetAddressOf(), &stride, &offset);
	}

	void D3D11VertexBuffer::Unbind() const
	{
	}

	void D3D11VertexBuffer::BindTemp(uint32_t stride) const
	{
		//const UINT stride = stride;
		const UINT offset = 0u;
		_c.GetPP().m_pContext->IASetVertexBuffers(0u, 1u, pVertexBuffer.GetAddressOf(), &((UINT)stride), &offset);

	}

	void D3D11VertexBuffer::SetData(const void* data, uint32_t size)
	{
	}






	/////////////////////////////////////////////////////////////////////////////
	// IndexBuffer //////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////
	Hazel::D3D11IndexBuffer::D3D11IndexBuffer(uint32_t* indices, uint32_t count)
		: IndexBuffer(GraphicsContext::Get_Active()), _c((D3D11Context&)this->_ctx), m_Count(count) 
	{
		Init(indices, count);
	}

	D3D11IndexBuffer::D3D11IndexBuffer(std::string& tag, uint32_t* indices, uint32_t count)
		: IndexBuffer(GraphicsContext::Get_Active()), _c((D3D11Context&)this->_ctx), m_Count(count), m_tag(tag)
	{
		Init(indices, count);
	}

	D3D11IndexBuffer::~D3D11IndexBuffer()
	{
	}

	void D3D11IndexBuffer::Init(uint32_t* indices, uint32_t count)
	{		//INFOMAN(gfx);

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

	void D3D11IndexBuffer::Bind() const
	{
		_c.GetPP().m_pContext->IASetIndexBuffer(pIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0u);
	}

	void D3D11IndexBuffer::Unbind() const
	{
	}

}