#pragma once

#include "Hazel/Renderer/GraphicsContext.h"

#include "Hazel/Renderer/Bindable.h"
#include "Hazel/Renderer/VertexLayout.h"

namespace Hazel {

	enum class ShaderDataType
	{
		None = 0, Float, Float2, Float3, Float4, Mat3, Mat4, Int, Int2, Int3, Int4, Bool
	};

	static uint32_t ShaderDataTypeSize(ShaderDataType type)
	{
		switch (type)
		{
		case ShaderDataType::Float:    return 4;
		case ShaderDataType::Float2:   return 4 * 2;
		case ShaderDataType::Float3:   return 4 * 3;
		case ShaderDataType::Float4:   return 4 * 4;
		case ShaderDataType::Mat3:     return 4 * 3 * 3;
		case ShaderDataType::Mat4:     return 4 * 3 * 3;
		case ShaderDataType::Int:      return 4;
		case ShaderDataType::Int2:     return 4 * 2;
		case ShaderDataType::Int3:     return 4 * 3;
		case ShaderDataType::Int4:     return 4 * 4;
		case ShaderDataType::Bool:     return 1;
		}

		HZ_CORE_ASSERT(false, "Unknown ShaderDataType!");
		return 0;
	}

	struct BufferElement
	{
		std::string Name;
		ShaderDataType Type;
		uint32_t Size;
		size_t Offset;
		bool Normalized;

		BufferElement() = default;

		BufferElement(ShaderDataType type, const std::string& name, bool normalized = false)
			: Name(name), Type(type), Size(ShaderDataTypeSize(type)), Offset(0), Normalized(normalized)
		{
		}

		uint32_t GetComponentCount() const
		{
			switch (Type)
			{
			case ShaderDataType::Float:   return 1;
			case ShaderDataType::Float2:  return 2;
			case ShaderDataType::Float3:  return 3;
			case ShaderDataType::Float4:  return 4;
			case ShaderDataType::Mat3:    return 3; // 3* float3
			case ShaderDataType::Mat4:    return 4; // 4* float4
			case ShaderDataType::Int:     return 1;
			case ShaderDataType::Int2:    return 2;
			case ShaderDataType::Int3:    return 3;
			case ShaderDataType::Int4:    return 4;
			case ShaderDataType::Bool:    return 1;
			}

			HZ_CORE_ASSERT(false, "Unknown ShaderDataType!");
			return 0;
		}
	};

	class BufferLayout
	{
	public:
		BufferLayout() {}

		BufferLayout(const std::initializer_list<BufferElement>& elements)
			: m_Elements(elements)
		{
			CalculateOffsetAndStride();
		}

		uint32_t GetStride() const { return m_Stride; }
		const std::vector<BufferElement>& GetElements() const { return m_Elements; }

		std::vector<BufferElement>::iterator begin() { return m_Elements.begin(); }
		std::vector<BufferElement>::iterator end() { return m_Elements.end(); }
		std::vector<BufferElement>::const_iterator begin() const { return m_Elements.begin(); }
		std::vector<BufferElement>::const_iterator end() const { return m_Elements.end(); }
	private:
		void CalculateOffsetAndStride()
		{
			size_t offset = 0;
			m_Stride = 0;
			for (auto& element : m_Elements)
			{
				element.Offset = offset;
				offset += element.Size;
				m_Stride += element.Size;
			}
		}
	private:
		std::vector<BufferElement> m_Elements;
		uint32_t m_Stride = 0;
	};

	class VertexBuffer : public Bindable
	{
	public:
		VertexBuffer(GraphicsContext& ctx) : Bindable(ctx) {}
		virtual ~VertexBuffer() = default;

		virtual void SetData(const void* data, uint32_t size) = 0;

		virtual void BindTemp(uint32_t stride) const {} // TEMPORANEA in attesa di aggiungere "stride in vb"

		virtual const BufferLayout& GetLayout() const = 0;
		virtual void SetLayout(const BufferLayout& layout) = 0;
		virtual void SetVertexLayout(const VertexLayout& layout) { m_VertexLayout = layout;	} // for now is a copy!!
	protected:
		VertexLayout m_VertexLayout;
		static std::string GenerateUID_(std::string tag);
	public:
		static std::string Get_ID_NR() { return std::to_string(std::time(0)); }
		template<typename...Ignore>
		static std::string GenerateUID(GraphicsContext& ctx, const std::string& tag, Ignore&&...ignore)
		{
			return GenerateUID_(tag);
		}

