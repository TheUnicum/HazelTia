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
		API GetAPI() const override { return m_Data.Api; }
		void* GetCfx() const override { return m_Context.get(); }

		// Window attributes
		void SetEventCallback(const EventCallbackFn& callback) override { m_Data.EventCallback = callback; }
		void SetPonterContext(void* p_context) override { m_Data.p_ctx = p_context; }
		virtual void SetVSync(bool enabled) override;
		virtual bool IsVSync() const override;


		virtual void* GetNativeWindow() const override { return m_pWin_handle; };
	private:
		virtual void Init(const WindowProps& props);
		virtual void Shutdown();
	private:
		GLFWwindow* m_Window;
		void* m_pWin_handle;
		Ref<GraphicsContext> m_Context;
	public:
		struct WindowData
		{
			std::string Title;
			unsigned int Width, Height;
			API Api;
			bool VSync;

			// Pointer to context
			void* p_ctx; // for Vulkan debug!

			EventCallbackFn EventCallback;
		};
	private:
		WindowData m_Data;
	};

}
