#include <iostream>

#include <Config.h>
#include <render/Render.hpp>
#include <render/RenderInstance.hpp>
#include <render/PhysicalDevice.hpp>
#include <render/RenderDevice.hpp>

int main(int ragc, char *argv[]) {
	std::shared_ptr<vkl::Render> render = vkl::Render::Create();
	if (!render) {
		std::cerr << "Failed to init render" << std::endl;
		return 1;
	}

	std::shared_ptr<vkl::RenderInstance> renderInstance = vkl::RenderInstance::Create(render);
	if (!renderInstance) {
		std::cerr << "Failed to init render instance" << std::endl;
		return 1;
	}

	for (std::shared_ptr<vkl::PhysicalDevice> device : renderInstance->getDevices()) {
		// std::shared_ptr<vkl::RenderDevice> renderDevice = vkl::RenderDevice::Create(device);
		// renderDevice->DeviceWaitIdle(renderDevice->getDevice());
	}

	return 0;
}
