#include "hzpch.h"
#include "Platform/OpenGL/OpenGLContext.h"

#include "Hazel/Core/Window.h"

#include <GLFW/glfw3.h>
#include <glad/glad.h>

namespace Hazel {

	OpenGLContext::OpenGLContext(Window& window)
		: m_window(window), m_windowHandle((GLFWwindow*)window.GetNativeWindow())
	{
		HZ_CORE_ASSERT(m_windowHandle, "Window handle is null!");
	}

	void OpenGLContext::Init()
	{
		HZ_PROFILE_FUNCTION();

		glfwMakeContextCurrent(m_windowHandle);
		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		HZ_CORE_ASSERT(status, "Failed to initialize Glad!");

		HZ_CORE_INFO("OpenGL Info:");
		HZ_CORE_INFO("  Vendor: {0}", glGetString(GL_VENDOR));
		HZ_CORE_INFO("  Renderer: {0}", glGetString(GL_RENDERER));
		HZ_CORE_INFO("  Version: {0}", glGetString(GL_VERSION));

		#ifdef HZ_ENABLE_ASSERTS
		int versionMajor;
		int versionMinor;
		glGetIntegerv(GL_MAJOR_VERSION, &versionMajor);
		glGetIntegerv(GL_MINOR_VERSION, &versionMinor);

		HZ_CORE_ASSERT(versionMajor > 4 || (versionMajor == 4 && versionMinor >= 5), "Hazel requires al least OpenGL version 4.5!");
		#endif
	}

	void OpenGLContext::SwapBuffers()
	{
		HZ_PROFILE_FUNCTION();

		glfwSwapBuffers(m_windowHandle);
	}

	API OpenGLContext::MakeCurrent()
	{
		if (m_windowHandle != glfwGetCurrentContext())
		{
			HZ_CORE_TRACE("NOt Same current");
			glfwMakeContextCurrent(m_windowHandle);
		}
		if (_s_active.get() != this)
		{
			_s_active = Resolve(API::OpenGL, m_window); // I need to use RESOLVE to 
		}
		return GetAPI();
	}

}
