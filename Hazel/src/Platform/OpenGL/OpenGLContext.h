#pragma once

#include "Hazel/Renderer/GraphicsContext.h"

struct GLFWwindow;

namespace Hazel {

	class OpenGLContext : public GraphicsContext
	{
	public:
		OpenGLContext(Window& windowHandle);

		virtual void Init() override;
		virtual void SwapBuffers() override;

		virtual RendererAPI::API GetAPI() override;
		virtual RendererAPI::API MakeCurrent() override;
	private:
		Window& m_window;
		GLFWwindow* m_windowHandle;
	};

}
