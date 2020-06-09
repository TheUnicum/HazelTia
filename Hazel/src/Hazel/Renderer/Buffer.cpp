#include "hzpch.h"
#include "Hazel/Renderer/Buffer.h"

#include "Hazel/Renderer/Renderer.h"

#include "Platform/OpenGL/OpenGLBuffer.h"
#include "Platform/D3D11/D3D11Buffer.h"

namespace Hazel {

	Ref<VertexBuffer> VertexBuffer::Create(uint32_t size)
	{
		switch (Renderer::GetAPI())
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
		switch (Renderer::GetAPI())
		{
			case API::None:    HZ_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
			case API::OpenGL:  return CreateRef<OpenGLVertexBuffer>(vertices, size);
		}

		HZ_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	Ref<IndexBuffer> IndexBuffer::Create(uint32_t* indices, uint32_t count) { return Create((GraphicsContext&)GraphicsContext::Get_Active(), indices, count); }
	Ref<IndexBuffer> IndexBuffer::Create(GraphicsContext& ctx, uint32_t* indices, uint32_t count)
	{
		switch (ctx.GetAPI())
		{
			case API::None:    HZ_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
			case API::OpenGL:  return CreateRef<OpenGLIndexBuffer>(indices, count);
			case API::D3D11:   return CreateRef<D3D11IndexBuffer>(indices, count);
		}
	
		HZ_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	//Ref<IndexBuffer> IndexBuffer::Make(GraphicsContext& ctx, uint32_t* indices, uint32_t count)
	//{
	//	switch (ctx.GetAPI())
	//	{
	//		case API::None:    HZ_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
	//		case API::OpenGL:  return CreateRef<OpenGLIndexBuffer>(indices, count);
	//		case API::D3D11:   return CreateRef<D3D11IndexBuffer>(indices, count);
	//	}
	//
	//	HZ_CORE_ASSERT(false, "Unknown RendererAPI!");
	//	return nullptr;
	//}




	//// X Test
	//Ref<IndexBufferX> IndexBufferX::Make(GraphicsContext& ctx, uint32_t* indices, uint32_t count)
	//{
	//	return nullptr;// MAKER_ON_ctx3_arg(INDEXBUFFER_TYPES, IndexBufferX, ctx, indices, count);
	//}

}
