#include <iostream>
#include <algorithm>
#include <vector>

#include <Config.h>
#include <render/RenderInstance.hpp>

// runtime helper functions (source https://github.com/glfw/glfw)
#ifdef _WIN32
#include <windows.h>

#define vklOpenLibrary(name) LoadLibraryA(name)
#define vklCloseLibrary(handle) FreeLibrary((HMODULE) handle)
#define vklGetProcAddress(handle, name) GetProcAddress((HMODULE) handle, name)

static char const * vklLibraryName = "vulkan-1.dll";
#else
#include <dlfcn.h>

#define vklOpenLibrary(name) dlopen(name, RTLD_LAZY | RTLD_LOCAL)
#define vklCloseLibrary(handle) dlclose(handle)
#define vklGetProcAddress(handle, name) dlsym(handle, name)

static char const * vklLibraryName = "libvulkan.so.1";
#endif

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
	"VK_LAYER_LUNARG_api_dump",
	"VK_LAYER_RENDERDOC_Capture",
};

static uint32_t const vklRequiredLayersSize = sizeof(vklRequiredLayers) / sizeof(*vklRequiredLayers);

static char const * vklRequiredExtensions[] = {
	VK_KHR_SWAPCHAIN_EXTENSION_NAME,
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
	std::shared_ptr<RenderInstance> RenderInstance::Create() {
		std::shared_ptr<RenderInstance> instance = std::make_shared<RenderInstance>();
		instance->m_library = (LibraryHandle)vklOpenLibrary(vklLibraryName);
		if (!instance->m_library) {
			std::cerr << "Failed to open library " << vklLibraryName << std::endl;
			return nullptr;
		}

		instance->GetInstanceProcAddr = (PFN_vkGetInstanceProcAddr)vklGetProcAddress(instance->m_library, "vkGetInstanceProcAddr");
		if (!instance->GetInstanceProcAddr) {
			std::cerr << "Failed to get vkGetInstanceProcAddr address" << std::endl;
			return nullptr;
		}

		instance->CreateInstance = (PFN_vkCreateInstance)instance->GetInstanceProcAddr(nullptr, "vkCreateInstance");
		if (!instance->CreateInstance) {
			std::cerr << "Failed to get vkCreateInstance address" << std::endl;
			return nullptr;
		}

		instance->EnumerateInstanceLayerProperties = (PFN_vkEnumerateInstanceLayerProperties)instance->GetInstanceProcAddr(nullptr, "vkEnumerateInstanceLayerProperties");
		if (!instance->EnumerateInstanceLayerProperties) {
			std::cerr << "Failed to get vkEnumerateInstanceLayerProperties address" << std::endl;
			return nullptr;
		}

		instance->EnumerateInstanceExtensionProperties = (PFN_vkEnumerateInstanceExtensionProperties)instance->GetInstanceProcAddr(nullptr, "vkEnumerateInstanceExtensionProperties");
		if (!instance->EnumerateInstanceExtensionProperties) {
			std::cerr << "Failed to get vkEnumerateInstanceExtensionProperties address" << std::endl;
			return nullptr;
		}

		uint32_t layersCount = 0;
		VkResult const layersCountResult = instance->EnumerateInstanceLayerProperties(&layersCount, nullptr);
		if (layersCountResult != VK_SUCCESS) {
			std::cerr << "Failed to call vkEnumerateInstanceLayerProperties: " << layersCountResult << std::endl;
			return nullptr;
		}

		instance->m_layers.resize(layersCount);

		VkResult const layersResult = instance->EnumerateInstanceLayerProperties(&layersCount, instance->m_layers.data());
		if (layersResult != VK_SUCCESS) {
			std::cerr << "Failed to call vkEnumerateInstanceLayerProperties: " << layersResult << std::endl;
			return nullptr;
		}

		std::cout << "Available Vulkan layers:" << std::endl;
		std::for_each(std::begin(instance->m_layers), std::end(instance->m_layers), [](VkLayerProperties const & l){
			std::cout << l.layerName << std::endl;
		});
		std::cout << std::endl;

		uint32_t extensionsCount = 0;
		VkResult const extensionsCountResult = instance->EnumerateInstanceExtensionProperties(nullptr, &extensionsCount, nullptr);
		if (extensionsCountResult != VK_SUCCESS) {
			std::cerr << "Failed to call vkEnumerateInstanceExtensionProperties: " << extensionsCountResult << std::endl;
			return nullptr;
		}

		instance->m_extensions.resize(extensionsCount);

		VkResult const extensionsResult = instance->EnumerateInstanceExtensionProperties(nullptr, &extensionsCount, instance->m_extensions.data());
		if (extensionsResult != VK_SUCCESS) {
			std::cerr << "Failed to call vkEnumerateInstanceExtensionProperties: " << extensionsResult << std::endl;
			return nullptr;
		}

		std::cout << "Available Vulkan extensions:" << std::endl;
		std::for_each(std::begin(instance->m_extensions), std::end(instance->m_extensions), [](VkExtensionProperties const & e){
			std::cout << e.extensionName << std::endl;
		});
		std::cout << std::endl;

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

		VkResult const createResult = instance->CreateInstance(&createInfo, nullptr, &instance->m_instance);
		if (createResult != VK_SUCCESS || !instance->m_instance) {
			std::cerr << "Failed to call vkCreateInstance: " << createResult << std::endl;
			return nullptr;
		}

		instance->DestroyInstance = (PFN_vkDestroyInstance)instance->GetInstanceProcAddr(instance->m_instance, "vkDestroyInstance");
		if (!instance->DestroyInstance) {
			std::cerr << "Failed to get vkDestroyInstance address" << std::endl;
			return nullptr;
		}

		return instance;
	}

	RenderInstance::RenderInstance() : m_library(nullptr), m_instance(nullptr) {
	}

	RenderInstance::~RenderInstance() {
		if (DestroyInstance && m_instance) {
			DestroyInstance(m_instance, nullptr);
		}
		vklCloseLibrary(m_library);
	}
}
