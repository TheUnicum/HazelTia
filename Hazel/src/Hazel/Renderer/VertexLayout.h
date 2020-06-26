#pragma once

#include <string>
#include <glm/glm.hpp>

//#include "gl"
//--#include "vulkan/vulkan.h"
#include <d3d11.h>

namespace Hazel
{
	struct BGRAColor
	{
		unsigned char a;
		unsigned char r;
		unsigned char g;
		unsigned char b;
	};

	enum glFormat
	{
		GL_FORMAT_R32G32_FLOAT,			// Float2
		GL_FORMAT_R32G32B32_FLOAT,		// Float3
		GL_FORMAT_R32G32B32A32_FLOAT,	// Float4
		
		GL_FORMAT_R8G8B8A8_UNORM,		// rbga(Int)
		//,, Mat3, Mat4, Int, Int2, Int3, Int4, Bool
		GL_count
	};

	class VertexLayout
	{
	public:
		enum ElementType
		{
			AP_FLOAT2,
			AP_FLOAT3,
			AP_FLOAT4,

			//
			count,
		};

		template<ElementType> struct Map;
		template<> struct Map<AP_FLOAT2>
		{
			using SysType = glm::vec2;
			static constexpr glFormat glFormat = GL_FORMAT_R32G32_FLOAT;
			static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32_FLOAT;
			//static constexpr VkFormat vkFormat = VK_FORMAT_R32G32_SFLOAT;
		};
		template<> struct Map<AP_FLOAT3>
		{
			using SysType = glm::vec3;
			static constexpr glFormat glFormat = GL_FORMAT_R32G32B32_FLOAT;
			static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32B32_FLOAT;
			//static constexpr VkFormat vkFormat = VK_FORMAT_R32G32B32_SFLOAT;
		};
		template<> struct Map<AP_FLOAT4>
		{
			using SysType = glm::vec4;
			static constexpr glFormat glFormat = GL_FORMAT_R32G32B32A32_FLOAT;
			static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32B32A32_FLOAT;
			//static constexpr VkFormat vkFormat = VK_FORMAT_R32G32B32A32_SFLOAT;
		};

		static constexpr uint32_t ApSizeof(ElementType type)
		{
			switch (type)
			{
				case ElementType::AP_FLOAT2:	return sizeof(typename Map<AP_FLOAT2>::SysType); // return 4;
				case ElementType::AP_FLOAT3:	return sizeof(typename Map<AP_FLOAT3>::SysType); // return 4 * 2;
				case ElementType::AP_FLOAT4:	return sizeof(typename Map<AP_FLOAT4>::SysType); // nreturn 4 * 3;
			}
			HZ_CORE_ASSERT(false, "Unknown ElementType!");
			return 0;
		}
		static constexpr uint32_t ApComponentCountof(ElementType type)
		{
			switch (type)
			{
				case ElementType::AP_FLOAT2:	return 1;
				case ElementType::AP_FLOAT3:	return 2;
				case ElementType::AP_FLOAT4:	return 3;
			}
			HZ_CORE_ASSERT(false, "Unknown v!");
			return 0;
		}
		class Element
		{
			friend class VertexLayout;
		public:
			Element(ElementType type, const char* name, bool normalized = false)
				:
				m_type(type),
				m_name(name),
				m_size(ApSizeof(type)),
				m_offset(0),
				m_normalized(normalized)
			{}
			ElementType GetType() const { return m_type; }
			const char* GetName() const { return m_name; }
			size_t Size() const { return m_size; }
			size_t GetOffset() const { return m_offset; }
			size_t GetOffsetAfter() const { return m_offset + Size(); }

			//D3D11_INPUT_ELEMENT_DESC GetDesc() const;
			//const char* GetCode() const noexcept;
		private:
			ElementType m_type;
			const char* m_name;
			uint32_t m_size;
			size_t m_offset;
			bool m_normalized;
		};
	public:
		// --class VertexLayout--
		VertexLayout() = default;
		VertexLayout(const std::initializer_list<Element>& elements)
			: m_elements(elements)
		{
			CalculateOffsetAndStride();
		}

		const Element& Resolve(const char* name) const
		{
			for (auto& e : m_elements)
			{
				if (e.GetName() == name) return e;
			}
			HZ_CORE_ASSERT(false, "Could not resolve element Name: " + std::string(name));
		
		
			return m_elements.front();
		}
		const Element& ResolveByIndex(uint32_t i) const
		{ 
			HZ_CORE_ASSERT(i < GetElementCount(), "VertexLayout-Resolve index out of bounds!");
			return m_elements[i];
		}
		VertexLayout& Append(ElementType type, const char* name)
		{
			m_elements.emplace_back(type, name);
			CalculateOffsetAndStride();
			return *this;
		}
		size_t Size() const { return m_stride; }
		size_t GetElementCount() const { return m_elements.size(); }
		//std::vector<D3D11_INPUT_ELEMENT_DESC> GetD3DLayout() const;
		//std::string GetCode() const;


