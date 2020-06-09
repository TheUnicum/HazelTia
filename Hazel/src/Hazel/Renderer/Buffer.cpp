#include "hzpch.h"
#include "Hazel/Renderer/Buffer.h"

#include "Platform/OpenGL/OpenGLBuffer.h"
#include "Platform/D3D11/D3D11Buffer.h"

namespace Hazel {

	Ref<VertexBuffer> VertexBuffer::Create(uint32_t size)
	{
		GraphicsContext& ctx = (GraphicsContext&)GraphicsContext::Get_Active();
		switch (ctx.GetAPI())
		{
			case API::None:    HZ_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
			case API::OpenGL:  return CreateRef<OpenGLVertexBuffer>(size);
			//case RendererAPI::API::D3D11:   return CreateRef<OpenGLVertexBuffer>(size);
		}

		HZ_CORE_ASSERT(false, "Unknow RendererAPI!");
		return nullptr;
	}


	Ref<VertexBuffer> VertexBuffer::Create(float* vertices, uint32_t size)
	{
		GraphicsContext& ctx = (GraphicsContext&)GraphicsContext::Get_Active();
		switch (ctx.GetAPI())
		{
			case API::None:    HZ_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
			case API::OpenGL:  return CreateRef<OpenGLVertexBuffer>(vertices, size);
		}

		HZ_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	std::string IndexBuffer::GenerateUID(GraphicsContext& ctx, uint32_t* indices, uint32_t count)
	{
		return ctx.GetAPI_TEXT() + "@:" + std::to_string(reinterpret_cast<uintptr_t>(&ctx)) +
			"|" + "tag";
	}

	Ref<IndexBuffer> IndexBuffer::Create(uint32_t* indices, uint32_t count) { return Create((GraphicsContext&)GraphicsContext::Get_Active(), indices, count); }
	Ref<IndexBuffer> IndexBuffer::Create(GraphicsContext& ctx, uint32_t* indices, uint32_t count)
	{
		MAKER_ON_ctx_args_2(OpenGLIndexBuffer, D3D11IndexBuffer, indices, count);
		//switch (ctx.GetAPI())
		//{
		//	case API::None:    HZ_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		//	case API::OpenGL:  return CreateRef<OpenGLIndexBuffer>(indices, count);
		//	case API::D3D11:   return CreateRef<D3D11IndexBuffer>(indices, count);
		//}
		//
		//HZ_CORE_ASSERT(false, "Unknown RendererAPI!");
		//return nullptr;
	}

}
