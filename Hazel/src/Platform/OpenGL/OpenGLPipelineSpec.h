#pragma once

#include "Hazel/Renderer/PipelineSpecification.h"



namespace Hazel {

	class OpenGLPipelineSpec : public PipelineSpecification
	{
	public:
		OpenGLPipelineSpec(const PipelineCreateInfo& createInfo);
		virtual ~OpenGLPipelineSpec();

		virtual void Bind() const override;
		virtual void Unbind() const override {};

		void ReCreate() const;

	private:
		std::string m_Name;
	public:
	};

}

