#pragma once

#include "Hazel/Renderer/Buffer.h"
#include "Platform/D3D11/D3D11Context.h"

#include <d3d11.h>
#include <wrl.h>

namespace Hazel {

	//class D3D11VertexBuffer : public VertexBuffer
	//{
	//public:
	//	D3D11VertexBuffer(uint32_t size);
	//	D3D11VertexBuffer(float* vertices, uint32_t size);
	//	virtual ~D3D11VertexBuffer();
	//
	//	virtual void Bind() const override;
	//	virtual void Unbind() const override;
	//
	//	virtual void SetData(const void* data, uint32_t size) override;
	//
	//	//virtual const BufferLayout& GetLayout() const override { return m_Layout; }
	//	//virtual void SetLayout(const BufferLayout& layout) override { m_Layout = layout; }
	//private:
	//	uint32_t m_RendererID;
	//	BufferLayout m_Layout;
	//};

	class D3D11IndexBuffer : public IndexBuffer
	{
	public:
		D3D11IndexBuffer(uint32_t* indices, uint32_t count);
		virtual ~D3D11IndexBuffer();

		virtual void Bind() const override;
		virtual void Unbind() const override;

		virtual uint32_t GetCount() const override { return m_Count; }
	private:
		Microsoft::WRL::ComPtr<ID3D11Buffer> pIndexBuffer;
		uint32_t m_Count;
	public:
		D3D11Context& _c;
	};

}
