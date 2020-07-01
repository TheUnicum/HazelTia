#include "hzpch.h"
#include "Hazel/Renderer/ConstantBuffer.h"

#include "Platform/OpenGL/OpenGLConstantBuffer.h"
#include "Platform/D3D11/D3D11ConstantBuffer.h"
#include "Platform/Vulkan/VulkanConstantBuffer.h"

namespace Hazel {

	Ref<ConstantBuffer> ConstantBuffer::Create(uint32_t size, const void* data) { return Create((GraphicsContext&)GraphicsContext::Get_Active(), size, data); }
	Ref<ConstantBuffer> ConstantBuffer::Create(GraphicsContext& ctx, uint32_t size, const void* data)
	{
		switch (ctx.GetAPI())
		{
			case API::None:    HZ_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
			case API::OpenGL:  return CreateRef<OpenGLConstantBuffer>(size, data);
			case API::D3D11:   return CreateRef<D3D11ConstantBuffer>(size, data);
			case API::Vulkan:  return CreateRef<VulkanConstantBuffer>(size, data);
		}
		HZ_CORE_ASSERT(false, "Unknow RendererAPI!");
		return nullptr;
		return Ref<ConstantBuffer>();
	}

}
