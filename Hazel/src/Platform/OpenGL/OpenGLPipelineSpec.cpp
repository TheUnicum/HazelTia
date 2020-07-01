#include "hzpch.h"
#include "OpenGLPipelineSpec.h"

#include "Platform/OpenGL/OpenGLVertexLayout.h"
#include "Platform/OpenGL/OpenGLVertexArray.h"

namespace Hazel {

	OpenGLPipelineSpec::OpenGLPipelineSpec(const PipelineCreateInfo& createInfo)
		: PipelineSpecification(GraphicsContext::Get_Active())
	{
		m_spec = createInfo;
		//m_Pipeline = CreateRef<Pipeline>(_c);
		//ReCreate();
	}

	OpenGLPipelineSpec::~OpenGLPipelineSpec()
	{
	}

	void OpenGLPipelineSpec::Bind() const
	{
		//ReCreate();

		//OpenGLVertexArray va;
		//va.Bind();

		m_spec.shader->Bind(); // Nothing in vulkan
		std::dynamic_pointer_cast<OpenGLVertexLayout>(m_spec.vertexLayout)->Bind();
		//m_Pipeline->Bind();
	}

	void OpenGLPipelineSpec::ReCreate() const
	{
	}

}