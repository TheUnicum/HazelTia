#include "hzpch.h"
#include "Hazel/Renderer/PipelineSpecification.h"

//#include "Platform/OpenGL/OpenGLPipelineSpec.h"
//#include "Platform/D3D11/D3D11PipelineSpec"
#include "Platform/VulkanC/VulkanPipelineSpec.h"

namespace Hazel {

    Ref<PipelineSpecification> PipelineSpecification::Create(const PipelineCreateInfo& createInfo) { return Create((GraphicsContext&)GraphicsContext::Get_Active(), createInfo); }
    Ref<PipelineSpecification> PipelineSpecification::Create(GraphicsContext& ctx, const PipelineCreateInfo& createInfo)
    {
		switch (ctx.GetAPI())
		{
			case API::None:    HZ_CORE_ASSERT(false, "Rendererctx::None is currently not supported!"); return nullptr;
			//case API::OpenGL:  return CreateRef<OpenGLShader>(filepath);
			//case API::D3D11:   return CreateRef<D3D11Shader>(filepath);
			case API::Vulkan:  return CreateRef<VulkanPipelineSpec>(createInfo);
		}
		HZ_CORE_ASSERT(false, "Unknow Rendererctx!");
		return nullptr;
    }

}