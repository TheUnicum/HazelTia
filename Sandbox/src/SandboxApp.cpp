#include <Hazel.h>

#include "imgui/imgui.h"

class ExampleLayer : public Hazel::Layer 
{
public:
	ExampleLayer()
		: Layer("Example")
	{
	}

	void OnUpdate() override
	{
		if (Hazel::Input::IsKeyPressed(HZ_KEY_TAB))
			HZ_TRACE("Tab key is pressed (poll)!");
	}

	virtual void OnImGuiRender() override
	{
		/*
		Hazel\vendor\imgui\imconfig.h
		uncomment line 21
		#define IMGUI_API __declspec( dllexport )
		or
		in project properties define
		#define IMGUI_API __declspec( dllexport )
		*/	

		/*
		PS Remember to update the submodule-files:
		Hazel\vendor\imgui\premake5.lua
		<Hazel\vendor\Glad\premake5.lua>
		Hazel\vendor\GLFW\premake5.lua
		*/

		ImGui::Begin("Test");
		ImGui::Text("Hello World!");
		ImGui::End();
	}

	void OnEvent(Hazel::Event& event) override
	{
		if (event.GetEventType() == Hazel::EventType::KeyPressed)
		{
			Hazel::KeyPressedEvent& e = (Hazel::KeyPressedEvent&)event;
			if (e.GetKeyCode() == HZ_KEY_TAB)
				HZ_TRACE("Tab key is pressed (event)!");
			HZ_TRACE("{0}", (char)e.GetKeyCode());
		}
	}

};

class Sandbox : public Hazel::Application
{
public:
	Sandbox()
	{
		PushLayer(new ExampleLayer());
	}

	~Sandbox()
	{

	}
};

Hazel::Application* Hazel::CreateApplication()
{
	return new Sandbox();
}
