#include <iostream>
#include <algorithm>

#include <Config.h>
#include <render/Render.hpp>
#include <render/RenderInstance.hpp>
#include <render/PhysicalDevice.hpp>

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

static uint32_t const vklRequiredLayersSize = sizeof(vklRequiredLayers) / sizeof(*vklRequiredLayers);

static char const * vklRequiredExtensions[] = {
	VK_KHR_SURFACE_EXTENSION_NAME,
#ifdef _WIN32
	VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
#else
	VK_KHR_XCB_SURFACE_EXTENSION_NAME,
#endif
	VK_EXT_DEBUG_REPORT_EXTENSION_NAME,
};

static uint32_t const vklRequiredExtensionsSize = sizeof(vklRequiredExtensions) / sizeof (*vklRequiredExtensions);

namespace vkl {
	RenderInstance::RenderInstance(std::shared_ptr<Render> render)
		: m_render(render), m_instance(nullptr) {
	}

	RenderInstance::~RenderInstance() {
		if (DestroyInstance && m_instance) {
			DestroyInstance(m_instance, nullptr);
		}
	}

	std::shared_ptr<RenderInstance> RenderInstance::Create(std::shared_ptr<Render> render) {
		std::shared_ptr<RenderInstance> instance = std::make_shared<RenderInstance>(render);

		VkApplicationInfo const applicationInfo = {
			VK_STRUCTURE_TYPE_APPLICATION_INFO,
			nullptr,
			Config.Application.name,
			Config.Application.version,
			Config.Framework.name,
			Config.Framework.version,
			VK_API_VERSION,
		};

		VkInstanceCreateInfo const createInfo = {
			VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
			nullptr,
			0,
			&applicationInfo,
			vklRequiredLayersSize,
			vklRequiredLayers,
			vklRequiredExtensionsSize,
			vklRequiredExtensions,
		};

		VkResult const createResult = instance->m_render->CreateInstance(&createInfo, nullptr, &instance->m_instance);
		if (createResult != VK_SUCCESS || !instance->m_instance) {
			std::cerr << "Failed to call vkCreateInstance: " << createResult << std::endl;
			return nullptr;
		}

		VKL_GET_PROC_INSTANCE(render, instance->getInstance(), DestroyInstance);
		VKL_GET_PROC_INSTANCE(render, instance->getInstance(), EnumeratePhysicalDevices);

		uint32_t devicesCount = 0;
		VkResult const devicesCountResult = instance->EnumeratePhysicalDevices(instance->m_instance, &devicesCount, nullptr);
		if (devicesCountResult != VK_SUCCESS) {
			std::cerr << "Failed to call vkEnumeratePhysicalDevices: " << devicesCountResult << std::endl;
			return nullptr;
		}

		std::vector<VkPhysicalDevice> devices;
		devices.resize(devicesCount);

		VkResult const devicesResult = instance->EnumeratePhysicalDevices(instance->m_instance, &devicesCount, devices.data());
		if (devicesResult != VK_SUCCESS) {
			std::cerr << "Failed to call vkEnumerateInstanceLayerProperties: " << devicesResult << std::endl;
			return nullptr;
		}

		std::for_each(std::begin(devices), std::end(devices), [&instance](VkPhysicalDevice const & d){
			instance->m_devices.push_back(PhysicalDevice::Create(instance, d));
		});

		return instance;
	}
}
