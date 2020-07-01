#pragma once
#include "Hazel/Renderer/VertexLayout.h"

#include <glad/glad.h>

namespace Hazel
{
	//static GLenum ShaderDataTypeToOpenGLBaseType(VertexLayout::ElementType type)
	//{
	//	switch (type)
	//	{
	//	case VertexLayout::ElementType::AP_FLOAT:    return GL_FLOAT;
	//	case VertexLayout::ElementType::AP_FLOAT2:   return GL_FLOAT;
	//	case VertexLayout::ElementType::AP_FLOAT3:   return GL_FLOAT;
	//	case VertexLayout::ElementType::AP_FLOAT4:   return GL_FLOAT;
	//	case VertexLayout::ElementType::AP_INT:      return GL_INT;
	//	case VertexLayout::ElementType::AP_INT2:     return GL_INT;
	//	case VertexLayout::ElementType::AP_INT3:     return GL_INT;
	//	case VertexLayout::ElementType::AP_INT4:     return GL_INT;
	//	}
	//
	//	HZ_CORE_ASSERT(false, "ShaderCode::ShaderType!");
	//	return 0;
	//}


	class D3D11VertexLayout : public VertexLayout
	{
	public:
		void Bind()
		{
			int m_VertexBufferIndex = 0;

			for (auto& element : m_elements)
			{

			}
		}

		const std::vector<D3D11_INPUT_ELEMENT_DESC> GetElementsDesc()
		{
			// I could create on beginning, Store and pass a reference!
			std::vector<D3D11_INPUT_ELEMENT_DESC> eleDesc;

			auto c = GetD3DLayout();

			//eleDesc.push_back({ "Position",0,DXGI_FORMAT_R32G32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 });
			//eleDesc.push_back({ "Color",0,DXGI_FORMAT_R32G32B32_FLOAT,0,8,D3D11_INPUT_PER_VERTEX_DATA,0 });
			return c;
		}

		const std::vector<D3D11_INPUT_ELEMENT_DESC> GetElementsDescAuto()
		{
			// I could create on beginning, Store and pass a reference!
			auto c = GetD3DLayout();
			size_t i = 0;
			for (auto& ele : c)
			{
				ele.SemanticName = "TEXCOORD";
				ele.SemanticIndex = i;
				i++;
			}

			return c;
		}

	private:

	};

}
