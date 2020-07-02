#include "hzpch.h"
#include "Pipeline.h"

#include "Platform/Vulkan/VulkanContext.h"
#include "Platform/Vulkan/VulkanShader.h"
#include "Platform/Vulkan/VulkanVertexLayout.h"
#include "Platform/Vulkan/VulkanConstantBuffer.h"
#include "Platform/Vulkan/VulkanTexture.h"


struct Vertex
{
	glm::vec3 pos;
	glm::vec3 color;
};

VkVertexInputBindingDescription GetBindingDescription()
{
	VkVertexInputBindingDescription bindingDescription{};
	bindingDescription.binding = 0;
	bindingDescription.stride = sizeof(Vertex);
	bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

	return bindingDescription;
}

std::vector<VkVertexInputAttributeDescription> GetAttributeDescriptions()
{
	std::vector<VkVertexInputAttributeDescription> attributeDescriptions;
	attributeDescriptions.resize(3);

	attributeDescriptions[0].binding = 0;
	attributeDescriptions[0].location = 0;
	attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
	attributeDescriptions[0].offset = offsetof(Vertex, pos);

	attributeDescriptions[1].binding = 0;
	attributeDescriptions[1].location = 1;
	attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
	attributeDescriptions[1].offset = offsetof(Vertex, color);


	return attributeDescriptions;
}








namespace Hazel {

	Pipeline::Pipeline(VulkanContext& ctx)
		: m_ctx(ctx)
	{
	}

	Pipeline::~Pipeline()
	{
		//Cleanup(); 
		auto c = m_ctx.GetDevice();
	}

	void Pipeline::Cleanup()
	{
		//std::dynamic_pointer_cast<VulkanShader>(m_spec.shader)->Cleanup();

		if (m_Pipeline)
		{ 
			vkDestroyPipeline(m_ctx.GetDevice(), m_Pipeline, nullptr);
			m_Pipeline = nullptr;
		}
		if (m_PipelineLayout)
		{
			vkDestroyPipelineLayout(m_ctx.GetDevice(), m_PipelineLayout, nullptr);
			m_PipelineLayout = nullptr;
		}
	}

	void Pipeline::Bind()
	{
		m_ctx.BindPipeline(CreateRef<Pipeline>(*this));
	}

	void Pipeline::Create(const PipelineCreateInfo& spec, const VkDescriptorSetLayout& composed_descSetLayout)
	{
		Cleanup();

		VulkanVertexLayout::DescriptorData& c = *(VulkanVertexLayout::DescriptorData*)(spec.vertexLayout->GetDescriptorData());
		c.InputBindingDescription;
		c.VertexInputAttributeDescription_list;

		VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
		vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
		vertShaderStageInfo.module = (std::dynamic_pointer_cast<VulkanShader>(spec.shader))->GetVertexShaderModule();
		vertShaderStageInfo.pName = "main";

		VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
		fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		fragShaderStageInfo.module = (std::dynamic_pointer_cast<VulkanShader>(spec.shader))->GetFragmentShaderModule();
		fragShaderStageInfo.pName = "main";

		VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };

		// Fixed functions **************************************
		/*
		Vertex input
		Input assembly
		Viewportsand scissors
		Rasterizer
		Multisampling
		Depthand stencil testing
		Color blending
		Dynamic state
		Pipeline layout
		*/

		//auto bindingDescription = m_spec.bufferLayout->GetBindingDescription();
		//auto attributeDescriptions = spec.bufferLayout->GetAttributeDescriptions();
		auto bindingDescription = c.InputBindingDescription; //GetBindingDescription();
		auto attributeDescriptions = c.VertexInputAttributeDescription_list;// GetAttributeDescriptions();

		// Vertex input
		VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
		vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertexInputInfo.vertexBindingDescriptionCount = 1;
		vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
		vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
		vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

		// Input assembly
		VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
		inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		inputAssembly.primitiveRestartEnable = VK_FALSE;

		// Viewports and scissors
		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = (float)m_ctx.GeSwapchainExtent().width; //(float)m_swapChainExtent.width;
		viewport.height = (float)m_ctx.GeSwapchainExtent().height; //(float)m_swapChainExtent.height;
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;

