#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>
#include <stdexcept>
#include <functional>
#include <vector>
#include <cstring>

const int WIDTH = 1280;
const int HEIGHT = 720;

const std::vector<const char*> validationLayers = {"VK_LAYER_LUNARG_standard_validation"};

#ifdef NDEBUG
	const bool enableValidationLayers = false;
#else
	const bool enableValidationLayers = true;
#endif //NDEBUG

VkResult CreateDebugReportCallbackEXT(VkInstance instance, 
									  const VkDebugReportCallbackCreateInfoEXT* pCreateInfo, 
									  const VkAllocationCallbacks* pAllocator, 
									  VkDebugReportCallbackEXT* pCallback)
{
	auto func = (PFN_vkCreateDebugReportCallbackEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugReportCallbackEXT");
    if (func != nullptr) {
		return func(instance, pCreateInfo, pAllocator, pCallback);
    } else {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

class TriangleApp {
public:
	void run() {
		initWindow();
		initVulkan();
		mainLoop();
		cleanup();
	}
private:

	GLFWwindow* window;
	VkInstance instance;
	VkDebugReportCallbackEXT callback;

	void initWindow() {
		// init GLFW
		glfwInit();

		// tell glfw not to use OpenGL
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		// tell glfw to not allow window resizing
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

		// set all of the window data in our window var
		window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan test", nullptr, nullptr);
	}

	void initVulkan() {
		createInstance();
		setupDebugCallback();
	}

	void setupDebugCallback() {
		if (!enableValidationLayers) return;

		VkDebugReportCallbackCreateInfoEXT createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
		createInfo.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT;
		createInfo.pfnCallback = debugCallback;

		if (CreateDebugReportCallbackEXT(instance, &createInfo, nullptr, &callback) != VK_SUCCESS) {
			throw std::runtime_error("failed to setup debug callback");
		}
	}

	void createInstance() {
		
		if (enableValidationLayers && !checkValidationLayerSupport()) {
			throw std::runtime_error("validation layers requested, but not available!");
		}
		
		VkApplicationInfo appInfo = {};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pApplicationName = "Hello Triangle";
		appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.pEngineName = "No Engine";
		appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.apiVersion = VK_API_VERSION_1_0;

		VkInstanceCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		createInfo.pApplicationInfo = &appInfo;

		if (enableValidationLayers) {
			createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
			createInfo.ppEnabledLayerNames = validationLayers.data();
		} else {
			createInfo.enabledLayerCount = 0;
		}

		// GLFW EXTENSIONS

		auto extensions = getRequiredExtensions();
		createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
		createInfo.ppEnabledExtensionNames = extensions.data();
		createInfo.enabledLayerCount = 0;

		// we can finally use our created structures to create an instance
		if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
			throw std::runtime_error("failed to create instance!");
		}

		// VULKAN EXTENSIONS

		uint32_t vkExtensionCount = 0;
		vkEnumerateInstanceExtensionProperties(nullptr, &vkExtensionCount, nullptr);

		std::vector<VkExtensionProperties> vkExtensions(vkExtensionCount);

		vkEnumerateInstanceExtensionProperties(nullptr, &vkExtensionCount, vkExtensions.data());	
	} //createInstance()

	bool checkValidationLayerSupport() {
		uint32_t layerCount;
		vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

		std::vector<VkLayerProperties> availableLayers(layerCount);
		vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

		for (const char* layerName : validationLayers) {
    		bool layerFound = false;

			for (const auto& layerProperties : availableLayers) {
				if (strcmp(layerName, layerProperties.layerName) == 0) {
					layerFound = true;
					break;
				}
			}

			if (!layerFound) {
				return false;
			}
		}

		return true;
	}

	std::vector<const char*> getRequiredExtensions() {
		std::vector<const char*> extensions;

		unsigned int glfwExtCount = 0;
		const char** glfwExt;
		glfwExt = glfwGetRequiredInstanceExtensions(&glfwExtCount);

		for (unsigned int i = 0; i < glfwExtCount; i++) {
			extensions.push_back(glfwExt[i]);
		}

		if (enableValidationLayers) {
			extensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
		}

		return extensions;

	}

	void mainLoop() {
		while (!glfwWindowShouldClose(window)) { // run mainloop until the window closes
			glfwPollEvents(); // check for window events, like window closing events
		}
	}

	void DestroyDebugReportCallbackEXT(VkInstance instance,
									   VkDebugReportCallbackEXT callback,
									   const VkAllocationCallbacks* pAllocator)
	{
		auto func = (PFN_vkDestroyDebugReportCallbackEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugReportCallbackEXT");
		if (func != nullptr) {
			func(instance, callback, pAllocator);
		}
	}

	void cleanup() {
		DestroyDebugReportCallbackEXT(instance, callback, nullptr);
		vkDestroyInstance(instance, nullptr);

		glfwDestroyWindow(window); // close the window and destory all of its data

		glfwTerminate(); // stop the glfw process
	}

	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
		VkDebugReportFlagsEXT flags,
		VkDebugReportObjectTypeEXT objType,
		uint64_t obj,
		size_t location,
		int32_t code,
		const char* layerPrefix,
		const char* msg,
		void* userData)
	{
		std::cerr << "validation layer: " << msg << std::endl;

		return VK_FALSE;
	}
};

int main() {

	try {
		TriangleApp app;
		app.run();
	} catch (const std::runtime_error& e) {
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
