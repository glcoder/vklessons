#pragma once

#ifdef _WIN32
#define VK_USE_PLATFORM_WIN32_KHR
#else
#define VK_USE_PLATFORM_XCB_KHR
#endif

#define VK_NO_PROTOTYPES
#include <vulkan/vulkan.h>

#include <memory>
#include <vector>

namespace vkl {
	typedef void * LibraryHandle;

	class RenderInstance {
	public:
		static std::shared_ptr<RenderInstance> Create();

		RenderInstance();
		~RenderInstance();

		PFN_vkGetInstanceProcAddr                  GetInstanceProcAddr                  = nullptr;
		PFN_vkCreateInstance                       CreateInstance                       = nullptr;
		PFN_vkDestroyInstance                      DestroyInstance                      = nullptr;
		PFN_vkEnumerateInstanceLayerProperties     EnumerateInstanceLayerProperties     = nullptr;
		PFN_vkEnumerateInstanceExtensionProperties EnumerateInstanceExtensionProperties = nullptr;

	private:
		LibraryHandle m_library;
		VkInstance    m_instance;

		std::vector<VkLayerProperties>     m_layers;
		std::vector<VkExtensionProperties> m_extensions;
	};
}
