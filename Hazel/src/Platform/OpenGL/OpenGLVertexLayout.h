#pragma once
#include "Hazel/Renderer/VertexLayout.h"

#include <glad/glad.h>

namespace Hazel
{
	static GLenum ShaderDataTypeToOpenGLBaseType(VertexLayout::ElementType type)
	{
		switch (type)
		{
			case VertexLayout::ElementType::AP_FLOAT:    return GL_FLOAT;
			case VertexLayout::ElementType::AP_FLOAT2:   return GL_FLOAT;
			case VertexLayout::ElementType::AP_FLOAT3:   return GL_FLOAT;
			case VertexLayout::ElementType::AP_FLOAT4:   return GL_FLOAT;
			case VertexLayout::ElementType::AP_INT:      return GL_INT;
			case VertexLayout::ElementType::AP_INT2:     return GL_INT;
			case VertexLayout::ElementType::AP_INT3:     return GL_INT;
			case VertexLayout::ElementType::AP_INT4:     return GL_INT;
		}

		HZ_CORE_ASSERT(false, "ShaderCode::ShaderType!");
		return 0;
	}


	class OpenGLVertexLayout : public VertexLayout
	{
	public:
		void Bind()
		{
			int m_VertexBufferIndex = 0;

			for (auto& element : m_elements)
			{
				auto a1 = s_get_EleComponenCountof(element.GetType());
				auto a2 = ShaderDataTypeToOpenGLBaseType(element.GetType());
				auto a3 = element.GetNormalize() ? GL_TRUE : GL_FALSE;
				auto a4 = GetStride();
				auto a5 = (const void*)element.GetOffset();

				glEnableVertexAttribArray(m_VertexBufferIndex);
				glVertexAttribPointer(m_VertexBufferIndex,
					s_get_EleComponenCountof(element.GetType()),
					ShaderDataTypeToOpenGLBaseType(element.GetType()),
					element.GetNormalize() ? GL_TRUE : GL_FALSE,
					GetStride(),
					(const void*)element.GetOffset());
				m_VertexBufferIndex++;
			}
		}
	private:

	};

}
