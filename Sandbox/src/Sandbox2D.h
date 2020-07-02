#pragma once

#include "Hazel.h"

class Sandbox2D :public Hazel::Layer
{
public:
	Sandbox2D();
	virtual ~Sandbox2D() = default;

	virtual void OnAttach() override;
	virtual void OnDetach() override;

	void OnUpdate(Hazel::Timestep ts) override;
	virtual void OnImGuiRender() override;
	void OnEvent(Hazel::Event& e) override;
private:
	Hazel::OrthographicCameraController m_CameraController;

	// Temp
	Hazel::Ref<Hazel::VertexArray> m_SquareVA;
	Hazel::Ref<Hazel::Shader> m_FlatColorShader;

	Hazel::Ref<Hazel::Texture2D> m_CheckerboardTexture;

	Hazel::Ref<Hazel::PipelineSpecification> PipeSpec1;
	Hazel::Ref<Hazel::PipelineSpecification> PipeSpec2;
	Hazel::Ref<Hazel::VertexBuffer> m_vbk;
	Hazel::Ref<Hazel::IndexBuffer> m_ibk;
	Hazel::Ref<Hazel::ConstantBuffer> m_cb;

	glm::vec4 m_SquareColor = { 0.2f, 0.3f, 0.8f, 1.0f };
	
};
