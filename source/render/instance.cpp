#include <iostream>

#include <config.h>
#include <render/instance.hpp>
#include <render/physical_device.hpp>

// runtime helper functions (source https://github.com/glfw/glfw)
#ifdef _WIN32
#include <windows.h>

#define _vkl_OpenLibrary(name)            LoadLibraryA(name)
#define _vkl_CloseLibrary(handle)         FreeLibrary((HMODULE) handle)
#define _vkl_GetProcAddress(handle, name) GetProcAddress((HMODULE) handle, name)

static char const * _vkl_LibraryName = "vulkan-1.dll";
#else
#include <dlfcn.h>

#define _vkl_OpenLibrary(name)            dlopen(name, RTLD_LAZY | RTLD_LOCAL)
#define _vkl_CloseLibrary(handle)         dlclose(handle)
#define _vkl_GetProcAddress(handle, name) dlsym(handle, name)

static char const * _vkl_LibraryName = "libvulkan.so.1";
#endif

#define VKL_GET_PROC_INSTANCE(__name) \
	instance->__name = (PFN_vk ## __name)instance->GetInstanceProcAddr(instance->m_handle, "vk" # __name); \
	if (!instance->__name) { \
		std::cerr << "vkGetInstanceProcAddr(vk " #__name ") failed" << std::endl; \
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
	// debug layers
	//"VK_LAYER_LUNARG_api_dump",
	//"VK_LAYER_RENDERDOC_Capture",
};

static char const * _vkl_required_extensions[] = {
	VK_KHR_SURFACE_EXTENSION_NAME,
#ifdef _WIN32
	VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
#else
	VK_KHR_XCB_SURFACE_EXTENSION_NAME,
#endif
	VK_EXT_DEBUG_REPORT_EXTENSION_NAME,
};

namespace vkl {
	void PrintLayersInformation(std::shared_ptr<instance_t> instance);
	void PrintExtensionsInformation(std::shared_ptr<instance_t> instance);

	instance_t::instance_t()
		: m_library(nullptr), m_handle(nullptr) {
	}

	instance_t::~instance_t() {
		if (DestroyInstance && m_handle) {
			DestroyInstance(m_handle, nullptr);
		}
		if (m_library) {
			_vkl_CloseLibrary(m_library);
		}
	}

	std::shared_ptr<instance_t> instance_t::create() {
		std::shared_ptr<instance_t> instance = std::make_shared<instance_t>();

		instance->m_library = _vkl_OpenLibrary(_vkl_LibraryName);
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
			config.application.name,
			VK_MAKE_VERSION(config.application.version, 0, 0),
			config.framework.name,
			VK_MAKE_VERSION(config.framework.version, 0, 0),
			VK_MAKE_VERSION(1, 0, 3),
		};

		VkInstanceCreateInfo const create_info = {
			VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
			nullptr,
			0,
			&applicationInfo,
			ARRAYSIZE(_vkl_required_layers),
			_vkl_required_layers,
			ARRAYSIZE(_vkl_required_extensions),
			_vkl_required_extensions,
		};

		VkResult const create_result = instance->CreateInstance(&create_info, nullptr, &instance->m_handle);
		if (create_result != VK_SUCCESS || !instance->m_handle) {
			std::cerr << "vkCreateInstance failed: " << format::result(create_result) << std::endl;
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
		VKL_GET_PROC_INSTANCE(GetPhysicalDeviceMemoryProperties);
		VKL_GET_PROC_INSTANCE(GetPhysicalDeviceQueueFamilyProperties);

		uint32_t count = 0;
		VkResult const count_result = instance->EnumeratePhysicalDevices(instance->handle(), &count, nullptr);
		if (count_result != VK_SUCCESS) {
			std::cerr << "vkEnumeratePhysicalDevices failed: " << format::result(count_result) << std::endl;
			return nullptr;
		}

		std::vector<VkPhysicalDevice> devices(count);
		VkResult const result = instance->EnumeratePhysicalDevices(instance->handle(), &count, devices.data());
		if (result != VK_SUCCESS) {
			std::cerr << "vkEnumeratePhysicalDevices failed: " << format::result(result) << std::endl;
			return nullptr;
		}

		instance->m_devices.reserve(count);
		for (VkPhysicalDevice & handle : devices) {
			instance->m_devices.emplace_back(physical_device_t::create(instance, handle));
		}

		return instance;
	}

	void PrintLayersInformation(std::shared_ptr<instance_t> instance) {
		uint32_t count = 0;
		VkResult const count_result = instance->EnumerateInstanceLayerProperties(&count, nullptr);
		if (count_result != VK_SUCCESS) {
			std::cerr << "vkEnumerateInstanceLayerProperties failed: " << format::result(count_result) << std::endl;
			return;
		}

		std::vector<VkLayerProperties> layers(count);
		VkResult const result = instance->EnumerateInstanceLayerProperties(&count, layers.data());
		if (result != VK_SUCCESS) {
			std::cerr << "vkEnumerateInstanceLayerProperties failed: " << format::result(result) << std::endl;
			return;
		}

		std::cout << "Vulkan Instance layers:" << std::endl;
		for (VkLayerProperties const & layer : layers) {
			std::cout << layer.layerName <<
				" | " << format::version(layer.specVersion) <<
				" | " << format::version(layer.implementationVersion) <<
				" | " << layer.description <<
				std::endl;
		}
		std::cout << std::endl;
	}

	void PrintExtensionsInformation(std::shared_ptr<instance_t> instance) {
		uint32_t count = 0;
		VkResult const count_result = instance->EnumerateInstanceExtensionProperties(nullptr, &count, nullptr);
		if (count_result != VK_SUCCESS) {
			std::cerr << "vkEnumerateInstanceExtensionProperties failed: " << format::result(count_result) << std::endl;
			return;
		}

		std::vector<VkExtensionProperties> extensions(count);
		VkResult const result = instance->EnumerateInstanceExtensionProperties(nullptr, &count, extensions.data());
		if (result != VK_SUCCESS) {
			std::cerr << "vkEnumerateInstanceExtensionProperties failed: " << format::result(result) << std::endl;
			return;
		}

		std::cout << "Vulkan Instance extensions:" << std::endl;
		for (VkExtensionProperties const & extension : extensions) {
			std::cout << extension.extensionName <<
				" | " << format::version(extension.specVersion) <<
				std::endl;
		}
		std::cout << std::endl;
	}
}
