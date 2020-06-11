#pragma once

#include "Hazel/Core/Base.h"

#include "Hazel/Renderer/Bindable.h"
#include "Hazel/Renderer/GraphicsContext.h"

#include "Platform/OpenGL/OpenGLConstantBuffer.h"
#include "Platform/D3D11/D3D11ConstantBuffer.h"


namespace Hazel {

	class ConstanBuffer : public Bindable
	{
	public:
		ConstanBuffer(GraphicsContext& ctx) : Bindable(ctx) {}
		virtual ~ConstanBuffer() = default;

		virtual void SetSlot(uint32_t slot, uint32_t target) = 0; // target 0 VS & PS, 1 only VS, 2 only PS
	private:

	public:
		// Classes Factory
		template<typename C>
		static Ref<ConstanBuffer> Create(const C& consts)
		{
			GraphicsContext& c = (GraphicsContext&)GraphicsContext::Get_Active();
			switch (c.GetAPI())
			{
				case API::None:    HZ_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
				case API::OpenGL:  return CreateRef<OpenGLConstantBuffer<C>>(consts);
				case API::D3D11:   return CreateRef<D3D11ConstantBuffer<C>>(consts);
			}
			HZ_CORE_ASSERT(false, "Unknow RendererAPI!");
			return nullptr;
		}
	};


}
