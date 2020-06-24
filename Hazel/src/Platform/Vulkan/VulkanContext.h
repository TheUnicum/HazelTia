#pragma once

#include "Hazel/Renderer/GraphicsContext.h"

#include "Platform/Vulkan/VulkanUtiliy.h"

#include "vulkan/vulkan.h"

#include "Platform/Vulkan/RenderPasses.h"

struct GLFWwindow;

namespace Hazel {

	class VulkanContext : public GraphicsContext
	{
	public:
		friend class VulkanUtiliy;
	public:
		VulkanContext(Window& window);
		~VulkanContext();

		void CleanUpSwapChain();

		virtual void Init() override;

		virtual void SwapBuffers() override;

		virtual API GetAPI() const override { return API::Vulkan; };
		virtual std::string GetAPI_TEXT() const override { return "Vulkan"; };
		virtual API MakeCurrent() override;
	private:
		Window& m_window;
		GLFWwindow* m_windowHandle;
	private:
		VulkanUtiliy m_VU;

		VkInstance m_Instance;
		VkDebugUtilsMessengerEXT m_DebugMessenger;
		VkSurfaceKHR m_Surface;

		VkPhysicalDevice m_PhysicalDevice = VK_NULL_HANDLE; // This DO NOT need to be destroyed ! It'll be implicitly destroyed when the VkInstance is destroyed,
		VkDevice m_Device;

		VkQueue m_GraphicsQueue; // Device queues are implicitly cleaned up when the device is destroyed
		VkQueue m_PresentQueue; // Device queues are implicitly cleaned up when the device is destroyed

		// SwapChain
		VkSwapchainKHR m_SwapChain;
		std::vector<VkImage> m_SwapChainImages;
		VkFormat m_SwapChainImageFormat;
		VkExtent2D m_SwapChainExtent;
		std::vector<VkImageView> m_SwapChainImageViews;
		std::vector<VkFramebuffer> m_SwapChainFramebuffers;

	private:
		Ref<RenderPasses> m_RenderPasses;
	private:
		void CreateInstance();
		void SetupDebugMessenger();
		void CreateSurface();
		void PickPhysicalDevice();
		void CreateLogicalDevice();

		void CreateSwapChain();
		void CreateImageViews();

		bool DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT msgSvrty, VkDebugUtilsMessageTypeFlagsEXT msgTyp, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData);

	public:
		const VkFormat& GetswapChainImageFormat() const { return m_SwapChainImageFormat; }
		//const VkFormat& GetDepthFormat() const { return VK_FORMAT_UNDEFINED; }// m_DepthResources->GetDepthFormat(); }
	};

}
