#pragma once

#include "Hazel/Renderer/Buffer.h"

namespace Hazel {

	class OpenGLVertexBuffer : public VertexBuffer
	{
	public:
		OpenGLVertexBuffer(uint32_t size);
		OpenGLVertexBuffer(std::string& tag, uint32_t size);
		OpenGLVertexBuffer(float* vertices, uint32_t size);
		OpenGLVertexBuffer(std::string& tag, float* vertices, uint32_t size);
		virtual ~OpenGLVertexBuffer();
		void InitDynamic(uint32_t size);
		void InitStatic(float* vertices, uint32_t size);

		virtual void Bind() const override;
		virtual void Unbind() const override;

		virtual void SetData(const void* data, uint32_t size) override;

		virtual const BufferLayout& GetLayout() const override { return m_Layout; }
		virtual void SetLayout(const BufferLayout& layout) override { m_Layout = layout; }
		virtual std::string GetUID() const override { return GenerateUID_(m_tag); }
	private:
		uint32_t m_RendererID;
		BufferLayout m_Layout;
		std::string m_tag;
	};

	class OpenGLIndexBuffer : public IndexBuffer
	{
	public:
		OpenGLIndexBuffer(uint32_t* indices, uint32_t count);
		OpenGLIndexBuffer(std::string& tag, uint32_t* indices, uint32_t count);
		virtual ~OpenGLIndexBuffer();
		void Init(uint32_t* indices, uint32_t count);

		virtual void Bind() const override;
		virtual void Unbind() const override;

		virtual uint32_t GetCount() const override { return m_Count; }
		virtual std::string GetUID() const override { return GenerateUID_(m_tag); }
	private:
		uint32_t m_RendererID;
		uint32_t m_Count;
		std::string m_tag;
	};

}
