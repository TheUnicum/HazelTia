#pragma once
#include "Hazel/Renderer/Texture.h"
#include "Platform/Vulkan/VulkanContext.h"

namespace Hazel {

	class VulkanTexture2D : public Texture2D
	{
	public:
		VulkanTexture2D(uint32_t width, uint32_t height);
		VulkanTexture2D(const std::string& path);
		virtual ~VulkanTexture2D();

		virtual uint32_t GetWidth() const override { return m_Widht; }
		virtual uint32_t GetHeight() const override { return m_Height; }

		virtual void SetData(void* data, uint32_t size) override;

		virtual void Bind(uint32_t) const override;
		virtual void SetSlot(uint32_t slot) override { m_slot = slot; }

		virtual void Bind() const override { Bind(m_slot); }
		virtual void Unbind() const override;

		virtual bool operator==(const Texture& other) const override { return false; }
		//{
		//	return m_RendererID == ((VulkanTexture2D&)other).m_RendererID;
		//};
		VkSampler& GetSampler() { return m_textureSampler; }
		VkImageView& GetView() { return m_textureImageView; }
	private:
		VkImage m_textureImage;
		VkDeviceMemory m_textureImageMemory;
	public: // temportary
		VkSampler m_textureSampler;
		VkImageView m_textureImageView;

		std::string m_Path;
		uint32_t m_Widht, m_Height;
		//GLenum m_InternalFormat, m_DataFormat;
		uint32_t m_slot;
		std::string m_tag;
	public:
		VulkanContext& _c;
	};

}