#include "Sandbox2D.h"
#include <imgui/imgui.h>

#include "glm/gtc/matrix_transform.hpp"
#include <glm/gtc/type_ptr.inl>

#include <memory>

#include "Platform/D3D11/D3D11Context.h"
#include "Platform/OpenGL/OpenGLVertexArray.h"

#include <d3dcompiler.h>
#include <DirectXMath.h>
#include "Hazel/VetexGeometryFactory/Plane.h"
namespace wrl = Microsoft::WRL;
namespace dx = DirectX;


#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <chrono>

Sandbox2D::Sandbox2D()
	: Layer("Sandbox2D"), m_CameraController(1280.0f / 720.0f), m_SquareColor({ 0.2f, 0.3f, 0.8f, 1.0f })
{
}

void Sandbox2D::OnAttach()
{
	HZ_PROFILE_FUNCTION();

	m_CheckerboardTexture = Hazel::Texture2D::Create("assets/textures/Checkerboard.png");

	{
		Hazel::VertexLayout::ElementType::AP_FLOAT2;

		//Hazel::BufferLayout layout = {
		//	{ Hazel::ShaderDataType::Float3, "a_Position"},
		//	{ Hazel::ShaderDataType::Float4, "a_Color"},
		//};
		
		Hazel::VertexLayout vl = {
			{ Hazel::VertexLayout::ElementType::AP_FLOAT3, "a_Position"},
			{ Hazel::VertexLayout::ElementType::AP_FLOAT4, "a_Color"},
		};

		vl.Append(Hazel::VertexLayout::ElementType::AP_FLOAT4, "a_Color2");
		vl;
		
		auto c = vl.Resolve("a_Color2");
		auto conunt = vl.GetElementCount();
		auto stride = vl.Size();
		auto s = vl.ResolveByIndex(1);
		auto s2 = vl.ResolveByIndex(2);


		using namespace Hazel;
		VertexLayout vl2;
		vl2.Append(VertexLayout::AP_FLOAT3, "Position3D")
			.Append(VertexLayout::AP_FLOAT4, "Color_4");
		
		auto desc = vl2.GetD3DLayout();
		
		{
			VertexData vb(std::move(vl2));
			vb.EmplaceBack(glm::vec3(1.23f, 4.5, 3), glm::vec4(1.0f));
			vb.EmplaceBack(glm::vec3(1.23f, 4.5, 3), glm::vec4(1.0f));
			vb.EmplaceBack(glm::vec3(.23f, 4.5, 3), glm::vec4(4.4f));
			auto pos = vb[0].Attr<VertexLayout::AP_FLOAT3>("Position3D");
			//glm::vec4 pos32 = *(glm::vec4*)(vb[2].Attrp("Color_4"));


			const auto& cvb = vb;
			auto& pos2 = cvb[0].Attr<VertexLayout::AP_FLOAT3>("Position3D");
			//pos2 = glm::vec3(3);
			//pos = vb[0].Attr<VertexLayout::AP_FLOAT3>("Position3D");
		}
	}


	{
		std::shared_ptr<Hazel::GraphicsContext> cc = Hazel::GraphicsContext::Resolve(Hazel::Application::Get().GetWindowTest(0));
		cc->MakeCurrent();

		using namespace Hazel;	
		Hazel::Ref<VertexLayout> vl2 = Hazel::VertexLayout::Create();
		//vl2->Append(VertexLayout::AP_FLOAT3, "inPosition2")
		//	.Append(VertexLayout::AP_FLOAT3, "inColor3")
		//	.Append(VertexLayout::AP_FLOAT2, "inTexCoord");
		//vl2->Append(VertexLayout::AP_FLOAT2, "Position")
		//	.Append(VertexLayout::AP_FLOAT3, "Color");

		//Ref<ShaderCode> sc = ShaderCode::Create("assets/shaders/Vulkan/FragColor_VB.glsl");
		//Ref<ShaderCode> sc = ShaderCode::Create("assets/shaders/Vulkan/FragColor_CBuff.glsl");
		Ref<ShaderCode> sc = ShaderCode::Create("assets/shaders/Vulkan/FragColor_CBuff_Text.glsl");
		//auto gl = sc->GetVertexLayoutEleList();
		//auto hl = sc->GetVertexLayoutEleListHLSL();
		//auto s = sc->GetCodeHLSL();
		auto ssREd = Hazel::Shader::Create(sc);

		//auto ssREd = Hazel::Shader::Create("assets/shaders/D3D/Mattia2.hlsl");

		//auto c = sc->GetCodeGLSL();
		//auto hs = sc->GetCodeHLSL(); 
		//auto v = sc->GetVertexLayoutEleList();


		//m_tex = Hazel::Texture2D::Create("assets/textures/texture.jpg");
		m_tex = Hazel::Texture2D::Create("assets/textures/Checkerboard.png");
		//m_tex = Hazel::Texture2D::Create("assets/textures/ChernoLogo.png");
		struct UniformBufferObject {
			glm::mat4 model;
			glm::mat4 view;
			glm::mat4 proj;
		};
		UniformBufferObject ubo{};
		ubo.model = glm::rotate(glm::mat4(1.0f),  glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		ubo.proj = glm::perspective(glm::radians(45.0f), (float)800 / (float)600, 0.1f, 10.0f);
		ubo.proj[1][1] *= -1;
		auto d = sizeof(UniformBufferObject);
		m_cb = Hazel::ConstantBuffer::Create(sizeof(UniformBufferObject), &ubo);
		m_cb->SetSlot(0, 1);



		Hazel::PipelineCreateInfo createInfo;// { Hazel::Shader::Create("assets/shaders/Vulkan/FragColor.glsl"), nullptr};
		createInfo.shader = ssREd;
		createInfo.vertexLayout = nullptr;//  vl2;
		createInfo.constantBuffer = m_cb;
		createInfo.texture = m_tex;
		PipeSpec2 = Hazel::PipelineSpecification::Create(createInfo);
		//PipeSpec2->Bind();

		struct VertexPos {
			glm::vec3 pos;
			glm::vec3 color;
			glm::vec2 texCoord;
		};
		const std::vector<VertexPos> vertices = {
			{{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
			{{0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
			{{0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
			{{-0.5f, 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}},

			{{-0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
			{{0.5f, -0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
			{{0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
			{{-0.5f, 0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}}
		};
		const std::vector<uint32_t> indices =
		{
		//0, 1, 2, 2, 3, 0
		//0, 1, 2, 2, 0, 3,
		0, 1, 2, 2, 3, 0,
		4, 5, 6, 6, 7, 4
		};


		//Hazel::Ref<Hazel::VertexBuffer> vbk = Hazel::VertexBuffer::Create((float*)vertices.data(), (uint32_t)(sizeof(Vertex) * vertices.size()));
		m_vbk = Hazel::VertexBuffer::Create((float*)vertices.data(), (uint32_t)(sizeof(VertexPos) * vertices.size()));
		m_ibk = Hazel::IndexBuffer::Create((uint32_t*)indices.data(), indices.size());
		m_vbk->SetVertexLayout(PipeSpec2->GetVertexLayout());
	}
}

void Sandbox2D::OnDetach()
{
	HZ_PROFILE_FUNCTION();
}

void Sandbox2D::OnUpdate(Hazel::Timestep ts)
{
	HZ_PROFILE_FUNCTION();

	// Update
	m_CameraController.OnUpdate(ts);

	// Render
	Hazel::Renderer2D::ResetStats();
	{
		HZ_PROFILE_SCOPE("Renderer Prep");
		Hazel::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });
		Hazel::RenderCommand::Clear();
	}

	{
		static float rotation = 0.0f;
		rotation += ts * 50.0f;

		HZ_PROFILE_SCOPE("Renderer Draw");
		Hazel::Renderer2D::BeginScene(m_CameraController.GetCamera());
		Hazel::Renderer2D::DrawRotatedQuad({ 1.0f, 0.0f }, { 0.8f, 0.8f }, -45.0f, { 0.8f, 0.2f, 0.3f, 1.0f });
		Hazel::Renderer2D::DrawQuad({ -1.0f, 0.0f }, { 0.8f, 0.8f }, { 0.8f, 0.2f, 0.3f, 1.0f });
		Hazel::Renderer2D::DrawQuad({ 0.5f, -0.5f }, { 0.5f, 0.75f }, m_SquareColor);
		Hazel::Renderer2D::DrawQuad({ 0.0f, 0.0f, -0.1f }, { 10.0f, 10.0f }, m_CheckerboardTexture, 10.0f);
		Hazel::Renderer2D::DrawQuad({ 0.0f, 0.0f, -0.1f }, { 20.0f, 20.0f }, m_CheckerboardTexture, 10.0f);
		Hazel::Renderer2D::EndScene();

		Hazel::Renderer2D::BeginScene(m_CameraController.GetCamera());
		for (float y = -5.0f; y < 5.0f; y += 0.5f)
		{
			for (float x = -5.0f; x < 5.0f; x += 0.5f)
			{
				glm::vec4 color = { (x + 5.0f) / 10.0f, 0.4f, (y + 5.0f) / 10.0f, 0.7f };
				Hazel::Renderer2D::DrawQuad({ x, y }, { 0.45f, 0.45f }, color);
			}
		}
		Hazel::Renderer2D::EndScene();


		// Test Vulkan
		{
			//static float rotation = 0.0f;
			//rotation += (ts * 50.0f);
			//if (int(rotation) % 100 < 50)
			//{
			//	PipeSpec1->Bind();
			//	Hazel::RenderCommandX::MakeContextCurrent(Hazel::Application::Get().GetWindowTest(0));
			//
			//	Hazel::RenderCommandX::Clear();
			//	Hazel::RenderCommandX::DrawArray();
			//}
			//else
			static auto startTime = std::chrono::high_resolution_clock::now();

			auto currentTime = std::chrono::high_resolution_clock::now();
			float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();
			
			struct UniformBufferObject {
				glm::mat4 model;
				glm::mat4 view;
				glm::mat4 proj;
			};
			UniformBufferObject ubo{};
			ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
			ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
			ubo.proj = glm::perspective(glm::radians(45.0f), (float)800 / (float)600, 0.1f, 10.0f);
			ubo.proj[1][1] *= -1;



			{

				std::shared_ptr<Hazel::GraphicsContext> cc = Hazel::GraphicsContext::Resolve(Hazel::Application::Get().GetWindowTest(0));
				cc->MakeCurrent();

				//std::dynamic_pointer_cast<Hazel::D3D11Context>(cc)->ClearBuffer_impl(.2f, 0.1f, 0.1f);
				//Hazel::RenderCommandX::Clear();
				//
				//				
				Hazel::RenderCommandX::MakeContextCurrent(Hazel::Application::Get().GetWindowTest(0));
				//m_vbk->BindTemp(PipeSpec2->m_spec.vertexLayout->GetStride()); //[Da sistemare x D3D]
				m_cb->Update(&ubo, sizeof(UniformBufferObject));
				//m_cb->SetSlot(0, 1);
				PipeSpec2->Bind();
				m_vbk->Bind();
				m_ibk->Bind();

				Hazel::RenderCommandX::CmdDrawIndexted(m_ibk->GetCount());
				//std::dynamic_pointer_cast<Hazel::D3D11Context>(cc)->CmdDrawIndexted_impl(6);
				//std::dynamic_pointer_cast<Hazel::D3D11Context>(cc)->DrawTriangle_impl2(0);
				///----2
				
				
				///-----




				//std::dynamic_pointer_cast<Hazel::D3D11Context>(cc)->GetPP().m_pContext->DrawIndexed((UINT)(m_ibk->GetCount()), 0, 0);

				//Hazel::RenderCommandX::CmdDrawIndexted(6);
				//std::dynamic_pointer_cast<Hazel::D3D11Context>(cc)->GetPP().m_pContext->DrawIndexed((UINT)(6), 0, 0);
				//std::dynamic_pointer_cast<Hazel::D3D11Context>(cc)->GetPP().m_pContext->Draw(3, 0);

			}
			//std::shared_ptr<Hazel::GraphicsContext> cc = Hazel::GraphicsContext::Resolve(Hazel::Application::Get().GetWindowTest(0));
			//cc->MakeCurrent();
			////cc->Bind();
			//
			//auto ss =  Hazel::Shader::Create("assets/shaders/Vulkan/FragColor.glsl");
			//
			//Hazel::PipelineCreateInfo createInfo;// { Hazel::Shader::Create("assets/shaders/Vulkan/FragColor.glsl"), nullptr};
			//createInfo.shader = ss;
			//Hazel::Ref<Hazel::PipelineSpecification> PipeSpec = Hazel::PipelineSpecification::Create(createInfo);
			//PipeSpec->Bind();
				//auto m_Pipeline = Ref<Pipeline>(*this);
				//
				//PipelineSpecification pipSpec;
				//pipSpec.shader = Shader::Create("assets/shaders/Vulkan/FragColor.glsl");
				//
				//m_Pipeline->SetSpec(pipSpec);
				//m_Pipeline->Bind();



			//std::dynamic_pointer_cast<Hazel::VulkanContext>(cc)->Bind()

			//Hazel::Ref<Hazel::Shader> sh0 = Hazel::Shader::Create("assets/shaders/Vulkan/FragColor.glsl");
			//Hazel::pipeli PipelineSpecification pspec;
			// pspec.shader = std::make_shared<Shader>(*this, "src/assets/shaders/FlatColor.glsl");
			//pspec.bufferLayout = std::make_shared<BufferLayout>();
			//pspec.uniformBuffer = m_ubo;


			//cc->ClearBuffer_impl(0.1f, 0.1f, 0.1f);
			//cc->DrawTriangle_impl(rotation);
			//cc->DrawTriangle_impl(-rotation);
		}


		// Test Direc3D 11
		//{
		//	static float rotation = 0.0f;
		//	rotation += glm::radians(ts * 50.0f);
		//	std::shared_ptr<Hazel::GraphicsContext> cc = Hazel::GraphicsContext::Resolve(Hazel::Application::Get().GetWindowTest(0));
		//	cc->MakeCurrent();
		//	cc->ClearBuffer_impl(0.1f, 0.1f, 0.1f);
		//	cc->DrawTriangle_impl(rotation);
		//	//cc->DrawTriangle_impl(-rotation);
		//}


		// OpenGL
		//{
		//	static float rotation = 0.0f;
		//	rotation += glm::radians(ts * 50.0f);
		//	std::shared_ptr<Hazel::GraphicsContext> cc = Hazel::GraphicsContext::Resolve(Hazel::Application::Get().GetWindowTest(1));
		//	cc->MakeCurrent();
		//	cc->ClearBuffer_impl(0.1f, 0.1f, 0.1f);
		//	cc->DrawTriangle_impl(rotation);
		//	//cc->DrawTriangle_impl(-rotation);
		//}
		//std::shared_ptr<Hazel::GraphicsContext> cc = Hazel::GraphicsContext::Resolve(Hazel::Application::Get().GetWindow());
		//cc->MakeCurrent();
	}
}

void Sandbox2D::OnImGuiRender()
{
	HZ_PROFILE_FUNCTION();

	ImGui::Begin("Settings");

	auto stats = Hazel::Renderer2D::GetStats();
	ImGui::Text("Renderer2D Stats: ");
	ImGui::Text("Draw Calls: %d", stats.DrawCalls);
	ImGui::Text("Quads: %d", stats.QuadCount);
	ImGui::Text("Vertices: %d", stats.GetTotalVertexCount());
	ImGui::Text("Indices: %d", stats.GetTotalIndexCount());

	ImGui::ColorEdit4("SquareColor", glm::value_ptr(m_SquareColor));
	ImGui::End();
}

void Sandbox2D::OnEvent(Hazel::Event& e)
{
	m_CameraController.OnEvent(e);
}
