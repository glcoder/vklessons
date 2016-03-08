#pragma once

#include <string>

#ifdef _WIN32
#define VK_USE_PLATFORM_WIN32_KHR
#else
#define VK_USE_PLATFORM_XCB_KHR
#endif

#define VK_NO_PROTOTYPES
#include <vulkan/vulkan.h>

#include <config.h>

#ifndef ARRAYSIZE
#define ARRAYSIZE(__a) (sizeof(__a)/sizeof((__a)[0]))
#endif

namespace vkl {
	namespace format {
		std::string const version(uint32_t version);
		std::string const result(VkResult result);
		std::string const device_type(VkPhysicalDeviceType type);
		std::string const memory_flags(VkMemoryPropertyFlags flags);
		std::string const heap_flags(VkMemoryHeapFlags flags);
		std::string const heap_size(uint64_t size);
		std::string const queue_flags(VkQueueFlags flags);
		std::string const extent(VkExtent3D extent);
	}
}
