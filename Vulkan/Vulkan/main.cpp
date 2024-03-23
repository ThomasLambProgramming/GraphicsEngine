#include <iostream>
//GLFW_INCLUDE VULKAN will make glfw load the vulkan.h header for us among other things
#define GLFW_INCLUDE_VULKAN
#include "glfw3.h"
#include <stdexcept>
#include <cstdlib>
#include <vector>
#include <cstring>

class BaseApplication
{
public:
	void Run()
	{
		InitWindow();
		InitVulkan();
		UpdateLoop();
		Cleanup();
	}
private:
	void InitVulkan()
	{
		CreateInstance();
	}
	void CreateInstance()
	{
		if (enableValidationLayers && !CheckValidationLayerSupport())
			throw std::runtime_error("Validation layers requested, but they are not available!");


		//Holy shit vulkan really is explicit (i like)
		VkApplicationInfo appInfo{};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pApplicationName = "Triangle Boi!";
		appInfo.applicationVersion = VK_MAKE_VERSION(1,0,0);
		appInfo.pEngineName = "No Engine";
		appInfo.engineVersion = VK_MAKE_VERSION(1,0,0);
		//THIS IS THE VULKAN API NOT JUST A RANDOM NUMBER IVE GIVEN. (tutorial was 1.0 so im avoiding using 1.3)
		appInfo.apiVersion = VK_API_VERSION_1_0;

		//I kinda fuck with the {} init
		VkInstanceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		createInfo.pApplicationInfo = &appInfo;

		std::vector<const char*> extensions = GetRequiredExtensions();
		createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
		createInfo.ppEnabledExtensionNames = extensions.data();
		
		if (enableValidationLayers)
		{
			createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
			createInfo.ppEnabledLayerNames = validationLayers.data();
			VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo;
			PopulateDebugMessengerCreateInfo(debugCreateInfo);
			createInfo.pNext = &debugCreateInfo;
		}
		else
		{
			createInfo.enabledLayerCount = 0;
			createInfo.pNext = nullptr;	
		}
		
		//Thank you vulkan for having actual fucking function names not DXGI11_CREATE_FACTORY bullshit
		VkResult result = vkCreateInstance(&createInfo, nullptr, &m_instance);
		
		if (result == VK_ERROR_EXTENSION_NOT_PRESENT)
		{
			
		}
		else if (result != VK_SUCCESS)
			throw std::runtime_error("Failed to create Vulkan Instance");

		if (enableValidationLayers)
			SetupDebugManager();
	}

	void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo)
	{
		createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		createInfo.messageSeverity =
			//VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
			//VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | 
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		createInfo.messageType =
			VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;

		createInfo.pfnUserCallback = DebugCallback;
	}
	
	void InitWindow()
	{
		glfwInit();
		//Glfw will default to using opengl, so we specify that this is not the case.
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		//resizeable windows require more management so for now it will be disabled.
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

		//monitor = what monitor you want to launch the program on, share pointer is just for opengl.
		m_window = glfwCreateWindow(WIDTH, HEIGHT, "Triangle Boi!", nullptr, nullptr);
	}
	void UpdateLoop()
	{
		while (!glfwWindowShouldClose(m_window))
		{
			glfwPollEvents();
		}
	}
	void Cleanup()
	{
		if (enableValidationLayers)
		{
			DestroyDebugUtilsMessengerEXT(m_instance, debugMessenger, nullptr);
		}
		vkDestroyInstance(m_instance, nullptr);
		glfwDestroyWindow(m_window);
		glfwTerminate();
	}
	bool CheckValidationLayerSupport()
	{
		uint32_t layerCount;
		vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
		std::vector<VkLayerProperties> availableLayers(layerCount);
		vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

		for (const char* layerName : validationLayers)
		{
			bool layerFound = false;
			for (const VkLayerProperties& layerProperties : availableLayers)
			{
				if (strcmp(layerName, layerProperties.layerName) == 0)
				{
					layerFound = true;
					break;
				}
			}
			if (layerFound == false)
				return false;
		}
		
		return true;
	}
	std::vector<const char*> GetRequiredExtensions()
	{
		uint32_t glfwExtensionCount = 0;
		const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
		std::vector<const char*>extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

		//add debug utils extension
		if (enableValidationLayers)
			extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
		return extensions;
	}

	void DestroyDebugUtilsMessengerEXT(VkInstance a_instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator)
	{
		auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(a_instance, "vkDestroyDebugUtilsMessengerEXT");
		if (func != nullptr)
			func(a_instance, debugMessenger, pAllocator);
	}

	VkResult CreateDebugUtilsMessengerEXT(
		VkInstance a_instance,
		const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
		VkAllocationCallbacks* pAllocator,
		VkDebugUtilsMessengerEXT* pDebugMessenger)
	{
		auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(a_instance, "vkCreateDebugUtilsMessengerEXT");
		
		if (func != nullptr)
			return func(a_instance, pCreateInfo, pAllocator, pDebugMessenger);
		else
			return VK_ERROR_EXTENSION_NOT_PRESENT;
	}
	void SetupDebugManager()
	{
		if (!enableValidationLayers)
			return;

		VkDebugUtilsMessengerCreateInfoEXT createInfoExt;
		PopulateDebugMessengerCreateInfo(createInfoExt);
		//Void pointer for pUserData so we could gather everything about the users pc and give it to the callback for instance.
		createInfoExt.pUserData = nullptr;
		
		if (CreateDebugUtilsMessengerEXT(m_instance, &createInfoExt, nullptr, &debugMessenger) != VK_SUCCESS)
			throw std::runtime_error("Could not setup debug messenger");
	}
	//VKAPI_CALL= __stdcall on windows (just tells the stack how to use the function.
	//VKAPI_ATTR = nothing on windows so we are actually just doing
	//static Vkbool32 (usestandardcall) DebugCallback
	static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserData)
	{
		std::cerr << "Validation Layer" << pCallbackData->pMessage << std::endl;
		return VK_FALSE;
	}
	VkDebugUtilsMessengerEXT debugMessenger;
	
	
	const std::vector<const char*> validationLayers = {
		"VK_LAYER_KHRONOS_validation"
	};
#ifdef NDEBUG
	const bool enableValidationLayers = false;
#else
	const bool enableValidationLayers = true;
#endif
	
	
	VkInstance m_instance;
	uint32_t WIDTH = 800;
	uint32_t HEIGHT = 600;
	GLFWwindow* m_window;
};

int main() 
{
	BaseApplication application;
	try
	{
		application.Run();
	}
	catch (const std::exception& error)
	{
		std::cerr << error.what() << std::endl;
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}
