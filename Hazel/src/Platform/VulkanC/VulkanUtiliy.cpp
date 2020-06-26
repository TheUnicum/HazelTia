#include "hzpch.h"
#include "VulkanUtiliy.h"

#include "VulkanContext.h"
#include "Platform/Windows/WindowsWindow.h"

#include <GLFW/glfw3.h>


#ifdef NDEBUG
	const bool g_enableValidationLayers = false;
#else
	const bool g_enableValidationLayers = true;
#endif

const std::vector<const char*> g_validationLayers = { "VK_LAYER_KHRONOS_validation" };
const std::vector<const char*> g_deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

namespace Hazel {

	VulkanUtiliy::VulkanUtiliy() {}

	bool VulkanUtiliy::ValidationLayersEnabled() const { return g_enableValidationLayers; }
	const std::vector<const char*>& VulkanUtiliy::GetValidationLayers() const { return g_validationLayers; }
	const std::vector<const char*>& VulkanUtiliy::GetDeviceExtensions() const { return g_deviceExtensions; }

	bool VulkanUtiliy::Error_ValidationLayerRequestedButNotPresence()
	{
		return (g_enableValidationLayers && !CheckValidationLayerSupport(g_validationLayers));
	}

	bool VulkanUtiliy::CheckValidationLayerSupport(std::vector<const char*> validationLayersRequested)
	{
		uint32_t layerCount;
		vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
		std::vector<VkLayerProperties> availableLayers(layerCount);
		vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

		HZ_CORE_INFO("Available Layer:");
		for (const auto& layer : availableLayers)
		{
			HZ_CORE_INFO("'\t{}", layer.layerName);
		}

		for (const char* layerName : validationLayersRequested)
		{
			bool layerFound = false;

			for (const auto& layerProperties : availableLayers)
			{
				if (strcmp(layerName, layerProperties.layerName) == 0) {
					layerFound = true;
					break;
				}
			}

			if (!layerFound) {
				return false;
			}
		}
		return true;
	}

	bool VulkanUtiliy::CheckInstanceExtensionSupport(std::vector<const char*> extensionRequested)
	{
		uint32_t extensionCount = 0;
		vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
		std::vector<VkExtensionProperties> extensions(extensionCount);
		vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

		HZ_CORE_INFO("Available extensions:");
		for (const auto& extension : extensions)
		{
			HZ_CORE_INFO("'\t{}", extension.extensionName);
		}

		for (const char* layerName : extensionRequested)
		{
			bool layerFound = false;

			for (const auto& layerProperties : extensions)
			{
				if (strcmp(layerName, layerProperties.extensionName) == 0)
				{
					layerFound = true;
					break;
				}
			}

			if (!layerFound) {
				return false;
			}
		}
		return true;
	}

	bool VulkanUtiliy::CheckDeviceExtensionSupport(VkPhysicalDevice device, std::vector<const char*> deviceExtensionRequested)
	{
		uint32_t extensionCount;
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);
		std::vector<VkExtensionProperties> availableExtensions(extensionCount);
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

		std::set<std::string> requiredExtensions(deviceExtensionRequested.begin(), deviceExtensionRequested.end());

		for (const auto& extension : availableExtensions)
		{
			requiredExtensions.erase(extension.extensionName);
		}

