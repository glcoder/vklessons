#pragma once

#ifdef _WIN32
#define VK_USE_PLATFORM_WIN32_KHR
#else
#define VK_USE_PLATFORM_XCB_KHR
#endif

#define VK_NO_PROTOTYPES
#include <vulkan/vulkan.h>
#include <string>

#include <Config.h>

#define VKL_GET_PROC_INSTANCE(src, inst, name) \
	instance->name = (PFN_vk ## name)src->GetInstanceProcAddr(inst, "vk" # name); \
	if (!instance->name) { \
		std::cerr << "Failed to get vk " #name " address" << std::endl; \
		return nullptr; \
	}

#define VKL_GET_PROC_DEVICE(src, dev, name) \
	instance->name = (PFN_vk ## name)src->GetDeviceProcAddr(dev, "vk" # name); \
	if (!instance->name) { \
		std::cerr << "Failed to get vk " #name " address" << std::endl; \
		return nullptr; \
	}

#define VKL_FORMAT_VERSION(version) \
	std::to_string(VK_VERSION_MAJOR(version)) + "." + \
	std::to_string(VK_VERSION_MINOR(version)) + "." + \
	std::to_string(VK_VERSION_PATCH(version))
