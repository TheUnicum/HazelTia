#include "hzpch.h"
#include "WindowInput.h"

#include "Hazel/Application.h"
#include "GLFW/glfw3.h"

namespace Hazel {

	Input* Input::s_Instance = new WindowInput();

	bool WindowInput::IsKeyPressedImpl(int keycode)
	{
		auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		auto state = glfwGetKey(window, keycode);
		return state == GLFW_PRESS || state == GLFW_REPEAT;
	}

	bool WindowInput::IsMouseButtonPressedImpl(int button)
	{
		auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		auto state = glfwGetMouseButton(window, button);
		return state == GLFW_PRESS;
	}

	std::pair<float, float> WindowInput::GetMousePositionImpl()
	{
		auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);

		return { (float)xpos, (float)ypos };
	}

	float WindowInput::GetMouseXImpl()
	{
		auto[x, y] = GetMousePosition();
		return x;
	}

	float WindowInput::GetMouseYImpl()
	{
		auto[x, y] = GetMousePosition();
		return y;
	}

}