		VkRect2D scissor{};
		scissor.offset = { 0, 0 };
		scissor.extent = m_ctx.GeSwapchainExtent(); // m_swapChainExtent;

		VkPipelineViewportStateCreateInfo viewportState{};
		viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewportState.viewportCount = 1;
		viewportState.pViewports = &viewport;
		viewportState.scissorCount = 1;
		viewportState.pScissors = &scissor;

		// Rasterizer
		VkPipelineRasterizationStateCreateInfo rasterizer{};
		rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		rasterizer.depthClampEnable = VK_FALSE; // This is useful in some special cases like shadow maps
		rasterizer.rasterizerDiscardEnable = VK_FALSE; // set to VK_TRUE then geometry never passes through.
		rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
		rasterizer.lineWidth = 1.0f;
		rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
		rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE; // VK_FRONT_FACE_CLOCKWISE;
		rasterizer.depthBiasEnable = VK_FALSE;
		rasterizer.depthBiasConstantFactor = 0.0f; // Optional
		rasterizer.depthBiasClamp = 0.0f; // Optional
		rasterizer.depthBiasSlopeFactor = 0.0f; // Optional

		// Multisampling
		VkPipelineMultisampleStateCreateInfo multisampling{};
		multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		multisampling.sampleShadingEnable = VK_FALSE;
		multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
		multisampling.minSampleShading = 1.0f; // Optional
		multisampling.pSampleMask = nullptr; // Optional
		multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
		multisampling.alphaToOneEnable = VK_FALSE; // Optional

		// Depth and stencil testing
		//--VkPipelineDepthStencilStateCreateInfo depthStencil{};
		//--depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		//--depthStencil.depthTestEnable = VK_TRUE;
		//--depthStencil.depthWriteEnable = VK_TRUE;
		//--depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
		//--depthStencil.depthBoundsTestEnable = VK_FALSE;
		//--depthStencil.minDepthBounds = 0.0f; // Optional
		//--depthStencil.maxDepthBounds = 1.0f; // Optional
		//--depthStencil.stencilTestEnable = VK_FALSE;
		//--// depthStencil.front{}; // Optional
		//--// depthStencil.back{}; // Optional

		// Color blending
		// 1st contains the configuration per attached framebuffer
		VkPipelineColorBlendAttachmentState colorBlendAttachment{};
		colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		colorBlendAttachment.blendEnable = VK_FALSE;
		colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
		colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
		colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD; // Optional
		colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
		colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
		colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD; // Optional

		// 2nd contains the global color blending settings
		VkPipelineColorBlendStateCreateInfo colorBlending{};
		colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		colorBlending.logicOpEnable = VK_FALSE;
		colorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional
		colorBlending.attachmentCount = 1;
		colorBlending.pAttachments = &colorBlendAttachment;
		colorBlending.blendConstants[0] = 0.0f; // Optional
		colorBlending.blendConstants[1] = 0.0f; // Optional
		colorBlending.blendConstants[2] = 0.0f; // Optional
		colorBlending.blendConstants[3] = 0.0f; // Optional

		// Pipeline layout
		// Set Binding Point for BUFFER!
		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		if (spec.constantBuffer)
		{
			pipelineLayoutInfo.setLayoutCount = 1; // Optional
			//VkDescriptorSetLayout& descLayout = std::dynamic_pointer_cast<VulkanConstantBuffer>(spec.constantBuffer)->GetDescriptorSetLayout();
			const VkDescriptorSetLayout& descLayout = composed_descSetLayout;
			pipelineLayoutInfo.pSetLayouts = &descLayout; 
		}
		else
		{
			pipelineLayoutInfo.setLayoutCount = 0; // Optional
			pipelineLayoutInfo.pSetLayouts = nullptr; // Optional
		}
		pipelineLayoutInfo.pushConstantRangeCount = 0; // Optional
		pipelineLayoutInfo.pPushConstantRanges = nullptr; // Optional

