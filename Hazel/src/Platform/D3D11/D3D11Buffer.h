#pragma once

#include "Hazel/Renderer/Buffer.h"
#include "Platform/D3D11/D3D11Context.h"

#include <d3d11.h>
#include <wrl.h>
#include <imstb_truetype.h>

namespace Hazel {

	class D3D11VertexBuffer : public VertexBuffer
	{
	public:
		D3D11VertexBuffer(uint32_t size);
		D3D11VertexBuffer(std::string& tag, uint32_t size);
		D3D11VertexBuffer(float* vertices, uint32_t size);
		D3D11VertexBuffer(std::string& tag, float* vertices, uint32_t size);
		virtual ~D3D11VertexBuffer();
	
		virtual void Bind() const override;
		virtual void Unbind() const override;

		virtual void BindTemp(uint32_t stride) const;

		virtual void SetData(const void* data, uint32_t size) override;
	
		virtual const BufferLayout& GetLayout() const override { return m_Layout; }
		virtual void SetLayout(const BufferLayout& layout) override { m_Layout = layout; }
	private:
		Microsoft::WRL::ComPtr<ID3D11Buffer> pVertexBuffer;
		BufferLayout m_Layout;
		std::string m_tag;
		uint32_t m_size = 0;
		mutable uint32_t m_stride = 0;
	public:
		D3D11Context& _c;
	};

	class D3D11IndexBuffer : public IndexBuffer
	{
	public:
		D3D11IndexBuffer(uint32_t* indices, uint32_t count);
		D3D11IndexBuffer(std::string& tag, uint32_t* indices, uint32_t count);
		virtual ~D3D11IndexBuffer();
		void Init(uint32_t* indices, uint32_t count);

		virtual void Bind() const override;
		virtual void Unbind() const override;

		virtual uint32_t GetCount() const override { return m_Count; }
		virtual std::string GetUID() const override { return GenerateUID_(m_tag); }
	private:
		Microsoft::WRL::ComPtr<ID3D11Buffer> pIndexBuffer;
		uint32_t m_Count;
		std::string m_tag;
	public:
		D3D11Context& _c;
	};

}
