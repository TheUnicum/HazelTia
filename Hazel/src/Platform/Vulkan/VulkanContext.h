#pragma once

#include "Hazel/Renderer/GraphicsContext.h"

#include "Platform/Vulkan/VulkanUtiliy.h"

#include "vulkan/vulkan.h"

struct GLFWwindow;

namespace Hazel {

	class VulkanContext : public GraphicsContext
	{
	public:
		friend class VulkanUtiliy;
	public:
		VulkanContext(Window& window);
		~VulkanContext();

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

	private:
		void CreateInstance();
		void SetupDebugMessenger();
		void CreateSurface();



		bool DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT msgSvrty, VkDebugUtilsMessageTypeFlagsEXT msgTyp, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData);
	};

}
