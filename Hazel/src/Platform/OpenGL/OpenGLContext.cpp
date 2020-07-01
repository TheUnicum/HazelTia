#include "hzpch.h"
#include "Platform/OpenGL/OpenGLContext.h"

#include "Hazel/Core/Window.h"

#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective

#include "Hazel.h"
#include "Hazel/Renderer/ConstantBuffer.h"

// Test
#include "Hazel/VetexGeometryFactory/Cube.h"
#include "Hazel/VetexGeometryFactory/Cone.h"
#include "Hazel/VetexGeometryFactory/Prism.h"
#include "Hazel/VetexGeometryFactory/Plane.h"
#include "Hazel/VetexGeometryFactory/Sphere.h"
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective

#include <d3dcompiler.h>
#include <DirectXMath.h>

namespace wrl = Microsoft::WRL;
namespace dx = DirectX;

namespace Hazel {

	OpenGLContext::OpenGLContext(Window& window)
		: m_window(window), m_windowHandle((GLFWwindow*)window.GetNativeWindow())
	{
		HZ_CORE_ASSERT(m_windowHandle, "Window handle is null!");
	}

	void OpenGLContext::Init()
	{
		HZ_PROFILE_FUNCTION();

		glfwMakeContextCurrent(m_windowHandle);
		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		HZ_CORE_ASSERT(status, "Failed to initialize Glad!");

		HZ_CORE_INFO("OpenGL Info:");
		HZ_CORE_INFO("  Vendor: {0}", glGetString(GL_VENDOR));
		HZ_CORE_INFO("  Renderer: {0}", glGetString(GL_RENDERER));
		HZ_CORE_INFO("  Version: {0}", glGetString(GL_VERSION));

		#ifdef HZ_ENABLE_ASSERTS
		int versionMajor;
		int versionMinor;
		glGetIntegerv(GL_MAJOR_VERSION, &versionMajor);
		glGetIntegerv(GL_MINOR_VERSION, &versionMinor);

		HZ_CORE_ASSERT(versionMajor > 4 || (versionMajor == 4 && versionMinor >= 5), "Hazel requires al least OpenGL version 4.5!");
		#endif
	}

	void OpenGLContext::SwapBuffers()
	{
		HZ_PROFILE_FUNCTION();

		glfwSwapBuffers(m_windowHandle);
	}

	API OpenGLContext::MakeCurrent()
	{
		if (m_windowHandle != glfwGetCurrentContext())
		{
			glfwMakeContextCurrent(m_windowHandle);
		}
		if (_s_active.get() != this)
		{
			_s_active = Resolve(API::OpenGL, m_window); // I need to use RESOLVE to 
		}
		return GetAPI();
	}

