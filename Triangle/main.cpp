#include <vulkan/vulkan.h>

#include <iostream>
#include <stdexcept>
#include <functional>

class TriangleApp {
public:
	void run() {
		initVulkan();
		mainLoop();
		cleanup();
	}
private:
	void initVulkan() {

	}

	void mainLoop() {

	}

	void cleanup() {

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
