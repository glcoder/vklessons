#include <iostream>

#include <render/instance.hpp>
#include <render/physical_device.hpp>
#include <render/device.hpp>

#define VKL_GET_PROC_DEVICE(__name) \
	device->__name = (PFN_vk ## __name)physical_device->instance()->GetDeviceProcAddr(device->m_handle, "vk" # __name); \
	if (!device->__name) { \
		std::cerr << "vkGetDeviceProcAddr(vk " #__name ") failed" << std::endl; \
		return nullptr; \
	}

static char const * _vkl_required_layers[] = {
	// validation layers
	"VK_LAYER_LUNARG_device_limits",
	"VK_LAYER_LUNARG_draw_state",
	"VK_LAYER_LUNARG_image",
	"VK_LAYER_LUNARG_mem_tracker",
	"VK_LAYER_LUNARG_object_tracker",
	"VK_LAYER_LUNARG_param_checker",
	"VK_LAYER_LUNARG_swapchain",
	"VK_LAYER_LUNARG_threading",
	"VK_LAYER_GOOGLE_unique_objects",
	"VK_LAYER_LUNARG_standard_validation",
	// debug layers
	//"VK_LAYER_LUNARG_api_dump",
	//"VK_LAYER_RENDERDOC_Capture",
};

static char const * _vkl_required_extensions[] = {
	VK_KHR_SWAPCHAIN_EXTENSION_NAME,
};

namespace vkl {
	device_t::device_t(std::shared_ptr<physical_device_t> physical_device)
		: m_handle(nullptr), m_physical_device(physical_device) {
	}

	device_t::~device_t() {
		if (DestroyDevice && m_handle) {
			DestroyDevice(m_handle, nullptr);
		}
	}

	std::shared_ptr<device_t> device_t::create(std::shared_ptr<physical_device_t> physical_device) {
		std::shared_ptr<device_t> device = std::make_shared<device_t>(physical_device);

		float const priorities[] = { 1.0f };
		VkDeviceQueueCreateInfo const queues_info[] = { {
				VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
				nullptr,
				0,
				physical_device->queue_index(VK_QUEUE_GRAPHICS_BIT),
				1,
				priorities
			} };

		VkDeviceCreateInfo const create_info = {
			VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
			nullptr,
			0,
			ARRAYSIZE(queues_info),
			queues_info,
			ARRAYSIZE(_vkl_required_layers),
			_vkl_required_layers,
			ARRAYSIZE(_vkl_required_extensions),
			_vkl_required_extensions,
			&physical_device->features()
		};

		VkResult const create_result = physical_device->instance()->CreateDevice(physical_device->handle(), &create_info, nullptr, &device->m_handle);
		if (create_result != VK_SUCCESS || !device->m_handle) {
			std::cerr << "vkCreateDevice failed: " << format::result(create_result) << std::endl;
			return nullptr;
		}

		VKL_GET_PROC_DEVICE(DestroyDevice);
		VKL_GET_PROC_DEVICE(DeviceWaitIdle);

		//physical_device->devices().push_back(device);
		return device;
	}
}