		//if (vkCreatePipelineLayout(m_device, &pipelineLayoutInfo, nullptr, &m_pipelineLayout) != VK_SUCCESS)
		if (vkCreatePipelineLayout(m_ctx.GetDevice(), &pipelineLayoutInfo, nullptr, &m_PipelineLayout) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create pipeline layout!");
		}
		// Fixed functions **************************************


		// ------------------------------------------
		// --------GraphicsPipelineCreate------------
		// ------------------------------------------
		VkGraphicsPipelineCreateInfo pipelineInfo{};
		pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipelineInfo.stageCount = 2;
		pipelineInfo.pStages = shaderStages;
		// Then we reference all of the structures describing the fixed-function stage.
		pipelineInfo.pVertexInputState = &vertexInputInfo;
		pipelineInfo.pInputAssemblyState = &inputAssembly;
		pipelineInfo.pViewportState = &viewportState;
		pipelineInfo.pRasterizationState = &rasterizer;
		pipelineInfo.pMultisampleState = &multisampling;
		pipelineInfo.pDepthStencilState = nullptr;// &depthStencil; // nullptr; // Optional
		pipelineInfo.pColorBlendState = &colorBlending;
		pipelineInfo.pDynamicState = nullptr; // Optional
		// Pipeline layout
		pipelineInfo.layout = m_PipelineLayout;
		// Render passes
		pipelineInfo.renderPass = m_ctx.GetRenderPass(); // m_renderPass;
		pipelineInfo.subpass = 0; // index of the sub pass where this graphics pipeline will be used
		pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
		pipelineInfo.basePipelineIndex = -1; // Optional

		if (vkCreateGraphicsPipelines(m_ctx.GetDevice(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_Pipeline) != VK_SUCCESS)
		{
			HZ_CORE_ASSERT(false, "Failed to create graphics pipeline!");
		}

	}

