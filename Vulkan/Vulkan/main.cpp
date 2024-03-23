#include <iostream>
//GLFW_INCLUDE VULKAN will make glfw load the vulkan.h header for us among other things
#define GLFW_INCLUDE_VULKAN
#include "glfw3.h"
#include <stdexcept>
#include <cstdlib>
#include <vector>
#include <cstring>
#include <optional>

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
		SetupDebugManager();
		SelectPhysicalDevice();
		CreateLogicalDevice();
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
			createInfo.enabledLayerCount = static_cast<uint32_t>(ValidationLayers.size());
			createInfo.ppEnabledLayerNames = ValidationLayers.data();
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

	}
	void CreateLogicalDevice()
	{
		QueueFamilyIndicies indicies = FindQueueFamilies(physicalDevice);
		VkDeviceQueueCreateInfo queueCreateInfo;
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = indicies.graphicsFamily.value();
		queueCreateInfo.queueCount = 1;
		float queuePriority = 1.0f;
		queueCreateInfo.pQueuePriorities = &queuePriority;
		VkPhysicalDeviceFeatures deviceFeatures{};

		VkDeviceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		createInfo.pQueueCreateInfos = &queueCreateInfo;
		createInfo.queueCreateInfoCount = 1;
		createInfo.pEnabledFeatures = &deviceFeatures;
		createInfo.enabledExtensionCount = 0;
		
		if (enableValidationLayers)
		{
			createInfo.enabledLayerCount = static_cast<uint32_t>(ValidationLayers.size());
			createInfo.ppEnabledLayerNames = ValidationLayers.data();
		}
		else
			createInfo.enabledLayerCount = 0;

		if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &m_device) != VK_SUCCESS)
			throw std::runtime_error("Failed to create logical device for vulkan!");

		vkGetDeviceQueue(m_device, indicies.graphicsFamily.value(), 0, &m_graphicsQueue);
	}
	void SelectPhysicalDevice()
	{
		uint32_t deviceCount = 0;
		vkEnumeratePhysicalDevices(m_instance, &deviceCount, nullptr);
		if (deviceCount == 0)
			throw std::runtime_error("There are no devices that support vulkan!");
		std::vector<VkPhysicalDevice> physicalDevices(deviceCount);
		vkEnumeratePhysicalDevices(m_instance, &deviceCount, physicalDevices.data());

		for (const auto& device : physicalDevices)
		{
			if (IsPhysicalDeviceSuitable(device))
			{
				physicalDevice = device;
				break;
			}
		}
		if (physicalDevice == VK_NULL_HANDLE)
			throw std::runtime_error("Could not find suitable gpu");
	}
	bool IsPhysicalDeviceSuitable(VkPhysicalDevice a_device)
	{
		VkPhysicalDeviceProperties deviceProperties;
		VkPhysicalDeviceFeatures deviceFeatures;
		vkGetPhysicalDeviceProperties(a_device, &deviceProperties);
		vkGetPhysicalDeviceFeatures(a_device, &deviceFeatures);

		QueueFamilyIndicies indicies = FindQueueFamilies(a_device);
		
		//we only want discrete gpus and if they are capable of geometry shaders. and it can support queues that take graphics commands.
		return indicies.isComplete() && deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU && deviceFeatures.geometryShader;

		//Small note. as the tutorial mentions you can rank physical devices by score and use a std::multimap<int, VkPhysicalDevice> to auto sort by highest score.
		//didnt know multimap was a thing that is very cool.
	}
	struct QueueFamilyIndicies
	{
		//using optional since uint32_t will always be a value but with optional it will be blank until we have a value for it
		std::optional<uint32_t> graphicsFamily;
		bool isComplete () {return graphicsFamily.has_value();}
	};
	QueueFamilyIndicies FindQueueFamilies(VkPhysicalDevice a_device)
	{
		QueueFamilyIndicies indicies;
		uint32_t queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(a_device, &queueFamilyCount, nullptr);
		std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(a_device, &queueFamilyCount, queueFamilies.data());

		int i = 0;
		for (const auto& queueFamily : queueFamilies)
		{
			if (indicies.isComplete())
				break;
			if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
				indicies.graphicsFamily = i;
			i++;
		}
		return indicies;
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
			DestroyDebugUtilsMessengerEXT(m_instance, DebugMessenger, nullptr);
		}
		vkDestroyDevice(m_device, nullptr);
		vkDestroyInstance(m_instance, nullptr);
		glfwDestroyWindow(m_window);
		glfwTerminate();
	}

#pragma region ValidationAndExtensions
	bool CheckValidationLayerSupport()
	{
		uint32_t layerCount;
		vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
		std::vector<VkLayerProperties> availableLayers(layerCount);
		vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

		for (const char* layerName : ValidationLayers)
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
#pragma endregion 
#pragma region Debugging
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
		
		if (CreateDebugUtilsMessengerEXT(m_instance, &createInfoExt, nullptr, &DebugMessenger) != VK_SUCCESS)
			throw std::runtime_error("Could not setup debug messenger");
	}
	
	void DestroyDebugUtilsMessengerEXT(VkInstance a_instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator)
	{
		auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(a_instance, "vkDestroyDebugUtilsMessengerEXT");
		if (func != nullptr)
			func(a_instance, debugMessenger, pAllocator);
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
	VkDebugUtilsMessengerEXT DebugMessenger;
#pragma endregion 
	
	const std::vector<const char*> ValidationLayers = {
		"VK_LAYER_KHRONOS_validation"
	};
	
#ifdef NDEBUG
	const bool enableValidationLayers = false;
#else
	const bool enableValidationLayers = true;
#endif

	VkSurfaceKHR m_surface;
	VkQueue m_graphicsQueue;
	VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
	VkDevice m_device;
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
