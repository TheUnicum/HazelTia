#pragma once

#include "Hazel/Renderer/Bindable.h"

#include "Hazel/Renderer/Buffer.h"
#include "Hazel/Renderer/Shader.h"
#include "Hazel/Renderer/VertexLayout.h"
#include "Hazel/Renderer/ConstantBuffer.h"

namespace Hazel {

	struct PipelineCreateInfo
	{
		Ref<Shader> shader;
		Ref<BufferLayout> bufferLayout;
		Ref<VertexLayout> vertexLayout;
		Ref<ConstantBuffer> constantBuffer;

		PipelineCreateInfo& operator=(const PipelineCreateInfo& other)
		{
			shader = other.shader;
			bufferLayout = other.bufferLayout;
			if (other.vertexLayout)
			{
				vertexLayout = other.vertexLayout;
			}
			else
			{
				vertexLayout = VertexLayout::Create();
				for (auto& ele : shader->GetVertexLayoutEleList())
				{
					vertexLayout->Append(ele.type, ele.name.c_str());
				}
			}
			constantBuffer = other.constantBuffer;
			return *this;
		}
	};


	class PipelineSpecification : public Bindable
	{
	public:
		PipelineSpecification(GraphicsContext& ctx) : Bindable(ctx) {}
		~PipelineSpecification() = default;

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual VertexLayout& GetVertexLayout() { return *(m_spec.vertexLayout); }

	public:
		PipelineCreateInfo m_spec;

	// Classes Factory
	static Ref<PipelineSpecification> Create(const PipelineCreateInfo& createInfo);
	static Ref<PipelineSpecification> Create(GraphicsContext& ctx, const PipelineCreateInfo& createInfo);

	};

}
