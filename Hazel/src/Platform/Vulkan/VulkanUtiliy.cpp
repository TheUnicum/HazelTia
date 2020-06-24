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

		std::cout << "available Layer:\n";
		for (const auto& extension : availableLayers)
			std::cout << '\t' << extension.layerName << '\n';

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

		std::cout << "Available extensions:\n";
		for (const auto& extension : extensions)
			std::cout << '\t' << extension.extensionName << '\n';

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

	VKAPI_ATTR VkBool32 VKAPI_CALL VulkanUtiliy::debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT msgSvrty, VkDebugUtilsMessageTypeFlagsEXT msgTyp, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData)
	{
		WindowsWindow::WindowData& data = *(WindowsWindow::WindowData*)pUserData;
		VulkanContext& ctx = *(VulkanContext*)(data.p_ctx);
		return ctx.DebugCallback(msgSvrty, msgTyp, pCallbackData, pUserData);
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
