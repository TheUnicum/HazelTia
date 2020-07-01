#include "hzpch.h"
#include "Hazel/Renderer/GraphicsContext.h"

#include "Hazel/Core/Window.h"

#include "Platform/OpenGL/OpenGLContext.h"
#include "Platform/D3D11/D3D11Context.h"
#include "Platform/Vulkan/VulkanContext.h"


namespace Hazel {

	Ref<GraphicsContext> GraphicsContext::_s_active = nullptr;
	std::unordered_map<std::string, Ref<GraphicsContext>> GraphicsContext::_s_map_context;

	std::string GraphicsContext::GenerateUID(API api, Window& window)
	{
		return "#Api" + std::to_string((int)api) + "@:" + std::to_string(reinterpret_cast<uintptr_t>(&window));
	}

	GraphicsContext& GraphicsContext::Get_Active()
	{
		HZ_CORE_ASSERT(_s_active, "Graphiccs Context not initialized!");
		return *_s_active;
	}

	API GraphicsContext::Get_API_Active()
	{
		if (_s_active)
			return _s_active->GetAPI();
		else
			return API::OpenGL; // Default value if not initialized!
	}

	// Classes Factory
	Ref<GraphicsContext> GraphicsContext::Create(Window& window) { return Create(window.GetAPI(), window); }
	Ref<GraphicsContext> GraphicsContext::Create(API api, Window& window)
	{
		switch (api)
		{
			case API::None:    HZ_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
			case API::OpenGL:  return CreateRef<OpenGLContext>(window);
			case API::D3D11:   return CreateRef<D3D11Context>(window);
			case API::Vulkan:  return CreateRef<VulkanContext>(window);
		}

		HZ_CORE_ASSERT(false, "Unknow RendererAPI!");
		return nullptr;
	}

	Ref<GraphicsContext> GraphicsContext::Resolve(Window& window, bool make_new_entity) { return Resolve(window.GetAPI(), window, make_new_entity); }
	Ref<GraphicsContext> GraphicsContext::Resolve(API api, Window& window, bool make_new_entity) // TODO: may be removed!
	{
		const auto key = GraphicsContext::GenerateUID(api, window);
		const auto internal_context = _s_map_context.find(key);
		if (internal_context == _s_map_context.end())
		{
			Ref<GraphicsContext> new_context = Create(api, window);
			_s_map_context[key] = new_context;
			return new_context;
		}
		else
		{
			if (make_new_entity)
				HZ_CORE_ASSERT(false, "CANT have duplicate of this CONTEXT!");
			return internal_context->second;
		}
	}

	void GraphicsContext::Release(API api, Window& window)
	{
		//const auto key = GraphicsContext::GenerateUID(api, window);
		//const auto internal_context = _s_map_context.find(key);
		//if (internal_context != _s_map_context.end())
		//{
		//	_s_map_context.erase(key);
		//}
		//else
		//{
		//	HZ_CORE_ASSERT("Release GraphicsContext not possible <{}>", key);
		//}
	}

}
