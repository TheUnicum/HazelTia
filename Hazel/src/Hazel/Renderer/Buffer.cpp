#include "hzpch.h"
#include "Hazel/Renderer/Buffer.h"

#include "Platform/OpenGL/OpenGLBuffer.h"
#include "Platform/D3D11/D3D11Buffer.h"

namespace Hazel {

	std::string VertexBuffer::GenerateUID_(std::string tag)
	{
		using namespace std::string_literals;
		return   typeid(VertexBuffer).name() + "#"s + tag;
	}
	// 1.
	Ref<VertexBuffer> VertexBuffer::Create(uint32_t size) { return Create(Get_ID_NR(), size); }
	Ref<VertexBuffer> VertexBuffer::Create(std::string& tag, uint32_t size) { return Create((GraphicsContext&)GraphicsContext::Get_Active(), Get_ID_NR(), size); }
	Ref<VertexBuffer> VertexBuffer::Create(GraphicsContext& ctx, std::string& tag, uint32_t size)
	{
		MAKER_ON_ctx_args_2(OpenGLVertexBuffer, D3D11VertexBuffer, tag, size);
		//switch (ctx.GetAPI())
		//{
		//	case API::None:    HZ_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		//	case API::OpenGL:  return CreateRef<OpenGLVertexBuffer>(tag, size);
		//	case API::D3D11:   return CreateRef<D3D11VertexBuffer>(tag, size);
		//}
		//HZ_CORE_ASSERT(false, "Unknow RendererAPI!");
		//return nullptr;
	}

	// 2.
	Ref<VertexBuffer> VertexBuffer::Create(float* vertices, uint32_t size) { return Create((GraphicsContext&)GraphicsContext::Get_Active(), vertices, size); }
	Ref<VertexBuffer> VertexBuffer::Create(GraphicsContext& ctx, float* vertices, uint32_t size) { return Create(ctx, Get_ID_NR(), vertices, size); }
	Ref<VertexBuffer> VertexBuffer::Create(std::string& tag, float* vertices, uint32_t size) { return Create((GraphicsContext&)GraphicsContext::Get_Active(), tag, vertices, size); }
	Ref<VertexBuffer> VertexBuffer::Create(GraphicsContext& ctx, std::string& tag, float* vertices, uint32_t size)
	{
		MAKER_ON_ctx_args_3(OpenGLVertexBuffer, D3D11VertexBuffer, tag, vertices, size);
		//switch (ctx.GetAPI())
		//{
		//	case API::None:    HZ_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		//	case API::OpenGL:  return CreateRef<OpenGLVertexBuffer>(tag, vertices, size);
		//	case API::D3D11:  return CreateRef<D3D11VertexBuffer>(tag, vertices, size);
		//}
		//HZ_CORE_ASSERT(false, "Unknown RendererAPI!");
		//return nullptr;
	}


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	std::string IndexBuffer::GenerateUID_(std::string tag)
	{
		using namespace std::string_literals;
		return   typeid(IndexBuffer).name() + "#"s + tag;
	}

	std::string IndexBuffer::GenerateUID(GraphicsContext& ctx, const std::string& _tag, uint32_t* indices, uint32_t count)
	{
		return GenerateUID_(_tag);
	}

	Ref<IndexBuffer> IndexBuffer::Create(uint32_t* indices, uint32_t count) { return Create((GraphicsContext&)GraphicsContext::Get_Active(), indices, count); }
	Ref<IndexBuffer> IndexBuffer::Create(GraphicsContext& ctx, uint32_t* indices, uint32_t count) { return Create(ctx, Get_ID_NR(), indices, count); }
	Ref<IndexBuffer> IndexBuffer::Create(std::string& tag, uint32_t* indices, uint32_t count) { return Create((GraphicsContext&)GraphicsContext::Get_Active(), tag, indices, count); }
	Ref<IndexBuffer> IndexBuffer::Create(GraphicsContext& ctx, std::string& tag, uint32_t* indices, uint32_t count)
	{
		MAKER_ON_ctx_args_3(OpenGLIndexBuffer, D3D11IndexBuffer, tag, indices, count);
		//switch (ctx.GetAPI())
		//{
		//	case API::None:    HZ_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		//	case API::OpenGL:  return CreateRef<OpenGLIndexBuffer>(tag, indices, count);
		//	case API::D3D11:   return CreateRef<D3D11IndexBuffer>(tag, indices, count);
		//}
		//HZ_CORE_ASSERT(false, "Unknown RendererAPI!");
		//return nullptr;
	}

}
