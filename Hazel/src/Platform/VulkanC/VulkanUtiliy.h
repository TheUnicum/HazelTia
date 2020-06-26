#pragma once

class VulkanContext;

#include "vulkan/vulkan.h"

#include <vector>
#include <optional>
#include <set>

namespace Hazel {

	struct QueueFamilyIndices
	{
		std::optional<uint32_t> graphicsFamily;
		std::optional<uint32_t> presentFamily;

		bool isComplete() { return graphicsFamily.has_value() && presentFamily.has_value(); }
	};
	struct SwapChainSupportDetails
	{
		VkSurfaceCapabilitiesKHR capabilities; // min/max number of images in swap chain, min/max width and height of images
		std::vector<VkSurfaceFormatKHR> formats; // pixel format, color space
		std::vector<VkPresentModeKHR> presentModes; // available presentation modes
	};

	class VulkanUtiliy
	{
	public:
		VulkanUtiliy();
		~VulkanUtiliy() = default;

		bool ValidationLayersEnabled() const;
		const std::vector<const char*>& GetValidationLayers() const;
		const std::vector<const char*>& GetDeviceExtensions() const;

		bool Error_ValidationLayerRequestedButNotPresence();
		static bool CheckValidationLayerSupport(std::vector<const char*> validationLayersRequested);
		static bool CheckInstanceExtensionSupport(std::vector<const char*> extensionRequested);
		static bool CheckDeviceExtensionSupport(VkPhysicalDevice device, std::vector<const char*> deviceExtensionRequested);

		std::vector<const char*> GetRequiredExtensions();

		bool IsDeviceSuitable(VkPhysicalDevice device, VkSurfaceKHR surface);
		QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface);
		SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface);

		// SwapChain Format/Mode/Capabilities
		VkSurfaceFormatKHR SWCH_chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
		VkPresentModeKHR SWCH_chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
		VkExtent2D SWCH_chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, uint32_t win_width, uint32_t win_height);

		// Callback
		static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT msgSvrty, VkDebugUtilsMessageTypeFlagsEXT msgTyp, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData);
		void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
		VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);
		void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);
	};
}
