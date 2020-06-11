#pragma once

#include "Hazel/Renderer/GraphicsContext.h"

struct GLFWwindow;

namespace Hazel {

	class OpenGLContext : public GraphicsContext
	{
	public:
		OpenGLContext() = default;
		OpenGLContext(Window& windowHandle);

		virtual void Init() override;
		virtual void SwapBuffers() override;

		virtual API GetAPI() const override { return API::OpenGL; };
		virtual std::string GetAPI_TEXT() const override { return "OpenGL"; };
		virtual API MakeCurrent() override;
	private:
		Window& m_window;
		GLFWwindow* m_windowHandle;

		// API functions
		public:
		virtual void DrawTriangle_impl(float angle) override;

		virtual void ClearBuffer_impl(float red, float green, float blue) override;


	};

}
