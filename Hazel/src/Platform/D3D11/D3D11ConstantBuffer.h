#pragma once
#include "Hazel/Renderer/ConstantBuffer.h"
#include "Platform/D3D11/D3D11Context.h"

#define GFX_THROW_INFO(x) x

namespace Hazel {

	class D3D11ConstantBuffer : public ConstantBuffer
	{
	public:
		D3D11ConstantBuffer(uint32_t size, const void* data);
		virtual ~D3D11ConstantBuffer() = default;

		virtual void Bind() const override;
		virtual void Unbind() const override {};

		virtual void Update(const void* data, uint32_t size = 0) override;
		virtual void SetSlot(uint32_t slot, uint32_t target) override; // target 0 VS & PS, 1 only VS, 2 only PS
	private:
		uint32_t m_size = 0;
		uint32_t m_slot = 0;
		uint32_t m_target = 0;
	public:
		Microsoft::WRL::ComPtr<ID3D11Buffer> m_pConstantBuffer;
		D3D11Context& _c;

	};

}
