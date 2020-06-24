#pragma once

class VulkanContext;
#include "vulkan/vulkan.h"

#include <vector>

namespace Hazel {

	class VulkanUtiliy
	{
	public:
		VulkanUtiliy();
		~VulkanUtiliy() = default;

		bool ValidationLayersEnabled() const;
		const std::vector<const char*>& GetValidationLayers() const;

		bool Error_ValidationLayerRequestedButNotPresence();
		bool CheckValidationLayerSupport(std::vector<const char*> validationLayersRequested);
		bool CheckInstanceExtensionSupport(std::vector<const char*> extensionRequested);
		std::vector<const char*> GetRequiredExtensions();
		void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);

		// Callback
		static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT msgSvrty, VkDebugUtilsMessageTypeFlagsEXT msgTyp, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData);
		VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);
		void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);

	};
}
