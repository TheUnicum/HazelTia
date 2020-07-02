#pragma once
#include "Hazel/Renderer/GraphicsContext.h"

#include "Platform/Vulkan/VulkanUtiliy.h"
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
		
		virtual void Init() override;
		virtual void SwapBuffers() override;
		// API functions
		void CmdClear_impl() override;
		void CmdClearColor_impl(float red, float green, float blue) override;
		void CmdDrawArrays_impl(uint32_t vertexCount, uint32_t offset = 0) override;
		void CmdDrawArraysInstanced_impl(uint32_t vertexCount, uint32_t instanceCount) override;
		void CmdDrawIndexted_impl(uint32_t indexCount, uint32_t offset = 0) override;
		void CmdDrawIndextedInstanced_impl(uint32_t indexCount, uint32_t instanceCount) override;

		virtual API GetAPI() const override { return API::Vulkan; };
		virtual std::string GetAPI_TEXT() const override { return "Vulkan"; };
		virtual API MakeCurrent() override;
	private:
		Window& m_window;
		GLFWwindow* m_windowHandle;
	private:
		void CleanUpSwapChain();
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

		// External Components
		Ref<RenderPasses> m_RenderPasses;
		Ref<Pipeline> m_Pipeline;
		Ref<CommandBuffer> m_CmdBuffer;
	private:
		void CreateInstance();
		void SetupDebugMessenger();
		void CreateSurface();
		void PickPhysicalDevice();
		void CreateLogicalDevice();
		void CreateSyncObjects(); // SyncObj

		void CreateCommandPool();
		void CreateCommandBuffers(); // need CommandPool

		void CreateSwapChain();
		void CreateImageViews();

		void BindRenderPass(); // need SwapChain

		void CreateFramebuffers();
		void LinkRenderPass();

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
		VkDescriptorSet& GetPipelineDescSet() { return m_Pipeline->GetDescriptorSet(); }
		VkPipelineLayout& GetPipelineLayout() { return m_Pipeline->GetLayout(); }
		bool GetPipelineHasDesctriptorSet() { return m_Pipeline->HasDesctriptorSet(); }
		VkQueue GetGraphicsQueue() { return m_GraphicsQueue; }
		VkQueue GetPresentQueue() { return m_PresentQueue; }
	public:
		//Api Exclusive Vulkan Render
		void BindVertexBuffer(const VkBuffer& vb); // TODO make direcly on the queue!
		void BindIndexBuffer(const VkBuffer& ib);
		void BindPipeline(Ref<Pipeline>& pipeline); // Bind Shader !
		void UnbindPipeline(); // Bind Shader !
	};

}
