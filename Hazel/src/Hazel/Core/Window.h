#pragma once

#include "hzpch.h"

#include "Hazel/Core/Base.h"
#include "Hazel/Events/Event.h"

#include "Hazel/Renderer/Renderer.h"

namespace Hazel {

	struct WindowProps
	{
		std::string Title;
		unsigned int Width;
		unsigned int Height;
		RendererAPI::API Api;

		WindowProps(const std::string& title = "Hazel Engine",
			unsigned int width = 1280,
			unsigned int height = 720,
			RendererAPI::API api = RendererAPI::API::OpenGL)
			: Title(title), Width(width), Height(height), Api(api)
		{
		}
	};

	// Interface representation a desktop system based Window
	class Window
	{
	public:
		using EventCallbackFn = std::function<void(Event&)>;

		virtual ~Window() = default;

		virtual void OnUpdate() = 0;

		virtual unsigned int GetWidth() const = 0;
		virtual unsigned int GetHeight() const = 0;
		virtual RendererAPI::API GetAPI() const = 0;


		// Window attributes
		virtual void SetEventCallback(const EventCallbackFn& callback) = 0;
		virtual void SetVSync(bool enabled) = 0;
		virtual bool IsVSync() const = 0;

		virtual void* GetNativeWindow() const = 0;

		static Scope<Window> Create(const WindowProps& props = WindowProps());
	};

}
