#pragma once

#include "Renderer.h"

namespace Hazel {

	class GraphicsContext
	{
	public:
		virtual ~GraphicsContext() = default;

		virtual void Init() = 0;
		virtual void SwapBuffers() = 0;

		virtual RendererAPI::API GetAPI() = 0;
		virtual RendererAPI::API MakeCurrent() = 0;

		// static
		static std::string GenerateUID(RendererAPI::API api, void* window);
		static Ref<GraphicsContext> Get_Active() { return _s_active; }
		static RendererAPI::API Get_API_Active();

		// Classes Factory
		static Ref<GraphicsContext> Create(void* window);
		static Ref<GraphicsContext> Create(RendererAPI::API api, void* window);

		static Ref<GraphicsContext> Resolve(void* window);
		static Ref<GraphicsContext> Resolve(RendererAPI::API api, void* window);
		static void Release(RendererAPI::API api, void* window);
	public:
		static Ref<GraphicsContext> _s_active;
		static std::unordered_map<std::string, Ref<GraphicsContext>> _s_map_context;

	};

}
