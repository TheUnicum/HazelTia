#include "hzpch.h"
#include "D3D11Texture.h"

#include <stb_image.h>

#define GFX_THROW_INFO(x) x

namespace Hazel {

	namespace wrl = Microsoft::WRL;

	D3D11Texture2D::D3D11Texture2D(uint32_t width, uint32_t height)
		: Texture2D(GraphicsContext::Get_Active()), _c((D3D11Context&)this->_ctx)
	{

	}

	D3D11Texture2D::D3D11Texture2D(const std::string& path)
		: Texture2D(GraphicsContext::Get_Active()), _c((D3D11Context&)this->_ctx),
		m_Path(path)//	slot(slot)
		{
			//INFOMAN(gfx);
			HZ_PROFILE_FUNCTION();
			

			int width, height, channels;
			stbi_set_flip_vertically_on_load(1);
			stbi_uc* data = nullptr;
			{
				HZ_PROFILE_SCOPE("stbi_load - D3D11Texture2D::D3D11Texture2D(const std:string&)");
				data = stbi_load(path.c_str(), &width, &height, &channels, 4);
			}
			HZ_CORE_ASSERT(data, "Failed to load image!");
			m_Widht = width;
			m_Height = height;

			bool hasAlpha = (channels == 4);

			// create texture resource
			D3D11_TEXTURE2D_DESC textureDesc = {};
			textureDesc.Width = m_Widht;
			textureDesc.Height = m_Height;
			textureDesc.MipLevels = 0;
			textureDesc.ArraySize = 1;
			textureDesc.Format = DXGI_FORMAT_B8G8R8X8_UNORM; //DXGI_FORMAT_B8G8R8A8_UNORM;
			textureDesc.SampleDesc.Count = 1;
			textureDesc.SampleDesc.Quality = 0;
			textureDesc.Usage = D3D11_USAGE_DEFAULT;
			textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
			textureDesc.CPUAccessFlags = 0;
			textureDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;
			wrl::ComPtr<ID3D11Texture2D> pTexture;
			GFX_THROW_INFO(_c.GetPP().m_pDevice->CreateTexture2D(
				&textureDesc, nullptr, &pTexture
			));

			//// write image data into top mip level
			//_c.GetPP().m_pContext->UpdateSubresource(
			//	pTexture.Get(), 0u, nullptr, data, GetWidth() * sizeof(float), 0u
			//);
			_c.GetPP().m_pContext->UpdateSubresource(
				pTexture.Get(), 0u, nullptr, data, m_Widht * 4, 0u
			);


			// create the resource view on the texture
			D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
			srvDesc.Format = textureDesc.Format;
			srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			srvDesc.Texture2D.MostDetailedMip = 0;
			srvDesc.Texture2D.MipLevels = -1;
			GFX_THROW_INFO(_c.GetPP().m_pDevice->CreateShaderResourceView(
				pTexture.Get(), &srvDesc, &m_pTextureView
			));

			// generate the mip chain using the gpu rendering pipeline
			_c.GetPP().m_pContext->GenerateMips(m_pTextureView.Get());


			// Sampler state
			D3D11_SAMPLER_DESC samplerDesc = CD3D11_SAMPLER_DESC{ CD3D11_DEFAULT{} };
			samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR; //D3D11_FILTER_ANISOTROPIC;
			samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
			samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
			samplerDesc.MaxAnisotropy = D3D11_REQ_MAXANISOTROPY;

			_c.GetPP().m_pDevice->CreateSamplerState(&samplerDesc, &m_pSampler);
	}

	D3D11Texture2D::~D3D11Texture2D()
	{
	}

	void D3D11Texture2D::SetData(void* data, uint32_t size)
	{
	}

	void D3D11Texture2D::Bind(uint32_t slot) const
	{
		_c.GetPP().m_pContext->PSSetShaderResources(slot, 1u, m_pTextureView.GetAddressOf());

		_c.GetPP().m_pContext->PSSetSamplers(0, 1, m_pSampler.GetAddressOf());
	}

	void D3D11Texture2D::Unbind() const
	{
	}

}
