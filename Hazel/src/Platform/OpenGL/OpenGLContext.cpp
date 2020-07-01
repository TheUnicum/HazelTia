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

	OpenGLContext::~OpenGLContext()
	{
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
			glfwMakeContextCurrent(m_windowHandle);
		}
		if (_s_active.get() != this)
		{
			_s_active = Resolve(API::OpenGL, m_window); // I need to use RESOLVE to 
		}
		return GetAPI();
	}

	// API functions
	void OpenGLContext::CmdClear_impl()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}
	void OpenGLContext::CmdClearColor_impl(float red, float green, float blue)
	{
		glClearColor(red, green, blue, 1.0f);
	}
	void OpenGLContext::CmdDrawArrays_impl(uint32_t vertexCount, uint32_t offset)
	{
		HZ_CORE_ASSERT(false, "Function Still NOT implemented!");
	}
	void OpenGLContext::CmdDrawArraysInstanced_impl(uint32_t vertexCount, uint32_t instanceCount)
	{
		HZ_CORE_ASSERT(false, "Function Still NOT implemented!");

	}
	void OpenGLContext::CmdDrawIndexted_impl(uint32_t indexCount, uint32_t offset)
	{
		glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, nullptr);
	}
	void OpenGLContext::CmdDrawIndextedInstanced_impl(uint32_t indexCount, uint32_t instanceCount)
	{
		HZ_CORE_ASSERT(false, "Function Still NOT implemented!");

	}

	// to remove !!
	void OpenGLContext::ClearBuffer_impl(float red, float green, float blue)
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(red, green, blue, 1.0f);
	}
}

