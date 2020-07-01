#pragma once

#include "Hazel/Renderer/PipelineSpecification.h"
#include "Platform/D3D11/D3D11Context.h"


namespace Hazel {

	class D3D11PipelineSpec : public PipelineSpecification
	{
	public:
		D3D11PipelineSpec(const PipelineCreateInfo& createInfo);
		virtual ~D3D11PipelineSpec();

		virtual void Bind() const override;
		virtual void Unbind() const override {};

		void ReCreate() const;

	private:
		std::string m_Name;
		bool m_Reflection_on_Shader;
	public:
		D3D11Context& _c;
	};

}

