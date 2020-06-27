#include "hzpch.h"
#include "Platform/Vulkan/VulkanContext.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "Hazel/Core/Window.h"

namespace Hazel {

	VulkanContext::VulkanContext(Window& window)
		: m_window(window), m_windowHandle((GLFWwindow*)window.GetNativeWindow())
	{
		HZ_CORE_ASSERT(m_windowHandle, "Window handle is null!");

		m_RenderPasses = std::make_shared<RenderPasses>(*this);
		
	}

	VulkanContext::~VulkanContext()
	{
		CleanUpSwapChain();

		vkDestroySemaphore(m_Device, m_RenderFinishedSemaphore, nullptr);
		vkDestroySemaphore(m_Device, m_ImageAvailableSemaphore, nullptr);

		vkDestroyDevice(m_Device, nullptr);

		if (m_VU.ValidationLayersEnabled())
		{
			m_VU.DestroyDebugUtilsMessengerEXT(m_Instance, m_DebugMessenger, nullptr);
		}

		vkDestroySurfaceKHR(m_Instance, m_Surface, nullptr);
		vkDestroyInstance(m_Instance, nullptr);
	}

	void VulkanContext::CleanUpSwapChain()
	{
		vkDestroyCommandPool(m_Device, m_CommandPool, nullptr);

		for (auto framebuffer : m_SwapChainFramebuffers)
		{
			vkDestroyFramebuffer(m_Device, framebuffer, nullptr);
		}

		// m_Pipeline->Cleanup(); will be manage externally

		m_RenderPasses->Cleanup();

		for (auto imageView : m_SwapChainImageViews)
		{
			vkDestroyImageView(m_Device, imageView, nullptr);
		}

		vkDestroySwapchainKHR(m_Device, m_SwapChain, nullptr);
	}

	void VulkanContext::Init()
	{
		CreateInstance();
		SetupDebugMessenger();
		CreateSurface();
		PickPhysicalDevice();
		CreateLogicalDevice();
		
		CreateSyncObjects();
		
		CreateCommandPool();
		CreateCommandBuffers();

		CreateSwapChain();
		CreateImageViews();
		Bind();
	}

	void VulkanContext::Bind()
	{
		BindRenderPass();
		CreateFramebuffers();
	}

	void VulkanContext::SwapBuffers()
	{
		if (!m_Pipeline) return;

		m_CmdBuffer->BindPipeline(m_Pipeline);

		//m_CmdBuffer->Draw(3);


		/*
		1.Acquire an image from the swap chain
		2.Execute the command buffer with that image as attachment in the framebuffer
		3.Return the image to the swap chain for presentation
		*/

		//-----------------------------------------------------1----------------------------
		// 1.Acquiring an image from the swap chain
		uint32_t imageIndex;
		VkResult result = vkAcquireNextImageKHR(m_Device, m_SwapChain, UINT64_MAX, m_ImageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex);

		//-if (result == VK_ERROR_OUT_OF_DATE_KHR)
		//-{
		//-	RecreateSwapChain();
		//-	return;
		//-}
		//-else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
		//-{
		//-	throw std::runtime_error("failed to acquire swap chain image!");
		//-}


		//-----------------------------------------------------2----------------------------
		// 2.Submitting the command buffer
		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

		VkSemaphore waitSemaphores[] = { m_ImageAvailableSemaphore };
		VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = waitSemaphores;
		submitInfo.pWaitDstStageMask = waitStages;

		submitInfo.commandBufferCount = 1;
		m_CmdBuffer->Bind(m_SwapChainFramebuffers[imageIndex]); //          <<<--------Bind Command Buffer to current framebuffer---
		submitInfo.pCommandBuffers = &m_CmdBuffer->Get(); // &m_cmdBufferFly;

		VkSemaphore signalSemaphores[] = { m_RenderFinishedSemaphore };
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = signalSemaphores;

		//vkResetFences(m_device, 1, &m_inFlightFences[m_currentFrame]);
		if (vkQueueSubmit(m_GraphicsQueue, 1, &submitInfo, VK_NULL_HANDLE) != VK_SUCCESS)
		{
			HZ_CORE_ASSERT(false, "Failed to submit draw command buffer!");
		}


		//-----------------------------------------------------3----------------------------
		// 3.Presentation  - Swap!  - Wait for signalSemaphores
		VkPresentInfoKHR presentInfo{};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = signalSemaphores;

		VkSwapchainKHR swapChains[] = { m_SwapChain };
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = swapChains;
		presentInfo.pImageIndices = &imageIndex;
		presentInfo.pResults = nullptr; // Optional

		result = vkQueuePresentKHR(m_PresentQueue, &presentInfo);
		//--if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || m_framebufferResized)
		//--{
		//--	m_framebufferResized = false;
		//--	RecreateSwapChain();
		//--}
		//--else if (result != VK_SUCCESS)
		//--{
		//--	throw std::runtime_error("failed to present swap chain image!");
		//--}

		vkDeviceWaitIdle(m_Device); //		ctx_vkDeviceWaitIdle(); // one frame at a time!!! for the moment
	}