		// Classes Factory
		static Ref<VertexBuffer> Create(uint32_t size);
		static Ref<VertexBuffer> Create(std::string& tag, uint32_t size);
		static Ref<VertexBuffer> Create(GraphicsContext& ctx, std::string& tag, uint32_t size);
		// 2.
		static Ref<VertexBuffer> Create(float* vertices, uint32_t size);
		static Ref<VertexBuffer> Create(GraphicsContext& ctx, float* vertices, uint32_t size);
		static Ref<VertexBuffer> Create(std::string& tag, float* vertices, uint32_t size);
		static Ref<VertexBuffer> Create(GraphicsContext& ctx, std::string& tag, float* vertices, uint32_t size);

		// 1. VertexBuffer Resolver (Star of chain of Class creation)
		static Ref<VertexBuffer> Resolve(uint32_t size)
		{
			return std::static_pointer_cast<VertexBuffer>(Bindable::ResolvePctx<VertexBuffer>(VertexBuffer::Get_ID_NR(), size));
		}
		static Ref<VertexBuffer> Resolve(std::string& tag, uint32_t size)
		{
			return std::static_pointer_cast<VertexBuffer>(Bindable::ResolvePctx<VertexBuffer>(tag, size));
		}
		static Ref<VertexBuffer> Resolve(GraphicsContext& ctx, std::string& tag, uint32_t size)
		{
			return std::static_pointer_cast<VertexBuffer>(Bindable::_Resolve<VertexBuffer>(ctx, tag, size));
		}

		// 2. VertexBuffer Resolver (Star of chain of Class creation)
		static Ref<VertexBuffer> Resolve(float* vertices, uint32_t size)
		{
			return std::static_pointer_cast<VertexBuffer>(Bindable::ResolvePctx<VertexBuffer>(VertexBuffer::Get_ID_NR(), vertices, size));
		}
		static Ref<VertexBuffer> Resolve(GraphicsContext& ctx, float* vertices, uint32_t size)
		{
			return std::static_pointer_cast<VertexBuffer>(Bindable::_Resolve<VertexBuffer>(ctx, VertexBuffer::Get_ID_NR(), vertices, size));
		}
		static Ref<VertexBuffer> Resolve(std::string& tag, float* vertices, uint32_t size)
		{
			return std::static_pointer_cast<VertexBuffer>(Bindable::ResolvePctx<VertexBuffer>(tag, vertices, size));
		}
		static Ref<VertexBuffer>Resolve(GraphicsContext& ctx, std::string& tag, float* vertices, uint32_t size)
		{
			return std::static_pointer_cast<VertexBuffer>(Bindable::_Resolve<VertexBuffer>(ctx, tag, vertices, size));
		}
	};


	// Currently Hazel only supports 32-bit index
	class IndexBuffer : public Bindable
	{
	public:
		IndexBuffer(GraphicsContext& ctx) : Bindable(ctx) {}
		virtual ~IndexBuffer() = default;
		virtual uint32_t GetCount() const = 0;
		
	protected:
		static std::string GenerateUID_(std::string tag);
	public:
		static std::string Get_ID_NR() { return std::to_string(std::time(0));}
		static std::string GenerateUID(GraphicsContext& ctx, const std::string& _tag, uint32_t* indices, uint32_t count);

		// Classes Factory
		static Ref<IndexBuffer> IndexBuffer::Create(uint32_t* indices, uint32_t count);
		static Ref<IndexBuffer> IndexBuffer::Create(GraphicsContext& ctx, uint32_t* indices, uint32_t count);
		static Ref<IndexBuffer> IndexBuffer::Create(std::string& tag, uint32_t* indices, uint32_t count);
		static Ref<IndexBuffer> IndexBuffer::Create(GraphicsContext& ctx, std::string& tag, uint32_t* indices, uint32_t count);

		// IndexBuffer Resolver (Star of chain of Class creation)
		static Ref<IndexBuffer> Resolve(uint32_t* indices, uint32_t count)
		{
			return std::static_pointer_cast<IndexBuffer>(Bindable::ResolvePctx<IndexBuffer>(IndexBuffer::Get_ID_NR(), indices, count));
		}
		static Ref<IndexBuffer> Resolve(GraphicsContext& ctx, uint32_t* indices, uint32_t count)
		{
			return std::static_pointer_cast<IndexBuffer>(Bindable::_Resolve<IndexBuffer>(ctx, IndexBuffer::Get_ID_NR(), indices, count));
		}
		static Ref<IndexBuffer> Resolve(std::string& tag, uint32_t* indices, uint32_t count)
		{
			return std::static_pointer_cast<IndexBuffer>(Bindable::ResolvePctx<IndexBuffer>(tag, indices, count));
		}
		static Ref<IndexBuffer> Resolve(GraphicsContext& ctx, std::string& tag, uint32_t* indices, uint32_t count)
		{
			return std::static_pointer_cast<IndexBuffer>(Bindable::_Resolve<IndexBuffer>(ctx, tag, indices, count));
		}
	public:

	};

}