	void Pipeline::CreateDescriptorPoolandSets(const PipelineCreateInfo& spec, const VkDescriptorSetLayout& composed_descSetLayout)
	{
		VkDevice& device = m_ctx.GetDevice();
		VulkanConstantBuffer& cub = *std::dynamic_pointer_cast<VulkanConstantBuffer>(spec.constantBuffer);

		if (m_descriptorPool)
			vkDestroyDescriptorPool(device, m_descriptorPool, nullptr);


		// ------CreateDescriptorPool
		std::vector<VkDescriptorPoolSize> poolSizes{};
		VkDescriptorPoolSize pool;
		if (spec.constantBuffer)
		{
			pool.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			pool.descriptorCount = 1;
			poolSizes.push_back(pool);
		}
		if (spec.texture)
		{
			pool.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			pool.descriptorCount = 1;
			poolSizes.push_back(pool);
		}
		VkDescriptorPoolCreateInfo poolInfo{};
		poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
		poolInfo.pPoolSizes = poolSizes.data();
		poolInfo.maxSets = 1;// static_cast<uint32_t>(m_swapChainImages.size());


		if (vkCreateDescriptorPool(device, &poolInfo, nullptr, &m_descriptorPool) != VK_SUCCESS)
		{
			HZ_CORE_ASSERT(false, "failed to create descriptor pool!")
		}




		// ------CreateDescriptorSets
		VkDescriptorSetAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = m_descriptorPool;
		allocInfo.descriptorSetCount = 1;
		allocInfo.pSetLayouts = &composed_descSetLayout;

		//m_descriptorSets.resize(m_swapChainImages.size());
		if (vkAllocateDescriptorSets(device, &allocInfo, &m_descriptorSets) != VK_SUCCESS)
		{
			HZ_CORE_ASSERT(false, "failed to allocate descriptor sets!")
		}


		std::array<VkWriteDescriptorSet, 2> descriptorWrites{};


		VkDescriptorBufferInfo bufferInfo{};
		bufferInfo.buffer = cub.m_uniformBuffer; // m_uniformBuffer;
		bufferInfo.offset = 0;
		bufferInfo.range = cub.m_size; // m_size; // sizeof(UniformBufferObject);

		auto& texture = std::dynamic_pointer_cast<VulkanTexture2D>(spec.texture);
		VkDescriptorImageInfo imageInfo{};
		imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		imageInfo.imageView = texture->GetView(); //m_Texture->GetView(); //m_textureImageView;
		imageInfo.sampler = texture->GetSampler(); //m_Sampler->Get(); //textureSampler;
		

		descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrites[0].dstSet = m_descriptorSets;
		descriptorWrites[0].dstBinding = 0;
		descriptorWrites[0].dstArrayElement = 0;
		descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descriptorWrites[0].descriptorCount = 1;
		descriptorWrites[0].pBufferInfo = &bufferInfo;
		descriptorWrites[0].pImageInfo = nullptr;

		descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrites[1].dstSet = m_descriptorSets;
		descriptorWrites[1].dstBinding = 1;
		descriptorWrites[1].dstArrayElement = 0;
		descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		descriptorWrites[1].descriptorCount = 1;
		descriptorWrites[1].pBufferInfo = nullptr;
		descriptorWrites[1].pImageInfo = &imageInfo;

		//vkUpdateDescriptorSets(device, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
		vkUpdateDescriptorSets(device, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);



		//--std::vector<VkWriteDescriptorSet> descriptorWrites{};
		//--if (spec.constantBuffer)
		//--{
		//--	VkDescriptorBufferInfo bufferInfo{};
		//--	bufferInfo.buffer = cub.m_uniformBuffer; // m_uniformBuffer;
		//--	bufferInfo.offset = 0;
		//--	bufferInfo.range = cub.m_size; // m_size; // sizeof(UniformBufferObject);
		//--
		//--	descriptorWrites.push_back({
		//--		VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,		// sType
		//--		nullptr,									// pNext
		//--		m_descriptorSets,							// dstSet
		//--		cub.m_slot,									// dstBinding
		//--		0,											// dstArrayElement
		//--		1,											// descriptorCount
		//--		VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,			//descriptorType
		//--		nullptr,									// pImageInfo
		//--		&bufferInfo,								// pBufferInfo
		//--		nullptr										// pTexelBufferView
		//--		});
		//--}
		//--if (spec.texture)
		//--{
		//--	VkDescriptorImageInfo imageInfo{};
		//--	imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		//--	imageInfo.imageView = std::dynamic_pointer_cast<VulkanTexture2D>(spec.texture)->m_textureImageView; //m_Texture->GetView(); //m_textureImageView;
		//--	imageInfo.sampler = std::dynamic_pointer_cast<VulkanTexture2D>(spec.texture)->m_textureSampler; //m_Sampler->Get(); //textureSampler;
		//--
		//--	descriptorWrites.push_back({
		//--		VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,		// sType
		//--		nullptr,									// pNext
		//--		m_descriptorSets,							// dstSet
		//--		1,											// dstBinding
		//--		0,											// dstArrayElement
		//--		1,											// descriptorCount
		//--		VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,	//descriptorType
		//--		&imageInfo,									// pImageInfo
		//--		nullptr,									// pBufferInfo
		//--		nullptr										// pTexelBufferView
		//--		});
		//--}
		//--
		//--vkUpdateDescriptorSets(device, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
		//vkUpdateDescriptorSets(device, 1, &desWrite, 0, nullptr);
		
		//VkDescriptorBufferInfo bufferInfo{};
		//bufferInfo.buffer = cub.m_uniformBuffer; // m_uniformBuffer;
		//bufferInfo.offset = 0;
		//bufferInfo.range = cub.m_size; // m_size; // sizeof(UniformBufferObject);
		//
		//VkWriteDescriptorSet descriptorWrite{};
		//descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		//descriptorWrite.dstSet = m_descriptorSets;
		//descriptorWrite.dstBinding = cub.m_slot; //m_slot;  //<----------------
		//descriptorWrite.dstArrayElement = 0;
		//descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		//descriptorWrite.descriptorCount = 1;
		//descriptorWrite.pBufferInfo = &bufferInfo;
		//descriptorWrite.pImageInfo = nullptr; // Optional
		//descriptorWrite.pTexelBufferView = nullptr; // Optional
		//
		//vkUpdateDescriptorSets(device, 1, &descriptorWrite, 0, nullptr);
	}

}
