#pragma once
#include "Hazel/Core/Window.h"

#include "Hazel/Renderer/GraphicsContext.h"

namespace Hazel {

	class RenderCommandX
	{
	public:
		static void MakeContextCurrent(Window& window)
		{
			s_ctx = &window.GetContext();
		}
	
		//static void Init()
		//{
		//	s_map_RendererX[s_api_selected]->Init();
		//}
		//
		//static void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
		//{
		//	s_map_RendererX[s_api_selected]->SetViewport(x, y, width, height);
		//}
		//
		//static void SetClearColor(const glm::vec4& color)
		//{
		//	s_map_RendererX[s_api_selected]->SetClearColor(color);
		//}
		//
		static void Clear()
		{
			if (s_ctx) s_ctx->CmdClear_impl();
		}
	
		static void DrawArray(uint32_t vertexCount = 0)
		{
			if (s_ctx) s_ctx->CmdDrawArrays_impl(vertexCount);
		}
	
	
		//// Test
		//static void DrawTriangle()
		//{
		//	s_map_RendererX[s_api_selected]->DrawTriangle();
		//}
	
	private:
		static GraphicsContext* s_ctx;
	};

}
