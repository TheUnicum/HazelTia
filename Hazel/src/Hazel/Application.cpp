#include "Application.h"

#include "Hazel/Events/ApplicationEvent.h"
#include "Hazel/Events/KeyEvent.h"
#include "Hazel/Events/MouseEvent.h"
#include "Hazel/Log.h"

namespace Hazel {
	
	Application::Application()
	{
	}


	Application::~Application()
	{
	}

	void Application::Run()
	{
		WindowResizeEvent winRes(1280, 720);
		if (winRes.IsInCategory(EventCategoryApplication))
			HZ_TRACE(winRes);

		KeyPressedEvent keyPres(345, 34);
		if (keyPres.IsInCategory(EventCategoryInput))
			HZ_TRACE(keyPres);
		
		MouseButtonPressedEvent mousePres(77);
		if (mousePres.IsInCategory(EventCategoryMouse))
			HZ_TRACE(mousePres);


		while (true);
	}

}
