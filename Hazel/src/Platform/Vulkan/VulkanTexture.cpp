#include "hzpch.h"
#include "VulkanTexture.h"

#include <stb_image.h>
#include "VulkanBuffer.h"

namespace Hazel {

	VulkanTexture2D::VulkanTexture2D(uint32_t width, uint32_t height)
		: Texture2D(GraphicsContext::Get_Active()), _c((VulkanContext&)this->_ctx),
		m_Widht(width), m_Height(height)
	{

	}

	VulkanTexture2D::VulkanTexture2D(const std::string& path)
		: Texture2D(GraphicsContext::Get_Active()), _c((VulkanContext&)this->_ctx),
		m_Path(path)
	{
		HZ_PROFILE_FUNCTION();

		VkDevice& device = _c.GetDevice();

		int width, height, channels;
		stbi_set_flip_vertically_on_load(1);
		stbi_uc* data = nullptr;
		{
			HZ_PROFILE_SCOPE("stbi_load - OpenGLTexture2D::OpenGLTexture2D(const std:string&)");
			data = stbi_load(path.c_str(), &width, &height, &channels, STBI_rgb_alpha); // FORCE TI LOAD AS 4 channel 0);  <----------------------
		}
		HZ_CORE_ASSERT(data, "Failed to load image!");
		m_Widht = width;
		m_Height = height;

		channels = 4;//<--------------------
		size_t internalFormat = 0, dataFormat = 0;
		if (channels == 4)
		{
			internalFormat = 4;
			dataFormat = 4;
		}
		else if (channels == 3)
		{
			internalFormat = 3;
			dataFormat = 3;
		}

		VkDeviceSize imageSize = m_Widht * m_Height * 4;
		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;
		createBuffer(_c, imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

		void* pm_data;
		vkMapMemory(device, stagingBufferMemory, 0, imageSize, 0, &pm_data);
		memcpy(pm_data, data, static_cast<size_t>(imageSize));
		vkUnmapMemory(device, stagingBufferMemory);

		stbi_image_free(data);

		CreateImage(_c, m_Widht, m_Height, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_textureImage, m_textureImageMemory);

		transitionImageLayout(_c, m_textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
		copyBufferToImage(_c, stagingBuffer, m_textureImage, static_cast<uint32_t>(m_Widht), static_cast<uint32_t>(m_Height));
		transitionImageLayout(_c, m_textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

		vkDestroyBuffer(device, stagingBuffer, nullptr);
		vkFreeMemory(device, stagingBufferMemory, nullptr);

		createBaseSampler(_c, m_textureSampler);
		createImageViewFromImage(_c, m_textureImage, VK_FORMAT_R8G8B8A8_SRGB, m_textureImageView);
	}

	VulkanTexture2D::~VulkanTexture2D()
	{
		VkDevice& device = _c.GetDevice();

		vkDestroySampler(device, m_textureSampler, nullptr);

		vkDestroyImage(device, m_textureImage, nullptr);
		vkFreeMemory(device, m_textureImageMemory, nullptr);
	}

	void VulkanTexture2D::SetData(void* data, uint32_t size)
	{
	}

	void VulkanTexture2D::Bind(uint32_t) const
	{
	}

	void VulkanTexture2D::Unbind() const
	{
	}

	void VulkanTexture2D::SetDescriptorSetLayout()
	{
		m_LayoutBinding.binding = m_slot;
		m_LayoutBinding.descriptorCount = 1;
		m_LayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		m_LayoutBinding.pImmutableSamplers = nullptr;
		m_LayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
	}

}
