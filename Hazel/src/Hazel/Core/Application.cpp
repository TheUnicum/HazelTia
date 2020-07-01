#include "hzpch.h"
#include "Hazel/Core/Application.h"

#include "Hazel/Core/Log.h"

#include "Hazel/Renderer/Renderer.h"
#include "Hazel/Renderer/GraphicsContext.h"

#include "Hazel/Core/Input.h"

#include <GLFW/glfw3.h>

// test Remeve at the end
#include "Hazel/Renderer/ShaderCode.h"
#include "Hazel/Renderer/PipelineSpecification.h"
// 

namespace Hazel {

	Application* Application::s_Instance = nullptr;

	Application::Application()
	{
		HZ_PROFILE_FUNCTION();

		HZ_CORE_ASSERT(!s_Instance, "Application already exists!");
		s_Instance = this;
		m_Window = Window::Create();
		m_Window->SetEventCallback(HZ_BIND_EVENT_FN(Application::OnEvent));

		//static_cast<GraphicsContext*>(m_Window->GetCfx())->MakeCurrent();
		//m_Window->OnUpdate();

		m_WindowsTest.emplace_back(Window::Create({ "2 D3D Test", 800, 600, API::D3D11 }));
		//m_WindowsTest.emplace_back(Window::Create({ "3 OpenGL Test", 800, 600, API::OpenGL }));
		//m_WindowsTest.emplace_back(Window::Create({ "3 Vulkan ", 800, 600, API::Vulkan }));


		//// Test shader CODE
		//static_cast<GraphicsContext*>(m_WindowsTest[0]->GetCfx())->MakeCurrent();
		//
		//Ref<ShaderCode> sc = ShaderCode::Create("assets/shaders/Vulkan/FragColor_VB.glsl");
		//auto c = sc->GetCodeGLSL();
		//auto hs = sc->GetCodeHLSL();
		//auto v = sc->GetVertexLayoutEleList();
		//
		////auto ssREd = Hazel::Shader::Create("assets/shaders/Vulkan/FragColor_VB.glsl");
		//auto ssREd = Hazel::Shader::Create(sc);
		//PipelineCreateInfo createInfo;
		//createInfo.shader = ssREd;
		//createInfo.vertexLayout = nullptr;
		//Ref<PipelineSpecification> PipeSpec2 = Hazel::PipelineSpecification::Create(createInfo);
		////PipeSpec2->Bind();
		////.....



		//m_WindowsTest.emplace_back(Window::Create({ "4 GL Test", 800, 600, API::OpenGL }));
		static_cast<GraphicsContext*>(m_Window->GetCfx())->MakeCurrent();
		//m_Window->OnUpdate(); // to reset glfwcontext to main windows old implementation!

		for (auto& win : m_WindowsTest)
		{
			win->SetEventCallback(HZ_BIND_EVENT_FN(Application::OnEvent));
		}

		Renderer::Init();

		m_ImGuiLayer = new ImGuiLayer();
		PushOverlay(m_ImGuiLayer);
	}

	Application::~Application()
	{
		HZ_PROFILE_FUNCTION();

		Renderer::Shutdown();
	}

	void Application::PushLayer(Layer* layer)
	{
		HZ_PROFILE_FUNCTION();

		m_LayerStack.PushLayer(layer);
		layer->OnAttach();
	}

	void Application::PushOverlay(Layer* layer)
	{
		HZ_PROFILE_FUNCTION();

		m_LayerStack.PushOverlay(layer);
		layer->OnAttach();
	}

	void Application::OnEvent(Event& e)
	{
		HZ_PROFILE_FUNCTION();

		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(HZ_BIND_EVENT_FN(Application::OnWindowClose));
		dispatcher.Dispatch<WindowResizeEvent>(HZ_BIND_EVENT_FN(Application::OnWindowResize));

		for (auto it = m_LayerStack.rbegin(); it != m_LayerStack.rend(); it++)
		{
			if (e.Handled)
				break;
			(*it)->OnEvent(e);
		}
	}

	void Application::Run()
	{
		HZ_PROFILE_FUNCTION();

		while (m_Running)
		{
			HZ_PROFILE_SCOPE("RunLoop");

			m_Window->OnUpdate(); // TODO: Remove, only for test also old GLFW code (glfwcurrentcontext)

			float time = (float)glfwGetTime(); // Platform::GetTime()
			Timestep timestep = time - m_LastFrameTime;
			m_LastFrameTime = time;

			if (!m_Minimize)
			{
				{
					HZ_PROFILE_SCOPE("LayerStack OnUpdate");

					for (Layer* layer : m_LayerStack)
						layer->OnUpdate(timestep);
				}

				m_ImGuiLayer->Begin();
				{
					HZ_PROFILE_SCOPE("LayerStack OnImGuiRender");

					for (Layer* layer : m_LayerStack)
						layer->OnImGuiRender();
				}
				m_ImGuiLayer->End();
			}

			for (auto& win : m_WindowsTest)
				win->OnUpdate(); // m_Context->MakeCurrent() inside this function

		}
	}

	bool Application::OnWindowClose(WindowCloseEvent& e)
	{
		m_Running = false;
		return true;
	}

	bool Application::OnWindowResize(WindowResizeEvent& e)
	{
		HZ_PROFILE_FUNCTION();

		if (e.GetWidth() == 0 || e.GetHeight() == 0)
		{
			m_Minimize = true;
			return false;
		}

		m_Minimize = false;
		Renderer::OnWindowResize(e.GetWidth(), e.GetHeight());

		return false;
	}

}
