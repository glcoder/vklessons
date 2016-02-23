#include <iostream>
#include <algorithm>

#include <render/Render.hpp>

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

namespace vkl {
	Render::Render()
		: m_library(nullptr) {
	}

	Render::~Render() {
		if (m_library) {
			_vkl_CloseLibrary(m_library);
		}
	}

	std::shared_ptr<Render> Render::Create() {
		std::shared_ptr<Render> instance = std::make_shared<Render>();
		instance->m_library = (LibraryHandle)_vkl_OpenLibrary(_vkl_LibraryName);
		if (!instance->m_library) {
			std::cerr << "Failed to open library " << _vkl_LibraryName << std::endl;
			return nullptr;
		}

		instance->GetInstanceProcAddr = (PFN_vkGetInstanceProcAddr)_vkl_GetProcAddress(instance->m_library, "vkGetInstanceProcAddr");
		if (!instance->GetInstanceProcAddr) {
			std::cerr << "Failed to get vkGetInstanceProcAddr address" << std::endl;
			return nullptr;
		}

		VKL_GET_PROC_INSTANCE(instance, nullptr, CreateInstance);
		VKL_GET_PROC_INSTANCE(instance, nullptr, EnumerateInstanceLayerProperties);
		VKL_GET_PROC_INSTANCE(instance, nullptr, EnumerateInstanceExtensionProperties);

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
			std::cout << l.layerName <<
				" " << VKL_FORMAT_VERSION(l.specVersion) <<
				" " << VKL_FORMAT_VERSION(l.implementationVersion) <<
				" " << l.description <<
				std::endl;
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
			std::cout << e.extensionName << " " << VKL_FORMAT_VERSION(e.specVersion) << std::endl;
		});
		std::cout << std::endl;

		return instance;
	}
}
