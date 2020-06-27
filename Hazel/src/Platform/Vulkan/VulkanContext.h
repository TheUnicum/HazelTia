#pragma once

#include "Hazel/Renderer/GraphicsContext.h"

#include "Platform/Vulkan/VulkanUtiliy.h"

#include "vulkan/vulkan.h"

#include "Platform/Vulkan/RenderPasses.h"
#include "Platform/Vulkan/Pipeline.h"
#include "Platform/Vulkan/CommandBuffer.h"

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
		virtual void Bind() override;

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

		VkSemaphore m_ImageAvailableSemaphore; // an image has been acquired and is ready for rendering.
		VkSemaphore m_RenderFinishedSemaphore; // rendering has finished and presentation can happen.

		// SwapChain
		VkSwapchainKHR m_SwapChain;
		std::vector<VkImage> m_SwapChainImages;
		VkFormat m_SwapChainImageFormat;
		VkExtent2D m_SwapChainExtent;
		std::vector<VkImageView> m_SwapChainImageViews;
		std::vector<VkFramebuffer> m_SwapChainFramebuffers;

		VkCommandPool m_CommandPool;

	private:
		Ref<RenderPasses> m_RenderPasses;
		Ref<Pipeline> m_Pipeline;
		Ref<CommandBuffer> m_CmdBuffer;
	private:
		void CreateInstance();
		void SetupDebugMessenger();
		void CreateSurface();
		void PickPhysicalDevice();
		void CreateLogicalDevice();

		void CreateSyncObjects();

		void CreateCommandPool();
		void CreateCommandBuffers(); // need CommandPool

		void CreateSwapChain();
		void CreateImageViews();

		void BindRenderPass(); // need SwapChain

		void CreateFramebuffers();



		bool DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT msgSvrty, VkDebugUtilsMessageTypeFlagsEXT msgTyp, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData);

	public:
		VkDevice& GetDevice() { return m_Device; }
		VkPhysicalDevice& GetPhysicalDevice() { return m_PhysicalDevice; }

		VkExtent2D& GeSwapchainExtent() { return m_SwapChainExtent; }
		VkRenderPass& GetRenderPass() { return m_RenderPasses->Get(); }
		
		VkCommandPool& GetCommandPool() { return m_CommandPool; }

		const VkFormat& GetswapChainImageFormat() const { return m_SwapChainImageFormat; }
		const VkFormat& GetDepthFormat() const { return VK_FORMAT_UNDEFINED; }// m_DepthResources->GetDepthFormat(); }

		VkPipeline& GetPipeline() { return m_Pipeline->Get(); }
		VkPipelineLayout& GetPipelineLayout() { return m_Pipeline->GetLayout(); }

		VkQueue GetGraphicsQueue() { return m_GraphicsQueue; }
		VkQueue GetPresentQueue() { return m_PresentQueue; }
		//Api Render
	public:
		void BindVertexBuffer(const VkBuffer& vb);
		void BindIndexBuffer(const VkBuffer& ib);
		void BindPipeline(Ref<Pipeline>& pipeline); // Bind Shader !
		void UnbindPipeline(); // Bind Shader !

		void CmdClear_impl() override;
		void CmdDrawArrays_impl(uint32_t vertexCount, uint32_t offset = 0) override;
		void CmdDrawArraysInstanced_impl(uint32_t vertexCount, uint32_t instanceCount) override;
		void CmdDrawIndexted_impl(uint32_t indexCount, uint32_t offset = 0) override;
		void CmdDrawIndextedInstanced_impl(uint32_t indexCount, uint32_t instanceCount) override;

	};

}
