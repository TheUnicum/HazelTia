#include "hzpch.h"
#include "VertexLayout.h"

#include "Platform/OpenGL/OpenGLVertexLayout.h"
#include "Platform/D3D11/D3D11VertexLayout.h"
#include "Platform/Vulkan/VulkanVertexLayout.h"

namespace Hazel {



	Ref<VertexLayout> VertexLayout::Create() { return Create((GraphicsContext&)GraphicsContext::Get_Active()); }
	Ref<VertexLayout> VertexLayout::Create(GraphicsContext& ctx)
	{
		switch (ctx.GetAPI())
		{
		case API::None:    HZ_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
		case API::OpenGL:  return CreateRef<OpenGLVertexLayout>();
		case API::D3D11:   return CreateRef<D3D11VertexLayout>();
		case API::Vulkan:  return CreateRef<VulkanVertexLayout>();   // DA aggiungere TAG!!!
		}
		HZ_CORE_ASSERT(false, "Unknow RendererAPI!");
		return nullptr;
		return Ref<VertexLayout>();
	}












	std::vector<D3D11_INPUT_ELEMENT_DESC> VertexLayout::GetD3DLayout() const
	{
		std::vector<D3D11_INPUT_ELEMENT_DESC> desc;
		desc.reserve(GetElementCount());
		for (const auto& e : m_elements)
		{
			desc.push_back(e.GenerateDesc());
		}
		return desc;
	}

	Vertex VertexData::Back()
	{
		assert(m_buffer.size() != 0u);
		return Vertex{ m_buffer.data() + m_buffer.size() - m_layout.Size(),m_layout };
	}
	Vertex VertexData::Front()
	{
		assert(m_buffer.size() != 0u);
		return Vertex{ m_buffer.data(),m_layout };
	}
	Vertex VertexData::operator[](size_t i)
	{
		assert(i < Size());
		return Vertex{ m_buffer.data() + m_layout.Size() * i,m_layout };
	}

	ConstVertex VertexData::Back() const
	{
		return const_cast<VertexData*>(this)->Back();
	}
	ConstVertex VertexData::Front() const
	{
		return const_cast<VertexData*>(this)->Front();
	}
	ConstVertex VertexData::operator[](size_t i) const
	{
		return const_cast<VertexData&>(*this)[i];
	}










}