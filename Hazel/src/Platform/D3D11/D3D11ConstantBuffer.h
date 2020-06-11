#pragma once

#include "Hazel/Renderer/ConstantBuffer.h"
#include "Platform/D3D11/D3D11Context.h"

#include <d3d11.h>
#include <wrl.h>

#define GFX_THROW_INFO(x) x

namespace Hazel {

	template<typename C>
	class D3D11ConstantBuffer : public ConstanBuffer
	{
	public:
		D3D11ConstantBuffer(const C& consts)
			: ConstanBuffer(GraphicsContext::Get_Active()), _c((D3D11Context&)this->_ctx)
		{
			//INFOMAN(gfx);

			D3D11_BUFFER_DESC cbd;
			cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			cbd.Usage = D3D11_USAGE_DYNAMIC;
			cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			cbd.MiscFlags = 0u;
			cbd.ByteWidth = sizeof(C);
			cbd.StructureByteStride = 0u;

			D3D11_SUBRESOURCE_DATA csd = {};
			csd.pSysMem = &consts;
			GFX_THROW_INFO(_c.GetPP().m_pDevice->CreateBuffer(&cbd, &csd, &m_pConstantBuffer));
		}
		virtual ~D3D11ConstantBuffer() = default;

		virtual void Bind() const override
		{
			if (m_target != 2)
				_c.GetPP().m_pContext->VSSetConstantBuffers(m_slot, 1u, m_pConstantBuffer.GetAddressOf());
			if (m_target != 1)
				_c.GetPP().m_pContext->PSSetConstantBuffers(m_slot, 1u, m_pConstantBuffer.GetAddressOf());
		}

		virtual void SetSlot(uint32_t slot, uint32_t target) // target 0 VS & PS, 1 only VS, 2 only PS
		{
			m_slot = slot;
			m_target = target;
		}

		virtual void Unbind() const override {}
	private:
		uint32_t m_slot = 0;
		uint32_t m_target = 0;
	protected:
		Microsoft::WRL::ComPtr<ID3D11Buffer> m_pConstantBuffer;

	public:
		D3D11Context& _c;

	};

}
