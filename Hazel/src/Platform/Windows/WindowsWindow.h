#pragma once

#include "Hazel/Core/Window.h"
#include "Hazel/Renderer/GraphicsContext.h"

#include <GLFW/glfw3.h>

namespace Hazel
{
	class WindowsWindow : public Window
	{
	public:
		WindowsWindow(const WindowProps& props);
		virtual ~WindowsWindow();

		void OnUpdate() override;

		unsigned int GetWidth() const override { return m_Data.Width; }
		unsigned int GetHeight() const override { return m_Data.Height; }
		RendererAPI::API GetAPI() const override { return m_Data.Api; }

		// Window attributes
		void SetEventCallback(const EventCallbackFn& callback) override { m_Data.EventCallback = callback; }
		virtual void SetVSync(bool enabled) override;
		virtual bool IsVSync() const override;

		virtual void* GetNativeWindow() const { return m_pWin_handle; };
	private:
		virtual void Init(const WindowProps& props);
		virtual void Shutdown();
	private:
		GLFWwindow* m_Window;
		void* m_pWin_handle;
		Ref<GraphicsContext> m_Context;

		struct WindowData
		{
			std::string Title;
			unsigned int Width, Height;
			RendererAPI::API Api;
			bool VSync;

			EventCallbackFn EventCallback;
		};

		WindowData m_Data;
	};

}
