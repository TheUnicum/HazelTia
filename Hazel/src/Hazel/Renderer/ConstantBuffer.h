#pragma once
#include "Hazel/Renderer/Bindable.h"
#include "Hazel/Renderer/GraphicsContext.h"

namespace Hazel {

	class ConstantBuffer : public Bindable
	{
	public:
		ConstantBuffer(GraphicsContext& ctx) : Bindable(ctx) {}
		virtual ~ConstantBuffer() = default;

		virtual void Bind() const override = 0;
		virtual void Unbind() const override = 0;

		virtual void Update(const void* data, uint32_t size = 0) = 0;
		virtual void SetSlot(uint32_t slot, uint32_t target) = 0; // target 0 VS & PS, 1 only VS, 2 only PS
	public:
		static Ref<ConstantBuffer> Create(uint32_t size, const void* data = nullptr);
		static Ref<ConstantBuffer> Create(GraphicsContext& ctx, uint32_t size, const void* data = nullptr);
	};

}
