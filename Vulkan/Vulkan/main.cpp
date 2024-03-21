#include <iostream>
//GLFW_INCLUDE VULKAN will make glfw load the vulkan.h header for us among other things
#define GLFW_INCLUDE_VULKAN
#include "glfw3.h"
#include <stdexcept>
#include <cstdlib>
#include <vector>

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
		//Holy shit vulkan really is explicit
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

		//Vulkan being platform agnostic requires an extension to interface with the window system of the os. glfw thankfully does that for us
		//win32 sucks ass thank you glfw
		uint32_t glfwExtensionCount = 0;
		const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
		createInfo.enabledExtensionCount = glfwExtensionCount;
		createInfo.ppEnabledExtensionNames = glfwExtensions;
		createInfo.enabledLayerCount = 0;

		//This code commented out lists all the supported extensions
		//uint32_t extensionCount = 0;
		////by leaving pProperties out it just returns us the extension count
		//vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
		//std::vector<VkExtensionProperties> extensions(extensionCount);
		//vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());
		//std::cout << "Available Extensions:\n";
		//for (const auto& extension : extensions)
		//{
		//	std::cout << '\t' << extension.extensionName << '\n';
		//}
		
		//Thank you vulkan for having actual fucking function names not DXGI11_CREATE_FACTORY bullshit
		VkResult result = vkCreateInstance(&createInfo, nullptr, &m_instance);
		if (result == VK_ERROR_EXTENSION_NOT_PRESENT)
		{
			
		}
		else if (result != VK_SUCCESS)
			throw std::runtime_error("Failed to create Vulkan Instance");
	}
	void InitWindow()
	{
		glfwInit();
		//Glfw will default to using opengl, so we specify that this is not the case.
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		//resizeable windows require more management so for now it will be disabled.
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

		//monitor = what monitor you want to launch the program on, share pointer is just for opengl.
		m_window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan Window", nullptr, nullptr);
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
		return false;
	}
	
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
