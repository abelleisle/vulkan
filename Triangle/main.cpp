#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>
#include <stdexcept>
#include <functional>
#include <vector>
#include <cstring>

const int WIDTH = 1280;
const int HEIGHT = 720;

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
	}

	void createInstance() {
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

		// GLFW EXTENSIONS

		unsigned int glfwExtCount = 0;
		const char** glfwExt;

		glfwExt = glfwGetRequiredInstanceExtensions(&glfwExtCount);

		std::cout << "Available GLFW extensions:" << std::endl;
		for (int i = 0; i < glfwExtCount; i++) {
			std::cout << "\t" << glfwExt[i] << std::endl;
		}

		createInfo.enabledExtensionCount = glfwExtCount;
		createInfo.ppEnabledExtensionNames = glfwExt;
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

		std::cout << "Available vulkan extensions: " << std::endl;

		for (const auto& e : vkExtensions) {
			std::cout << "\t" << e.extensionName << std::endl;
		}
	
		// CHECKING IF EXTENSIONS REQUIREMENTS ARE MET
		for (int r = 0; r < glfwExtCount; r++) {
			if (std::none_of(std::begin(vkExtensions), std::end(vkExtensions), [glfwExt, r](auto v){return(!strcmp(glfwExt[r], v.extensionName));}))
				throw std::runtime_error("Required extensions not found!");
		}

	}

	void mainLoop() {
		while (!glfwWindowShouldClose(window)) { // run mainloop until the window closes
			glfwPollEvents(); // check for window events, like window closing events
		}
	}

	void cleanup() {
		vkDestroyInstance(instance, nullptr);

		glfwDestroyWindow(window); // close the window and destory all of its data

		glfwTerminate(); // stop the glfw process
	}
};

int main() {
	TriangleApp app;

	try {
		app.run();
	} catch (const std::runtime_error& e) {
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
