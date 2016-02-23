#include <iostream>

#include <Config.h>
#include <render/RenderInstance.hpp>

int main(int ragc, char *argv[]) {
	std::shared_ptr<vkl::RenderInstance> instance = vkl::RenderInstance::Create();
	if (!instance) {
		std::cerr << "Failed to init render instance" << std::endl;
		return 1;
	}

	return 0;
}
