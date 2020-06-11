#pragma once

#include "Hazel/Renderer/Texture.h"
#include "Platform/D3D11/D3D11Context.h"

#include <d3d11.h>
#include <wrl.h>

namespace Hazel {

	class D3D11Texture2D : public Texture2D
	{
	public:
		D3D11Texture2D(uint32_t width, uint32_t height);
		D3D11Texture2D(const std::string& path);
		virtual ~D3D11Texture2D();

		virtual uint32_t GetWidth() const override { return m_Widht; }
		virtual uint32_t GetHeight() const override { return m_Height; }

		virtual void SetData(void* data, uint32_t size) override;
		
		virtual void Bind(uint32_t) const override;
		virtual void SetSlot(uint32_t slot) override { m_slot = slot; }

		virtual void Bind() const override { Bind(m_slot); }
		virtual void Unbind() const override;

		virtual bool operator==(const Texture& other) const override { return false; }
		//{
		//	return m_RendererID == ((D3D11Texture2D&)other).m_RendererID;
		//};
	private:
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pTextureView;
		std::string m_Path;
		uint32_t m_Widht, m_Height;
		//GLenum m_InternalFormat, m_DataFormat;
		uint32_t m_slot;
		std::string m_tag;
	public:
		D3D11Context& _c;
	};

}
