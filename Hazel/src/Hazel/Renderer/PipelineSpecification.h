#pragma once

#include "Hazel/Renderer/Bindable.h"

#include "Buffer.h"
#include "Shader.h"

namespace Hazel {

	struct PipelineCreateInfo
	{
		Ref<Shader> shader;
		Ref<BufferLayout> bufferLayout;
		//std::shared_ptr<UniformBuffer> uniformBuffer;

		PipelineCreateInfo& operator=(const PipelineCreateInfo& other)
		{
			shader = other.shader;
			bufferLayout = other.bufferLayout;
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

	public:
		PipelineCreateInfo m_spec;

	// Classes Factory
	static Ref<PipelineSpecification> Create(const PipelineCreateInfo& createInfo);
	static Ref<PipelineSpecification> Create(GraphicsContext& ctx, const PipelineCreateInfo& createInfo);

	};

}