	API VulkanContext::MakeCurrent()
	{
		if (_s_active.get() != this)
		{
			_s_active = Resolve(m_window);
		}
		return GetAPI();
	}

	void VulkanContext::CreateInstance()
	{
		m_window.SetPonterContext(static_cast<void*>(this)); // is already use for event in window class glfwSetWindowUserPointer(m_windowHandle, this)

		if (m_VU.Error_ValidationLayerRequestedButNotPresence())
		{
			HZ_CORE_ASSERT(false, "Validation layers requested, but not available!");
		}
		
		VkApplicationInfo appInfo{};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pApplicationName = "Hello Triangle";
		appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.pEngineName = "No Engine";
		appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.apiVersion = VK_API_VERSION_1_0;
		
		VkInstanceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		createInfo.pApplicationInfo = &appInfo;
		// window info
		auto extensions = m_VU.GetRequiredExtensions();
		HZ_CORE_INFO("Extension REQUESTED:");
		for (const auto& extension : extensions)
		{
			HZ_CORE_INFO("'\t{}", extension);
		}
		createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
		createInfo.ppEnabledExtensionNames = extensions.data();
		
		// layer options
		VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo;
		if (m_VU.ValidationLayersEnabled())
		{
			createInfo.enabledLayerCount = static_cast<uint32_t>(m_VU.GetValidationLayers().size());
			createInfo.ppEnabledLayerNames = m_VU.GetValidationLayers().data();
		
			m_VU.PopulateDebugMessengerCreateInfo(debugCreateInfo);
			createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
		}
		else
		{
			createInfo.enabledLayerCount = 0;
			createInfo.pNext = nullptr;
		}
		
		if (!m_VU.CheckInstanceExtensionSupport(extensions))
		{
			HZ_CORE_ASSERT(false, "Extension not available!");
		}

		if (vkCreateInstance(&createInfo, nullptr, &m_Instance) != VK_SUCCESS)
		{
			HZ_CORE_ASSERT(false, "Failed to create instance!");
		}
	}

	void VulkanContext::SetupDebugMessenger()
	{
		if (!m_VU.ValidationLayersEnabled()) return;

		VkDebugUtilsMessengerCreateInfoEXT createInfo;
		m_VU.PopulateDebugMessengerCreateInfo(createInfo);

		if (m_VU.CreateDebugUtilsMessengerEXT(m_Instance, &createInfo, nullptr, &m_DebugMessenger) != VK_SUCCESS)
		{
			HZ_CORE_ASSERT(false, "Failed to set up debug messenger!");
		}
	}

	void VulkanContext::CreateSurface()
	{
		if (glfwCreateWindowSurface(m_Instance, m_windowHandle, nullptr, &m_Surface) != VK_SUCCESS)
		{
			HZ_CORE_ASSERT(false, "failed to create window surface!");
		}
	}

	void VulkanContext::PickPhysicalDevice()
	{
		uint32_t deviceCount = 0;
		vkEnumeratePhysicalDevices(m_Instance, &deviceCount, nullptr);
		if (deviceCount == 0)
		{
			HZ_CORE_ASSERT(false, "Failed to find GPUs with Vulkan support!");
		}
		std::vector<VkPhysicalDevice> devices(deviceCount);
		vkEnumeratePhysicalDevices(m_Instance, &deviceCount, devices.data());

		for (const auto& device : devices)
		{
			if (m_VU.IsDeviceSuitable(device, m_Surface))
			{
				m_PhysicalDevice = device;
				break;
			}
		}

		if (m_PhysicalDevice == VK_NULL_HANDLE)
		{
			HZ_CORE_ASSERT(false, "Failed to find a suitable GPU!");
		}
	}