	void OpenGLContext::ClearBuffer_impl(float red, float green, float blue)
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(red, green, blue, 1.0f);
	}

	void OpenGLContext::CmdClear_impl()
	{
		ClearBuffer_impl(0.1f, 0.1f, 0.1f);
		//HZ_CORE_ASSERT(false, "Function Still NOT implemented!");

	}

	void OpenGLContext::CmdDrawArrays_impl(uint32_t vertexCount, uint32_t offset)
	{
		HZ_CORE_ASSERT(false, "Function Still NOT implemented!");

	}

	void OpenGLContext::CmdDrawArraysInstanced_impl(uint32_t vertexCount, uint32_t instanceCount)
	{
		HZ_CORE_ASSERT(false, "Function Still NOT implemented!");

	}

	void OpenGLContext::CmdDrawIndexted_impl(uint32_t indexCount, uint32_t offset)
	{
		glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, nullptr);
	}

	void OpenGLContext::CmdDrawIndextedInstanced_impl(uint32_t indexCount, uint32_t instanceCount)
	{
		HZ_CORE_ASSERT(false, "Function Still NOT implemented!");

	}

	void OpenGLContext::DrawTriangle_impl(float angle)
	{
		glEnable(GL_CULL_FACE);
		glCullFace(GL_FRONT);

		#define GFX_THROW_INFO(x) x
		#define GFX_THROW_INFO_ONLY(x) x

		// generator
		struct VertexPos
		{
			glm::vec3 pos;
			glm::vec2 tex;
		};

		//auto c = Prism::MakeTesselated<VertexPos>(24);
		//auto c = Sphere::Make<VertexPos>();
		auto c = Plane::Make<VertexPos>();
		c.Transform(glm::scale(glm::mat4(1.0f), glm::vec3(3.0f, 3.0f, 3.0f)));

		//auto c = Plane::Make<VertexPos>();
		//c.Transform(glm::rotate(glm::mat4(1.0f), PI / 2, glm::vec3(1.0f, 0.0f, .0f)));

		Ref<VertexArray> va = VertexArray::Create();
		
		Ref<VertexBuffer> vb = VertexBuffer::Create((float*)&c.vertices[0], uint32_t(sizeof(VertexPos) * c.vertices.size()));
		vb->SetLayout({
			{ShaderDataType::Float3, "a_Position"},
			{ShaderDataType::Float2, "a_TexCoord"},
			});

		va->AddVertexBuffer(vb);
		// vb->BindTemp(sizeof(VertexPos));   D3D Temp



		Ref<IndexBuffer> ibuff = IndexBuffer::Create(&c.indices[0], (uint32_t)c.indices.size());
		//ibuff->Bind();
		va->SetIndexBuffer(ibuff);

		// create constant buffer for transformation matrix
		struct ConstantBuffer
		{
			//dx::XMMATRIX transformation;
			glm::mat4 transformation;
		
		};
		const ConstantBuffer cb = // This is a CCW rotation not a transposd!!!!!
		{
				glm::perspectiveFovLH(glm::radians(70.0f), 800.0f, 600.0f, .5f, 10.f) *
				glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 4)) *
				glm::rotate(glm::mat4(1.0f), angle, glm::vec3(1, 0, 0))*
				glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0, 0, 1))
		};
		
		Ref<ConstanBuffer> cBuff = ConstanBuffer::Create(cb);
		cBuff->SetSlot(0, 0);
		cBuff->Bind();


		// lookup table for cube face colors
		struct ConstantBuffer2
		{
			struct
			{
				float r;
				float g;
				float b;
				float a;
			} face_colors[6];
		};
		const ConstantBuffer2 cb2 =
		{
			{
				{1.0f,0.0f,1.0f},
				{1.0f,0.0f,0.0f},
				{0.0f,1.0f,0.0f},
				{0.0f,0.0f,1.0f},
				{1.0f,1.0f,0.0f},
				{0.0f,1.0f,1.0f},
			}
		};
		Ref<ConstanBuffer> cBuff2 = ConstanBuffer::Create(cb2);
		cBuff2->SetSlot(0, 1);
		cBuff2->Bind();



		//Ref<Shader> shader = (Shader::Resolve("assets/shaders/D3D/FlatColor.hlsl"));
		//Ref<Shader> shader = Shader::Resolve("assets/shaders/OpengGL/FlatColor.glsl");

		//Ref<Shader> shader1 = Shader::Create("assets/shaders/OpenGL/FlatColor.glsl");
		Ref<Shader> shader1 = Shader::Create("assets/shaders/OpenGL/Texture.glsl");

		uint32_t id = shader1->GetID();
		unsigned int sIdx_matrices = glGetUniformBlockIndex(shader1->GetID(), "Matrices");
		glUniformBlockBinding(shader1->GetID(), sIdx_matrices, 0);

		unsigned int sIdx_Colors = glGetUniformBlockIndex(shader1->GetID(), "BufColors");
		glUniformBlockBinding(shader1->GetID(), sIdx_Colors, 1);


		auto pBlob = shader1->GetpShaderBytecode();
		shader1->Bind();


		//// input (vertex) layout (2d position only)
		//wrl::ComPtr<ID3D11InputLayout> pInputLayout;
		//const D3D11_INPUT_ELEMENT_DESC ied[] =
		//{
		//{ "Position",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 },
		//{ "TexCoord",0,DXGI_FORMAT_R32G32_FLOAT,0,12,D3D11_INPUT_PER_VERTEX_DATA,0 },
		//
		//};
		//GFX_THROW_INFO(ppD3D.m_pDevice->CreateInputLayout(
		//	ied, (UINT)std::size(ied),
		//	pBlob->GetBufferPointer(),
		//	pBlob->GetBufferSize(),
		//	&pInputLayout
		//));

		// bind vertex layout
		///ppD3D.m_pContext->IASetInputLayout(pInputLayout.Get());

		// bind render target
		//ppD3D.m_pContext->OMSetRenderTargets(1u, ppD3D.m_pTarget.GetAddressOf(), nullptr);

		// Set primitive topology to triangle list (group of 3 vertices)
		///ppD3D.m_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		//// configure viewport
		//D3D11_VIEWPORT vp;
		//vp.Width = 800;
		//vp.Height = 600;
		//vp.MinDepth = 0;
		//vp.MaxDepth = 1;
		//vp.TopLeftX = 0;
		//vp.TopLeftY = 0;
		//ppD3D.m_pContext->RSSetViewports(1u, &vp);



		// Texture
		Ref<Texture2D> texture = Texture2D::Create("assets/textures/Checkerboard.png");
		texture->Bind(0);


		va->Bind();
		//GFX_THROW_INFO_ONLY(ppD3D.m_pContext->Draw((UINT)std::size(vertices), 0u));
		//ppD3D.m_pContext->DrawIndexed((UINT)std::size(indices), 0, 0);
		//ppD3D.m_pContext->DrawIndexed((UINT)(c.indices.size()), 0, 0);
		glDrawElements(GL_TRIANGLES, (UINT)(c.indices.size()), GL_UNSIGNED_INT, nullptr);
	}

}
