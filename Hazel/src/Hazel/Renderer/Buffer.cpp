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

	std::string IndexBuffer::GenerateUID_(std::string tag)
	{
		using namespace std::string_literals;
		return   typeid(IndexBuffer).name() + "#"s + tag;
	}

	Ref<IndexBuffer> IndexBuffer::Create(uint32_t* indices, uint32_t count) { return Create((GraphicsContext&)GraphicsContext::Get_Active(), indices, count); }
	Ref<IndexBuffer> IndexBuffer::Create(GraphicsContext& ctx, uint32_t* indices, uint32_t count) { return Create(ctx, Get_ID_NR(), indices, count); }
	Ref<IndexBuffer> IndexBuffer::Create(std::string& tag, uint32_t* indices, uint32_t count) { return Create((GraphicsContext&)GraphicsContext::Get_Active(), Get_ID_NR(), indices, count); }
	Ref<IndexBuffer> IndexBuffer::Create(GraphicsContext& ctx, std::string& tag, uint32_t* indices, uint32_t count)
	{
		//MAKER_ON_ctx_args_3(OpenGLIndexBuffer, D3D11IndexBuffer, indices, tag, count);
		switch (ctx.GetAPI())
		{
			case API::None:    HZ_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
			case API::OpenGL:  return CreateRef<OpenGLIndexBuffer>(tag, indices, count);
			case API::D3D11:   return CreateRef<D3D11IndexBuffer>(tag, indices, count);
		}
		HZ_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

}
