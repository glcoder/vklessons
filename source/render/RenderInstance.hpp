#pragma once

#include <memory>
#include <vector>

#include <render/Common.hpp>

namespace vkl {
	typedef void * LibraryHandle;

	class RenderInstance {
	public:
		static std::shared_ptr<RenderInstance> Create();

		RenderInstance();
		~RenderInstance();

		VkInstance getInstance() const {
			return m_instance;
		}

		std::vector<VkPhysicalDevice> const & getDevices() const {
			return m_devices;
		}

		// get from dynamic library
		PFN_vkGetInstanceProcAddr                  GetInstanceProcAddr                  = nullptr;

		// get from GetInstanceProcAddr with null instance
		PFN_vkCreateInstance                       CreateInstance                       = nullptr;
		PFN_vkEnumerateInstanceLayerProperties     EnumerateInstanceLayerProperties     = nullptr;
		PFN_vkEnumerateInstanceExtensionProperties EnumerateInstanceExtensionProperties = nullptr;

		// get from GetInstanceProcAddr with not null instance
		PFN_vkDestroyInstance                      DestroyInstance                      = nullptr;
		PFN_vkEnumeratePhysicalDevices             EnumeratePhysicalDevices             = nullptr;
		PFN_vkGetPhysicalDeviceFeatures            GetPhysicalDeviceFeatures            = nullptr;
		PFN_vkGetPhysicalDeviceProperties          GetPhysicalDeviceProperties          = nullptr;
		PFN_vkEnumerateDeviceLayerProperties       EnumerateDeviceLayerProperties       = nullptr;
		PFN_vkEnumerateDeviceExtensionProperties   EnumerateDeviceExtensionProperties   = nullptr;
		PFN_vkGetDeviceProcAddr                    GetDeviceProcAddr                    = nullptr;
		PFN_vkCreateDevice                         CreateDevice                         = nullptr;

	private:
		LibraryHandle m_library;
		VkInstance    m_instance;

		std::vector<VkPhysicalDevice> m_devices;
	};
}
