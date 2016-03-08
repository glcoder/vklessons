#pragma once

#include <memory>
#include <vector>

#include <render/common.hpp>

namespace vkl {
	typedef void * vulkan_library_t;

	class physical_device_t;

	class instance_t {
	public:
		static std::shared_ptr<instance_t> create();

		instance_t();
		~instance_t();

		VkInstance handle() const {
			return m_handle;
		}

		std::vector<std::shared_ptr<physical_device_t>> const & devices() const {
			return m_devices;
		}

		// get from dynamic library
		PFN_vkGetInstanceProcAddr GetInstanceProcAddr = nullptr;

		// get from GetInstanceProcAddr with null instance
		PFN_vkCreateInstance CreateInstance = nullptr;
		PFN_vkEnumerateInstanceLayerProperties EnumerateInstanceLayerProperties = nullptr;
		PFN_vkEnumerateInstanceExtensionProperties EnumerateInstanceExtensionProperties = nullptr;

		// get from GetInstanceProcAddr with not null instance
		PFN_vkDestroyInstance DestroyInstance = nullptr;
		PFN_vkEnumeratePhysicalDevices EnumeratePhysicalDevices = nullptr;
		PFN_vkGetPhysicalDeviceFeatures GetPhysicalDeviceFeatures = nullptr;
		PFN_vkGetPhysicalDeviceProperties GetPhysicalDeviceProperties = nullptr;
		PFN_vkEnumerateDeviceLayerProperties EnumerateDeviceLayerProperties = nullptr;
		PFN_vkEnumerateDeviceExtensionProperties EnumerateDeviceExtensionProperties = nullptr;
		PFN_vkGetDeviceProcAddr GetDeviceProcAddr = nullptr;
		PFN_vkCreateDevice CreateDevice = nullptr;
		PFN_vkGetPhysicalDeviceMemoryProperties GetPhysicalDeviceMemoryProperties = nullptr;
		PFN_vkGetPhysicalDeviceQueueFamilyProperties GetPhysicalDeviceQueueFamilyProperties = nullptr;

	private:
		vulkan_library_t m_library = nullptr;
		VkInstance       m_handle  = nullptr;
		std::vector<std::shared_ptr<physical_device_t>> m_devices;
	};
}
