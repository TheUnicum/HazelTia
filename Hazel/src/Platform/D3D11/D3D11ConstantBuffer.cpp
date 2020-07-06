#include "hzpch.h"
#include "Platform/D3D11/D3D11ConstantBuffer.h"

namespace Hazel {

	D3D11ConstantBuffer::D3D11ConstantBuffer(uint32_t size, const void* data)
		: ConstantBuffer(GraphicsContext::Get_Active()), _c((D3D11Context&)this->_ctx), m_size(size)
	{
		//INFOMAN(gfx);

		D3D11_BUFFER_DESC cbd;
		cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cbd.Usage = D3D11_USAGE_DYNAMIC;
		cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		cbd.MiscFlags = 0u;
		cbd.ByteWidth = m_size;
		cbd.StructureByteStride = 0u;

		D3D11_SUBRESOURCE_DATA csd = {};
		csd.pSysMem = data;
		GFX_THROW_INFO(_c.GetPP().m_pDevice->CreateBuffer(&cbd, &csd, &m_pConstantBuffer));
	}

	void D3D11ConstantBuffer::Bind() const
	{
		if (m_target != 2)
			_c.GetPP().m_pContext->VSSetConstantBuffers(m_slot, 1u, m_pConstantBuffer.GetAddressOf());
		if (m_target != 1)
			_c.GetPP().m_pContext->PSSetConstantBuffers(m_slot, 1u, m_pConstantBuffer.GetAddressOf());
	}

	void D3D11ConstantBuffer::Update(const void* data, uint32_t size)
	{
		//HZ_CORE_ASSERT(false, "NOT IMPLEMETED YET")
		//INFOMAN(gfx);

		D3D11_MAPPED_SUBRESOURCE msr;

		_c.GetPP().m_pContext->Map(
			m_pConstantBuffer.Get(), 0u,
			D3D11_MAP_WRITE_DISCARD, 0u,
			&msr
		);
		memcpy(msr.pData, data, size);
		_c.GetPP().m_pContext->Unmap(m_pConstantBuffer.Get(), 0u);
	}

	void D3D11ConstantBuffer::SetSlot(uint32_t slot, uint32_t target)
	{
		m_slot = slot;
		m_target = target;
	}
}