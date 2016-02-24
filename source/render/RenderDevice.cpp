#include <iostream>

#include <render/RenderInstance.hpp>
#include <render/RenderDevice.hpp>

#define VKL_GET_PROC_DEVICE(__name) \
	instance->__name = (PFN_vk ## __name)instance->m_instance->GetDeviceProcAddr(instance->m_device, "vk" # __name); \
	if (!instance->__name) { \
		std::cerr << "vkGetDeviceProcAddr(vk " #__name ") failed" << std::endl; \
		return nullptr; \
	}

static char const * vklRequiredLayers[] = {
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
	// debug layers
	//"VK_LAYER_LUNARG_api_dump",
	//"VK_LAYER_RENDERDOC_Capture",
};

static uint32_t const vklRequiredLayersSize = sizeof vklRequiredLayers / sizeof vklRequiredLayers[0];

static char const * vklRequiredExtensions[] = {
	VK_KHR_SWAPCHAIN_EXTENSION_NAME,
};

static uint32_t const vklRequiredExtensionsSize = sizeof vklRequiredExtensions / sizeof vklRequiredExtensions[0];

namespace vkl {
	RenderDevice::RenderDevice(std::shared_ptr<RenderInstance> renderInstance, VkPhysicalDevice physicalDevice)
		: m_instance(renderInstance), m_physicalDevice(physicalDevice), m_device(nullptr) {
	}

	RenderDevice::~RenderDevice() {
		if (DestroyDevice && m_device) {
			DestroyDevice(m_device, nullptr);
		}
	}

	std::shared_ptr<RenderDevice> RenderDevice::Create(std::shared_ptr<RenderInstance> renderInstance, VkPhysicalDevice physicalDevice) {
		std::shared_ptr<RenderDevice> instance = std::make_shared<RenderDevice>(renderInstance, physicalDevice);

		VkPhysicalDeviceFeatures features = {};
		renderInstance->GetPhysicalDeviceFeatures(physicalDevice, &features);

		VkDeviceCreateInfo const createInfo = {
			VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
			nullptr,
			0,
			0, // queueCreateInfoCount
			nullptr, // pQueueCreateInfos
			vklRequiredLayersSize,
			vklRequiredLayers,
			vklRequiredExtensionsSize,
			vklRequiredExtensions,
			&features
		};

		VkResult const createResult = renderInstance->CreateDevice(physicalDevice, &createInfo, nullptr, &instance->m_device);
		if (createResult != VK_SUCCESS || !instance->m_device) {
			std::cerr << "vkCreateDevice failed: " << FormatResult(createResult) << std::endl;
			return nullptr;
		}

		VKL_GET_PROC_DEVICE(DestroyDevice);
		VKL_GET_PROC_DEVICE(DeviceWaitIdle);

		return instance;
	}
}
