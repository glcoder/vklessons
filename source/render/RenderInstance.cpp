#include <iostream>

#include <Config.h>
#include <render/RenderInstance.hpp>

// runtime helper functions (source https://github.com/glfw/glfw)
#ifdef _WIN32
#include <windows.h>

#define _vkl_OpenLibrary(name) LoadLibraryA(name)
#define _vkl_CloseLibrary(handle) FreeLibrary((HMODULE) handle)
#define _vkl_GetProcAddress(handle, name) GetProcAddress((HMODULE) handle, name)

static char const * _vkl_LibraryName = "vulkan-1.dll";
#else
#include <dlfcn.h>

#define _vkl_OpenLibrary(name) dlopen(name, RTLD_LAZY | RTLD_LOCAL)
#define _vkl_CloseLibrary(handle) dlclose(handle)
#define _vkl_GetProcAddress(handle, name) dlsym(handle, name)

static char const * _vkl_LibraryName = "libvulkan.so.1";
#endif

#define VKL_GET_PROC_INSTANCE(__name) \
	instance->__name = (PFN_vk ## __name)instance->GetInstanceProcAddr(instance->m_instance, "vk" # __name); \
	if (!instance->__name) { \
		std::cerr << "vkGetInstanceProcAddr(vk " #__name ") failed" << std::endl; \
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
	VK_KHR_SURFACE_EXTENSION_NAME,
#ifdef _WIN32
	VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
#else
	VK_KHR_XCB_SURFACE_EXTENSION_NAME,
#endif
	VK_EXT_DEBUG_REPORT_EXTENSION_NAME,
};

static uint32_t const vklRequiredExtensionsSize = sizeof vklRequiredExtensions / sizeof vklRequiredExtensions[0];

namespace vkl {
	RenderInstance::RenderInstance()
		: m_library(nullptr), m_instance(nullptr) {
	}

	RenderInstance::~RenderInstance() {
		if (DestroyInstance && m_instance) {
			DestroyInstance(m_instance, nullptr);
		}
		if (m_library) {
			_vkl_CloseLibrary(m_library);
		}
	}

	void PrintLayersInformation(std::shared_ptr<RenderInstance> instance);
	void PrintExtensionsInformation(std::shared_ptr<RenderInstance> instance);
	void PrintDevicesInformation(std::shared_ptr<RenderInstance> instance);

	std::shared_ptr<RenderInstance> RenderInstance::Create() {
		std::shared_ptr<RenderInstance> instance = std::make_shared<RenderInstance>();

		instance->m_library = (LibraryHandle)_vkl_OpenLibrary(_vkl_LibraryName);
		if (!instance->m_library) {
			std::cerr << "_vkl_OpenLibrary(" << _vkl_LibraryName << ") failed" << std::endl;
			return nullptr;
		}

		instance->GetInstanceProcAddr = (PFN_vkGetInstanceProcAddr)_vkl_GetProcAddress(instance->m_library, "vkGetInstanceProcAddr");
		if (!instance->GetInstanceProcAddr) {
			std::cerr << "_vkl_GetProcAddress(vkGetInstanceProcAddr) failed" << std::endl;
			return nullptr;
		}

		VKL_GET_PROC_INSTANCE(CreateInstance);
		VKL_GET_PROC_INSTANCE(EnumerateInstanceLayerProperties);
		VKL_GET_PROC_INSTANCE(EnumerateInstanceExtensionProperties);

		PrintLayersInformation(instance);
		PrintExtensionsInformation(instance);

		VkApplicationInfo const applicationInfo = {
			VK_STRUCTURE_TYPE_APPLICATION_INFO,
			nullptr,
			Config.Application.name,
			VK_MAKE_VERSION(Config.Application.version, 0, 0),
			Config.Framework.name,
			VK_MAKE_VERSION(Config.Framework.version, 0, 0),
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
			std::cerr << "vkCreateInstance failed: " << FormatResult(createResult) << std::endl;
			return nullptr;
		}

		VKL_GET_PROC_INSTANCE(DestroyInstance);
		VKL_GET_PROC_INSTANCE(EnumeratePhysicalDevices);
		VKL_GET_PROC_INSTANCE(GetPhysicalDeviceFeatures);
		VKL_GET_PROC_INSTANCE(GetPhysicalDeviceProperties);
		VKL_GET_PROC_INSTANCE(EnumerateDeviceLayerProperties);
		VKL_GET_PROC_INSTANCE(EnumerateDeviceExtensionProperties);
		VKL_GET_PROC_INSTANCE(GetDeviceProcAddr);
		VKL_GET_PROC_INSTANCE(CreateDevice);

		PrintDevicesInformation(instance);

		uint32_t count = 0;
		VkResult const countResult = instance->EnumeratePhysicalDevices(instance->m_instance, &count, nullptr);
		if (countResult != VK_SUCCESS) {
			std::cerr << "vkEnumeratePhysicalDevices failed: " << FormatResult(countResult) << std::endl;
			return nullptr;
		}

		instance->m_devices.resize(count);
		VkResult const result = instance->EnumeratePhysicalDevices(instance->m_instance, &count, instance->m_devices.data());
		if (result != VK_SUCCESS) {
			std::cerr << "vkEnumeratePhysicalDevices failed: " << FormatResult(result) << std::endl;
			return nullptr;
		}

		return instance;
	}

	void PrintLayersInformation(std::shared_ptr<RenderInstance> instance) {
		uint32_t count = 0;
		VkResult const countResult = instance->EnumerateInstanceLayerProperties(&count, nullptr);
		if (countResult != VK_SUCCESS) {
			std::cerr << "vkEnumerateInstanceLayerProperties failed: " << FormatResult(countResult) << std::endl;
			return;
		}

		std::vector<VkLayerProperties> layers(count);
		VkResult const result = instance->EnumerateInstanceLayerProperties(&count, layers.data());
		if (result != VK_SUCCESS) {
			std::cerr << "vkEnumerateInstanceLayerProperties failed: " << FormatResult(result) << std::endl;
			return;
		}

		std::cout << "Vulkan Instance layers:" << std::endl;
		for (VkLayerProperties const & layer : layers) {
			std::cout << layer.layerName <<
				" | " << FormnatVersion(layer.specVersion) <<
				" | " << FormnatVersion(layer.implementationVersion) <<
				" | " << layer.description <<
				std::endl;
		}
		std::cout << std::endl;
	}

	void PrintExtensionsInformation(std::shared_ptr<RenderInstance> instance) {
		uint32_t count = 0;
		VkResult const countResult = instance->EnumerateInstanceExtensionProperties(nullptr, &count, nullptr);
		if (countResult != VK_SUCCESS) {
			std::cerr << "vkEnumerateInstanceExtensionProperties failed: " << FormatResult(countResult) << std::endl;
			return;
		}

		std::vector<VkExtensionProperties> extensions(count);
		VkResult const result = instance->EnumerateInstanceExtensionProperties(nullptr, &count, extensions.data());
		if (result != VK_SUCCESS) {
			std::cerr << "vkEnumerateInstanceExtensionProperties failed: " << FormatResult(result) << std::endl;
			return;
		}

		std::cout << "Vulkan Instance extensions:" << std::endl;
		for (VkExtensionProperties const & extension : extensions) {
			std::cout << extension.extensionName <<
				" | " << FormnatVersion(extension.specVersion) <<
				std::endl;
		}
		std::cout << std::endl;
	}

	void PrintDevicesInformation(std::shared_ptr<RenderInstance> instance) {
		uint32_t count = 0;
		VkResult const countResult = instance->EnumeratePhysicalDevices(instance->getInstance(), &count, nullptr);
		if (countResult != VK_SUCCESS) {
			std::cerr << "vkEnumeratePhysicalDevices failed: " << FormatResult(countResult) << std::endl;
			return;
		}

		std::vector<VkPhysicalDevice> devices(count);
		VkResult const result = instance->EnumeratePhysicalDevices(instance->getInstance(), &count, devices.data());
		if (result != VK_SUCCESS) {
			std::cerr << "vkEnumeratePhysicalDevices failed: " << FormatResult(result) << std::endl;
			return;
		}

		std::cout << "Vulkan physical devices:" << std::endl;
		for (VkPhysicalDevice const & device : devices) {
			VkPhysicalDeviceProperties properties = {};
			instance->GetPhysicalDeviceProperties(device, &properties);

			std::cout << properties.deviceName <<
				" | " << FormatDeviceType(properties.deviceType) <<
				" | " << FormnatVersion(properties.apiVersion) <<
				" | " << FormnatVersion(properties.driverVersion) <<
				std::endl;
		}
		std::cout << std::endl;
	}
}