	void VulkanContext::CreateLogicalDevice()
	{
		std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;

		QueueFamilyIndices m_qfIndices = m_VU.FindQueueFamilies(m_PhysicalDevice, m_Surface); // I may store the results on member variables(?)
		std::set<uint32_t> uniqueQueueFamilies = { m_qfIndices.graphicsFamily.value(), m_qfIndices.presentFamily.value() };

		float queuePriority = 1.0f;
		for (uint32_t queueFamily : uniqueQueueFamilies)
		{
			VkDeviceQueueCreateInfo queueCreateInfo{};
			queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queueCreateInfo.queueFamilyIndex = queueFamily;
			queueCreateInfo.queueCount = 1;
			queueCreateInfo.pQueuePriorities = &queuePriority;
			queueCreateInfos.push_back(queueCreateInfo);
		}

		VkPhysicalDeviceFeatures deviceFeatures{}; // Like geometry shaders Feature, for now we dont need anything.
		deviceFeatures.samplerAnisotropy = VK_TRUE;

		// Create THE LOGICAL DEVICE!----
		VkDeviceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

		createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
		createInfo.pQueueCreateInfos = queueCreateInfos.data();

		createInfo.pEnabledFeatures = &deviceFeatures;

		createInfo.enabledExtensionCount = static_cast<uint32_t>(m_VU.GetDeviceExtensions().size());
		createInfo.ppEnabledExtensionNames = m_VU.GetDeviceExtensions().data();

		// This may be ignore by up-to-date.
		if (m_VU.ValidationLayersEnabled())
		{
			createInfo.enabledLayerCount = static_cast<uint32_t>(m_VU.GetValidationLayers().size());
			createInfo.ppEnabledLayerNames = m_VU.GetValidationLayers().data();
		}
		else
		{
			createInfo.enabledLayerCount = 0;
		}

		if (vkCreateDevice(m_PhysicalDevice, &createInfo, nullptr, &m_Device) != VK_SUCCESS)
		{
			HZ_CORE_ASSERT(false, "Failed to create logical device!");
		}

		vkGetDeviceQueue(m_Device, m_qfIndices.graphicsFamily.value(), 0, &m_GraphicsQueue);
		vkGetDeviceQueue(m_Device, m_qfIndices.presentFamily.value(), 0, &m_PresentQueue);
	}

