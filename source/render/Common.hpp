#pragma once

#include <string>

#ifdef _WIN32
#define VK_USE_PLATFORM_WIN32_KHR
#else
#define VK_USE_PLATFORM_XCB_KHR
#endif

#define VK_NO_PROTOTYPES
#include <vulkan/vulkan.h>

#include <Config.h>

namespace vkl {
	std::string const FormnatVersion(uint32_t version);
	std::string const FormatResult(VkResult result);
	std::string const FormatDeviceType(VkPhysicalDeviceType type);
}