		return requiredExtensions.empty();
	}

	std::vector<const char*> VulkanUtiliy::GetRequiredExtensions()
	{
		uint32_t glfwExtensionCount = 0;
		const char** glfwExtensions;
		glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

		std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

		if (g_enableValidationLayers)
		{
			extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
		}

		return extensions;
	}

	bool VulkanUtiliy::IsDeviceSuitable(VkPhysicalDevice device, VkSurfaceKHR surface)
	{
		/*
		VkPhysicalDeviceProperties deviceProperties;
		VkPhysicalDeviceFeatures deviceFeatures;
		vkGetPhysicalDeviceProperties(device, &deviceProperties);
		vkGetPhysicalDeviceFeatures(device, &deviceFeatures);
		*/

		QueueFamilyIndices m_qfIndices = FindQueueFamilies(device, surface);

		bool extensionsSupported = CheckDeviceExtensionSupport(device, g_deviceExtensions);

		bool swapChainAdequate = false;
		if (extensionsSupported)
		{
			SwapChainSupportDetails m_swapCDetails = QuerySwapChainSupport(device, surface);
			// Swap chain support is sufficient if there is at least one supported image <format> & one supported <presentation mode>.
			swapChainAdequate = !m_swapCDetails.formats.empty() && !m_swapCDetails.presentModes.empty();
		}

		VkPhysicalDeviceFeatures supportedFeatures;
		vkGetPhysicalDeviceFeatures(device, &supportedFeatures);

		return m_qfIndices.isComplete() && extensionsSupported && swapChainAdequate && supportedFeatures.samplerAnisotropy;
	}

	QueueFamilyIndices VulkanUtiliy::FindQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface)
	{
		QueueFamilyIndices indices;

		uint32_t queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
		std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

		int i = 0;
		for (const auto& queueFamily : queueFamilies)
		{	// We need to find at least one queue family that supports VK_QUEUE_GRAPHICS_BIT.
			if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
			{
				indices.graphicsFamily = i;
			}
			//  look for a queue family that has the capability of presenting to our window surface
			VkBool32 presentSupport = false;
			vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);

			if (presentSupport)
			{
				indices.presentFamily = i;
			}

			if (indices.isComplete())
			{
				break;
			}
			i++;
		}

		return indices;
	}

	SwapChainSupportDetails VulkanUtiliy::QuerySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface)
	{
		SwapChainSupportDetails details;

		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);

		uint32_t formatCount;
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);
		if (formatCount != 0)
		{
			details.formats.resize(formatCount);
			vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());
		}

		uint32_t presentModeCount;
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);
		if (presentModeCount != 0)
		{
			details.presentModes.resize(presentModeCount);
			vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentModes.data());
		}

		return details; return SwapChainSupportDetails();
	}

	// SwapChain Format/Mode/Capabilities
	VkSurfaceFormatKHR VulkanUtiliy::SWCH_chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
	{
		for (const auto& availableFormat : availableFormats)
		{
			if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
			{
				return availableFormat;
			}
		}
		return availableFormats[0];
	}

	VkPresentModeKHR VulkanUtiliy::SWCH_chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes)
	{
		for (const auto& availablePresentMode : availablePresentModes)
		{
			if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
			{
				return availablePresentMode;
			}
		}

		return VK_PRESENT_MODE_FIFO_KHR; //  VK_PRESENT_MODE_FIFO_KHR mode is guaranteed to be available.
	}

	VkExtent2D VulkanUtiliy::SWCH_chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, uint32_t win_width, uint32_t win_height)
	{
		if (capabilities.currentExtent.width != UINT32_MAX)
		{
			return capabilities.currentExtent;
		}
		else
		{
			VkExtent2D actualExtent = { static_cast<uint32_t>(win_width), static_cast<uint32_t>(win_height) };

			actualExtent.width = std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, actualExtent.width));
			actualExtent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, actualExtent.height));
			return actualExtent;
		}
	}

	VKAPI_ATTR VkBool32 VKAPI_CALL VulkanUtiliy::debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT msgSvrty, VkDebugUtilsMessageTypeFlagsEXT msgTyp, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData)
	{
		WindowsWindow::WindowData& data = *(WindowsWindow::WindowData*)pUserData;
		VulkanContext& ctx = *(VulkanContext*)(data.p_ctx);
		HZ_CORE_ASSERT(&ctx, "Pointer to context not initialized!, Not possoble call instantiated callback");
		return ctx.DebugCallback(msgSvrty, msgTyp, pCallbackData, pUserData);
	}

	void VulkanUtiliy::PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo)
 {
		createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | //VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT 
									 VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
									 VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
								 VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
								 VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		createInfo.pfnUserCallback = VulkanUtiliy::debugCallback; // nullptr; // debugCallback;
		createInfo.pUserData = this; // Optional
	}

	VkResult VulkanUtiliy::CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger)
	{
		auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
		if (func != nullptr)
		{
			return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
		}
		else
		{
			return VK_ERROR_EXTENSION_NOT_PRESENT;
		}
	}

	void VulkanUtiliy::DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator)
	{
		auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
		if (func != nullptr)
		{
			func(instance, debugMessenger, pAllocator);
		}
	}

}
