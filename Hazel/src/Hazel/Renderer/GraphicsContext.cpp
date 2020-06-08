#include "hzpch.h"
#include "Hazel/Renderer/GraphicsContext.h"

#include "Hazel/Renderer/Renderer.h"
#include "Platform/OpenGL/OpenGLContext.h"
#include "Platform/D3D11/D3D11Context.h"

#include <typeinfo>

namespace Hazel {

	Ref<GraphicsContext> GraphicsContext::_s_active;
	std::unordered_map<std::string, Ref<GraphicsContext>> GraphicsContext::_s_map_context;

	std::string GraphicsContext::GenerateUID(RendererAPI::API api, Window& window)
	{
		return "#Api-" + std::to_string((int)api) + "@:" + std::to_string(reinterpret_cast<uintptr_t>(&window));
	}

	RendererAPI::API GraphicsContext::Get_API_Active()
	{
		if (_s_active)
			return _s_active->Get_API_Active();
		else
			return RendererAPI::API::OpenGL;
	}

	// Classes Factory
	Ref<GraphicsContext> GraphicsContext::Create(Window& window) { return Create(Renderer::GetAPI(), window); }
	Ref<GraphicsContext> GraphicsContext::Create(RendererAPI::API api, Window& window)
	{
		switch (api)
		{
		case RendererAPI::API::None:    HZ_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case RendererAPI::API::OpenGL:  return CreateRef<OpenGLContext>(window);
		case RendererAPI::API::D3D11:   return CreateRef<D3D11Context>(window);
		}

		HZ_CORE_ASSERT(false, "Unknow RendererAPI!");
		return nullptr;
	}

	Ref<GraphicsContext> GraphicsContext::Resolve(Window& window, bool make_new_entity) { return Resolve(window.GetAPI(), window, make_new_entity); }
	Ref<GraphicsContext> GraphicsContext::Resolve(RendererAPI::API api, Window& window, bool make_new_entity) // TODO: may be removed!
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

	void GraphicsContext::Release(RendererAPI::API api, Window& window)
	{
		const auto key = GraphicsContext::GenerateUID(api, window);
		const auto internal_context = _s_map_context.find(key);
		if (internal_context != _s_map_context.end())
		{
			_s_map_context.erase(key);
		}
		else
		{
			HZ_CORE_ASSERT("Release GraphicsContext not possible <{}>", key);
		}
	}



}
