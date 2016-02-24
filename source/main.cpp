#include <iostream>

#include <Config.h>
#include <render/RenderInstance.hpp>
#include <render/RenderDevice.hpp>

int main(int ragc, char *argv[]) {
	std::shared_ptr<vkl::RenderInstance> renderInstance = vkl::RenderInstance::Create();
	if (!renderInstance || renderInstance->getDevices().empty()) {
		std::cerr << "vkl::RenderInstance::Create() failed" << std::endl;
		return 1;
	}

	std::shared_ptr<vkl::RenderDevice> renderDevice = vkl::RenderDevice::Create(renderInstance, renderInstance->getDevices().front());
	if (!renderDevice) {
		std::cerr << "vkl::RenderDevice::Create() failed" << std::endl;
		return 1;
	}

	return 0;
}