	void VulkanContext::CreateSyncObjects()
	{

		VkSemaphoreCreateInfo semaphoreInfo{};
		semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

		//-VkFenceCreateInfo fenceInfo{};
		//-fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		//-fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		if (vkCreateSemaphore(m_Device, &semaphoreInfo, nullptr, &m_ImageAvailableSemaphore) != VK_SUCCESS ||
			vkCreateSemaphore(m_Device, &semaphoreInfo, nullptr, &m_RenderFinishedSemaphore) != VK_SUCCESS)// ||
		{
			HZ_CORE_ASSERT(false, "Failed to create synchronization objects for a frame!");
		}

	}

	void VulkanContext::CreateSwapChain()
	{
		// This change during window resize.
		SwapChainSupportDetails m_swapCDetails = m_VU.QuerySwapChainSupport(m_PhysicalDevice, m_Surface);
		QueueFamilyIndices m_qfIndices = m_VU.FindQueueFamilies(m_PhysicalDevice, m_Surface);

		VkSurfaceFormatKHR surfaceFormat = m_VU.SWCH_chooseSwapSurfaceFormat(m_swapCDetails.formats);
		VkPresentModeKHR presentMode = m_VU.SWCH_chooseSwapPresentMode(m_swapCDetails.presentModes);
		VkExtent2D extent = m_VU.SWCH_chooseSwapExtent(m_swapCDetails.capabilities, m_window.GetWidth(), m_window.GetHeight());

		// where 0 is a special value that means that there is no maximum:
		uint32_t imageCount = m_swapCDetails.capabilities.minImageCount + 1;
		if (m_swapCDetails.capabilities.maxImageCount > 0 && imageCount > m_swapCDetails.capabilities.maxImageCount)
		{
			imageCount = m_swapCDetails.capabilities.maxImageCount;
		}

		VkSwapchainCreateInfoKHR createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		createInfo.surface = m_Surface;

		createInfo.minImageCount = imageCount;
		createInfo.imageFormat = surfaceFormat.format;
		createInfo.imageColorSpace = surfaceFormat.colorSpace;
		createInfo.imageExtent = extent;
		createInfo.imageArrayLayers = 1; // This is always 1 unless you are developing a stereoscopic 3D application
		createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT; // to, or Render"VK_IMAGE_USAGE_TRANSFER_DST_BIT" to <*Framebuffer>

		// Next, we need to specify how to handle swap chain images that will be used across multiple queue families
		uint32_t queueFamilyIndices[] = { m_qfIndices.graphicsFamily.value(), m_qfIndices.presentFamily.value() };

		if (m_qfIndices.graphicsFamily != m_qfIndices.presentFamily) {
			createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			//  Concurrent mode requires you to specify in advance between which queue families ownership will be share
			createInfo.queueFamilyIndexCount = 2;
			createInfo.pQueueFamilyIndices = queueFamilyIndices;
		}
		else {
			createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
			createInfo.queueFamilyIndexCount = 0; // Optional
			createInfo.pQueueFamilyIndices = nullptr; // Optional
		}

		createInfo.preTransform = m_swapCDetails.capabilities.currentTransform;// Example: 90 degree clockwise rotation or horizontal flip.
		createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR; // simply ignore the alpha channel between other images.
		createInfo.presentMode = presentMode;
		createInfo.clipped = VK_TRUE;

		createInfo.oldSwapchain = VK_NULL_HANDLE;

		if (vkCreateSwapchainKHR(m_Device, &createInfo, nullptr, &m_SwapChain) != VK_SUCCESS)
		{
			HZ_CORE_ASSERT(false, "Failed to create swap chain!");
		}

		vkGetSwapchainImagesKHR(m_Device, m_SwapChain, &imageCount, nullptr);
		m_SwapChainImages.resize(imageCount);
		vkGetSwapchainImagesKHR(m_Device, m_SwapChain, &imageCount, m_SwapChainImages.data());

		// store swapchain format
		m_SwapChainImageFormat = surfaceFormat.format;
		m_SwapChainExtent = extent;
	}

	void VulkanContext::CreateImageViews()
	{
		m_SwapChainImageViews.resize(m_SwapChainImages.size());

		for (size_t i = 0; i < m_SwapChainImages.size(); i++) {
			VkImageViewCreateInfo createInfo{};
			createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			createInfo.image = m_SwapChainImages[i];
			createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			createInfo.format = m_SwapChainImageFormat;
			createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			createInfo.subresourceRange.baseMipLevel = 0;
			createInfo.subresourceRange.levelCount = 1;
			createInfo.subresourceRange.baseArrayLayer = 0;
			createInfo.subresourceRange.layerCount = 1;

			if (vkCreateImageView(m_Device, &createInfo, nullptr, &m_SwapChainImageViews[i]) != VK_SUCCESS)
			{
				HZ_CORE_ASSERT(false, "Failed to create image views!");
			}
		}
	}
	// ----------------Bind-------------------------------
	void VulkanContext::BindRenderPass()
	{
		m_RenderPasses->Bind();

	}

	void VulkanContext::CreateFramebuffers()
	{
		m_SwapChainFramebuffers.resize(m_SwapChainImageViews.size());

		for (size_t i = 0; i < m_SwapChainImageViews.size(); i++)
		{
			std::array<VkImageView, 1> attachments =
			{
				m_SwapChainImageViews[i],
				//--m_DepthResources->GetimageView(),
			};

			VkFramebufferCreateInfo framebufferInfo{};
			framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			// We first need to specify with which renderPass the framebuffer needs to be compatible
			framebufferInfo.renderPass = m_RenderPasses->Get(); // <<<<<<<-------------------m_renderPass;
			framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
			framebufferInfo.pAttachments = attachments.data();
			framebufferInfo.width = m_SwapChainExtent.width;
			framebufferInfo.height = m_SwapChainExtent.height;
			framebufferInfo.layers = 1;

			if (vkCreateFramebuffer(m_Device, &framebufferInfo, nullptr, &m_SwapChainFramebuffers[i]) != VK_SUCCESS)
			{
				HZ_CORE_ASSERT(false, "Failed to create Framebuffer!");
			}
		}
	}

	void VulkanContext::CreateCommandPool()
	{
		QueueFamilyIndices m_qfIndices = m_VU.FindQueueFamilies(m_PhysicalDevice, m_Surface); // I may store the results on member variables(?)

		VkCommandPoolCreateInfo poolInfo{};
		poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		poolInfo.queueFamilyIndex = m_qfIndices.graphicsFamily.value();
		poolInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;// 0; // Optional

		if (vkCreateCommandPool(m_Device, &poolInfo, nullptr, &m_CommandPool) != VK_SUCCESS)
		{
			HZ_CORE_ASSERT(false, "Failed to create command pool!");
		}
	}

	void VulkanContext::CreateCommandBuffers()
	{
		m_CmdBuffer = std::make_shared<CommandBuffer>(*this);


		//m_CmdBuffer->Rec();  To move on swapchain
	}

	// Callback function
	bool VulkanContext::DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT msgSvrty, VkDebugUtilsMessageTypeFlagsEXT msgTyp, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData)
	{
		switch (msgSvrty)
		{
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
			HZ_CORE_TRACE("Validation layer: {}", pCallbackData->pMessage);
			break;
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
			HZ_CORE_INFO("Validation layer: {}", pCallbackData->pMessage);
			break;
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
			HZ_CORE_WARN("Validation layer: {}", pCallbackData->pMessage);
			break;
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
			HZ_CORE_ERROR("Validation layer: {}", pCallbackData->pMessage);
			break;
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_FLAG_BITS_MAX_ENUM_EXT:
			HZ_CORE_CRITICAL("Validation layer: {}", pCallbackData->pMessage);
			break;
		default:
			break;
		}
		return VK_FALSE;
	}


	void VulkanContext::BindVertexBuffer(const VkBuffer& vb)
	{
		m_CmdBuffer->BindVertexBuffer(vb);
	}

	void VulkanContext::BindIndexBuffer(const VkBuffer& ib)
	{
		m_CmdBuffer->BindIndexBuffer(ib);
	}

	void VulkanContext::BindPipeline(Ref<Pipeline>& pipeline)
	{
		m_Pipeline = pipeline;
	}
	void VulkanContext::UnbindPipeline()
	{
		m_Pipeline = nullptr;
	}


	//--------------------------
