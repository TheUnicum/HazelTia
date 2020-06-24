#include "hzpch.h"
#include "Platform/Vulkan/VulkanContext.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>


namespace Hazel {

	VulkanContext::VulkanContext(Window& window)
		: m_window(window), m_windowHandle((GLFWwindow*)window.GetNativeWindow())
	{
		HZ_CORE_ASSERT(m_windowHandle, "Window handle is null!");

		m_RenderPasses = std::make_shared<RenderPasses>(*this);
		m_Pipeline = std::make_shared<Pipeline>(*this);
	}

	VulkanContext::~VulkanContext()
	{
		CleanUpSwapChain();

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
		CreateSwapChain();
		CreateImageViews();
		
		BindRenderPass();
		BindPipeline();
	}

	void VulkanContext::SwapBuffers()
	{
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

	void VulkanContext::BindRenderPass()
	{
		m_RenderPasses->Bind();

	}

	void VulkanContext::BindPipeline()
	{
		PipelineSpecification pipSpec;
		m_Pipeline->SetSpec(pipSpec);
		m_Pipeline->Bind();

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

}
