#include "hzpch.h"
#include "Hazel/Renderer/Texture.h"

#include "Platform/OpenGL/OpenGLTexture.h"
#include "Platform/D3D11/D3D11Texture.h"
#include "Platform/Vulkan/VulkanTexture.h"

namespace Hazel {

	Ref<Texture2D> Texture2D::Create(uint32_t width, uint32_t height) { return Create((GraphicsContext&)GraphicsContext::Get_Active(), width, height); }
	Ref<Texture2D> Texture2D::Create(GraphicsContext& ctx, uint32_t width, uint32_t height)
	{
		switch (ctx.GetAPI())
		{
			case API::None:    HZ_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
			case API::OpenGL:  return CreateRef<OpenGLTexture2D>(width, height);
			case API::D3D11:   return CreateRef<D3D11Texture2D>(width, height);
		}
		HZ_CORE_ASSERT(false, "Unknow RendererAPI!");
		return Ref<Texture2D>();
	}


	Ref<Texture2D> Texture2D::Create(const std::string& path) { return Create((GraphicsContext&)GraphicsContext::Get_Active(), path); }
	Ref<Texture2D> Texture2D::Create(GraphicsContext& ctx, const std::string& path)
	{
		switch (ctx.GetAPI())
		{
			case API::None:    HZ_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
			case API::OpenGL:  return CreateRef<OpenGLTexture2D>(path);
			case API::D3D11:   return CreateRef<D3D11Texture2D>(path);
			case API::Vulkan:   return CreateRef<VulkanTexture2D>(path);
		}
		HZ_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

}
