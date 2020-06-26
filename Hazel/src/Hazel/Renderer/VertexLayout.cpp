#include "hzpch.h"
#include "VertexLayout.h"

namespace Hazel {

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