		std::vector<Element>::iterator begin() { return m_elements.begin(); }
		std::vector<Element>::iterator end() { return m_elements.end(); }
		std::vector<Element>::const_iterator begin() const { return m_elements.begin(); }
		std::vector<Element>::const_iterator end() const { return m_elements.end(); }
	private:
		void CalculateOffsetAndStride()
		{
			size_t offset = 0;
			m_stride = 0;
			for (auto& element : m_elements)
			{
				element.m_offset = offset;
				offset += element.m_size;
				m_stride += element.m_size;
			}
		}

	private:
		std::vector<Element> m_elements;
		uint32_t m_stride = 0;
	};

	
	class Vertex
	{
		friend class VertexData;
	public:
		template<VertexLayout::ElementType Type>
		auto& Attr(const char* name)
		{
			const auto element = m_layout.Resolve(name);
			auto pAttribute = m_pData + element.GetOffset();
			return *reinterpret_cast<typename VertexLayout::Map<Type>::SysType*>(pAttribute);
		}

		auto* Attrp(const char* name)
		{
			const auto element = m_layout.Resolve(name);
			auto pAttribute = m_pData + element.GetOffset();	
			return pAttribute;
		}
		template<typename T>
		void SetAttributeByIndex(uint32_t i, T&& val)
		{
			const auto& element = m_layout.ResolveByIndex(i);
			auto pAttribute = m_pData + element.GetOffset();
			switch (element.GetType())
			{
			case VertexLayout::AP_FLOAT2:
				SetAttribute<VertexLayout::AP_FLOAT2>(pAttribute, std::forward<T>(val));
				break;
			case VertexLayout::AP_FLOAT3:
				SetAttribute<VertexLayout::AP_FLOAT3>(pAttribute, std::forward<T>(val));
				break;
			case VertexLayout::AP_FLOAT4:
				SetAttribute<VertexLayout::AP_FLOAT4>(pAttribute, std::forward<T>(val));
				break;
			default:
				HZ_CORE_ASSERT(false, "Bad element type index");

				assert("Bad element type" && false);
				break;
			}
		}
	protected:
		Vertex(char* pData, const VertexLayout& layout)
			:
			m_pData(pData),
			m_layout(layout)
		{
			HZ_CORE_ASSERT(m_pData, "Vertex canìt have nullptr as pData!");
		}
	private:
		// enables parameter pack setting of multiple parameters by element index
		template<typename First, typename ...Rest>
		void SetAttributeByIndex(uint32_t i, First&& first, Rest&&... rest)
		{
			SetAttributeByIndex(i, std::forward<First>(first));
			SetAttributeByIndex(i + 1, std::forward<Rest>(rest)...);
		}
		// helper to reduce code duplication in SetAttributeByIndex
		template<VertexLayout::ElementType DestLayoutType, typename SrcType>
		void SetAttribute(char* pAttribute, SrcType&& val)
		{
			using Dest = typename VertexLayout::Map<DestLayoutType>::SysType;
			if constexpr (std::is_assignable<Dest, SrcType>::value)
			{
				*reinterpret_cast<Dest*>(pAttribute) = val;
			}
			else
			{
				assert("Parameter attribute type mismatch" && false);
			}
		}
	private:
		char* m_pData = nullptr;
		const VertexLayout& m_layout;
	};

	class ConstVertex
	{
	public:
		ConstVertex::ConstVertex(const Vertex& v)
			:
			vertex(v)
		{}
		template<VertexLayout::ElementType Type>
		const auto& Attr(const char* name) const
		{
			return const_cast<Vertex&>(vertex).Attr<Type>(name);
		}
	private:
		Vertex vertex;
	};


	class VertexData
	{
	public:
		VertexData(VertexLayout layout, size_t size = 0u)
			:
			m_layout(std::move(layout))	{ Resize(size); }
		void Resize(size_t newSize)
		{
			const auto size = Size();
			if (size < newSize)
			{
				m_buffer.resize(m_buffer.size() + m_layout.Size() * (newSize - size));
			}
		}
		const char* GetData() const { return m_buffer.data(); }
		const VertexLayout& GetLayout() const { return m_layout; }
		uint32_t Count() const { return static_cast<uint32_t>(m_buffer.size() / m_layout.Size()); }
		uint32_t Size() const { return static_cast<uint32_t>(m_buffer.size()); }
		template<typename ...Params>
		void EmplaceBack(Params&&... params)
		{

			HZ_CORE_ASSERT(sizeof...(params) == m_layout.GetElementCount(), "Param count doesn't match number of vertex elements");
			m_buffer.resize(m_buffer.size() + m_layout.Size());
			Back().SetAttributeByIndex(0u, std::forward<Params>(params)...);
		}
		Vertex Back();
		Vertex Front();
		Vertex operator[](size_t i);
		ConstVertex Back() const;
		ConstVertex Front() const;
		ConstVertex operator[](size_t i) const;
	private:
		std::vector<char> m_buffer;
		VertexLayout m_layout;
	};

}