//	void VulkanContextvkCmdClearAttachments



	void VulkanContext::CmdClear_impl()
	{
		m_CmdBuffer->GetQueue().push_back([=](const VkCommandBuffer& drawCommandBuffer, const VkFramebuffer& framebuffer)
		{
			VkClearAttachment clearAttachments = {};
			clearAttachments.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			clearAttachments.clearValue.color = { 164.0f / 256.0f, 30.0f / 256.0f, 34.0f / 256.0f, 0.0f };
			clearAttachments.colorAttachment = 0;
		
			VkClearRect clearRect = {};
			clearRect.layerCount = 1;
			clearRect.rect.offset = { 0, 0 };
			clearRect.rect.extent = { m_SwapChainExtent.width, m_SwapChainExtent.height };
		
			vkCmdClearAttachments(drawCommandBuffer, 1, &clearAttachments, 1, &clearRect);
		});
	}

	void VulkanContext::CmdDrawArrays_impl(uint32_t vertexCount, uint32_t offset)
	{
		m_CmdBuffer->GetQueue().push_back([=](const VkCommandBuffer& drawCommandBuffer, const VkFramebuffer& framebuffer)
		{
			vkCmdDraw(drawCommandBuffer, vertexCount, 1, offset, 0);
		});
	}

	void VulkanContext::CmdDrawArraysInstanced_impl(uint32_t vertexCount, uint32_t indexCount)
	{
		m_CmdBuffer->GetQueue().push_back([=](const VkCommandBuffer& drawCommandBuffer, const VkFramebuffer& framebuffer)
		{
			vkCmdDraw(drawCommandBuffer, vertexCount, indexCount, 0, 0);
		});
	}

	void VulkanContext::CmdDrawIndexted_impl(uint32_t indexCount, uint32_t offset)
	{
		m_CmdBuffer->GetQueue().push_back([=](const VkCommandBuffer& drawCommandBuffer, const VkFramebuffer& framebuffer)
		{
			vkCmdDrawIndexed(drawCommandBuffer, indexCount, 1, offset, 0, 0);
		});
	}

	void VulkanContext::CmdDrawIndextedInstanced_impl(uint32_t indexCount, uint32_t instanceCount)
	{
		m_CmdBuffer->GetQueue().push_back([=](const VkCommandBuffer& drawCommandBuffer, const VkFramebuffer& framebuffer)
		{
			vkCmdDrawIndexed(drawCommandBuffer, indexCount, instanceCount, 0, 0, 0);
		});
	}


}
