#include "hzpch.h"
#include "Platform/Vulkan/VulkanContext.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>


namespace Hazel {


	VulkanContext::VulkanContext(Window& window)
		: m_window(window), m_windowHandle((GLFWwindow*)window.GetNativeWindow())
	{
		HZ_CORE_ASSERT(m_windowHandle, "Window handle is null!");
	}

	VulkanContext::~VulkanContext()
	{
		
		if (m_VU.ValidationLayersEnabled())
		{
			m_VU.DestroyDebugUtilsMessengerEXT(m_Instance, m_DebugMessenger, nullptr);
		}
		vkDestroySurfaceKHR(m_Instance, m_Surface, nullptr);
		vkDestroyInstance(m_Instance, nullptr);
	}

	void VulkanContext::Init()
	{
		CreateInstance();
		SetupDebugMessenger();
		CreateSurface();
	}

	void VulkanContext::SwapBuffers()
	{
	}

	API VulkanContext::MakeCurrent()
	{
		return API();
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
		std::cout << "Extension REQUESTED:\n";
		for (const auto& extension : extensions) {
			std::cout << '\t' << extension << '\n';
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
			throw std::runtime_error("Extension not available!");
		}
		
		if (vkCreateInstance(&createInfo, nullptr, &m_Instance) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create instance!");
		}
	}

	void VulkanContext::SetupDebugMessenger()
	{
		if (!m_VU.ValidationLayersEnabled()) return;

		VkDebugUtilsMessengerCreateInfoEXT createInfo;
		m_VU.PopulateDebugMessengerCreateInfo(createInfo);

		if (m_VU.CreateDebugUtilsMessengerEXT(m_Instance, &createInfo, nullptr, &m_DebugMessenger) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to set up debug messenger!");
		}
	}

	void VulkanContext::CreateSurface()
	{
		if (glfwCreateWindowSurface(m_Instance, m_windowHandle, nullptr, &m_Surface) != VK_SUCCESS)
		{
			HZ_CORE_ASSERT(false, "failed to create window surface!");
		}
	}

	// Callback function
	bool VulkanContext::DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT msgSvrty, VkDebugUtilsMessageTypeFlagsEXT msgTyp, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData)
	{
		std::cerr << "*ERR* validation layer: " << pCallbackData->pMessage << std::endl;
		return VK_FALSE;
	}

}
