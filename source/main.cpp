#include <iostream>

#include <config.h>
#include <render/instance.hpp>
#include <render/physical_device.hpp>
#include <render/device.hpp>

int main(int ragc, char *argv[]) {
	std::shared_ptr<vkl::instance_t> instance = vkl::instance_t::create();
	if (!instance) {
		std::cerr << "vkl::instance_t::create() failed" << std::endl;
		return 1;
	}

	if (instance->devices().empty()) {
		std::cerr << "No Vulkan Device found in system" << std::endl;
		return 1;
	}

	std::shared_ptr<vkl::device_t> device = vkl::device_t::create(instance->devices().front());
	if (!device) {
		std::cerr << "vkl::device_t::create() failed" << std::endl;
		return 1;
	}

	device->DeviceWaitIdle(device->handle());
	return 0;
}
