#include <render/common.hpp>

#define VKL_ERROR_HELPER(__err) case __err: return #__err
#define VKL_ENUM_HELPER(__pref,__name) case __pref ## _ ## __name: return #__name
#define VKL_FLAGS_HELPER(__result, __flags, __pref, __name) if (__flags & (__pref ## _ ## __name)) __result = __result + #__name + " "

namespace vkl {
	namespace format {
		std::string const version(uint32_t version) {
			return
				std::to_string(VK_VERSION_MAJOR(version)) + "." +
				std::to_string(VK_VERSION_MINOR(version)) + "." +
				std::to_string(VK_VERSION_PATCH(version));
		}

		std::string const result(VkResult result) {
			switch (result) {
				VKL_ERROR_HELPER(VK_SUCCESS);
				VKL_ERROR_HELPER(VK_NOT_READY);
				VKL_ERROR_HELPER(VK_TIMEOUT);
				VKL_ERROR_HELPER(VK_EVENT_SET);
				VKL_ERROR_HELPER(VK_EVENT_RESET);
				VKL_ERROR_HELPER(VK_INCOMPLETE);
				VKL_ERROR_HELPER(VK_ERROR_OUT_OF_HOST_MEMORY);
				VKL_ERROR_HELPER(VK_ERROR_OUT_OF_DEVICE_MEMORY);
				VKL_ERROR_HELPER(VK_ERROR_INITIALIZATION_FAILED);
				VKL_ERROR_HELPER(VK_ERROR_DEVICE_LOST);
				VKL_ERROR_HELPER(VK_ERROR_MEMORY_MAP_FAILED);
				VKL_ERROR_HELPER(VK_ERROR_LAYER_NOT_PRESENT);
				VKL_ERROR_HELPER(VK_ERROR_EXTENSION_NOT_PRESENT);
				VKL_ERROR_HELPER(VK_ERROR_FEATURE_NOT_PRESENT);
				VKL_ERROR_HELPER(VK_ERROR_INCOMPATIBLE_DRIVER);
				VKL_ERROR_HELPER(VK_ERROR_TOO_MANY_OBJECTS);
				VKL_ERROR_HELPER(VK_ERROR_FORMAT_NOT_SUPPORTED);
				VKL_ERROR_HELPER(VK_ERROR_SURFACE_LOST_KHR);
				VKL_ERROR_HELPER(VK_ERROR_NATIVE_WINDOW_IN_USE_KHR);
				VKL_ERROR_HELPER(VK_SUBOPTIMAL_KHR);
				VKL_ERROR_HELPER(VK_ERROR_OUT_OF_DATE_KHR);
				VKL_ERROR_HELPER(VK_ERROR_INCOMPATIBLE_DISPLAY_KHR);
				VKL_ERROR_HELPER(VK_ERROR_VALIDATION_FAILED_EXT);
			}
			return "UNKNOWN_RESULT";
		}

		std::string const device_type(VkPhysicalDeviceType type) {
			switch (type) {
				VKL_ENUM_HELPER(VK_PHYSICAL_DEVICE_TYPE, OTHER);
				VKL_ENUM_HELPER(VK_PHYSICAL_DEVICE_TYPE, INTEGRATED_GPU);
				VKL_ENUM_HELPER(VK_PHYSICAL_DEVICE_TYPE, DISCRETE_GPU);
				VKL_ENUM_HELPER(VK_PHYSICAL_DEVICE_TYPE, VIRTUAL_GPU);
				VKL_ENUM_HELPER(VK_PHYSICAL_DEVICE_TYPE, CPU);
			}
			return "UNKNOWN_DEVICE";
		}

		std::string const memory_flags(VkMemoryPropertyFlags flags) {
			std::string result;
			VKL_FLAGS_HELPER(result, flags, VK_MEMORY_PROPERTY, DEVICE_LOCAL_BIT);
			VKL_FLAGS_HELPER(result, flags, VK_MEMORY_PROPERTY, HOST_VISIBLE_BIT);
			VKL_FLAGS_HELPER(result, flags, VK_MEMORY_PROPERTY, HOST_COHERENT_BIT);
			VKL_FLAGS_HELPER(result, flags, VK_MEMORY_PROPERTY, HOST_CACHED_BIT);
			VKL_FLAGS_HELPER(result, flags, VK_MEMORY_PROPERTY, LAZILY_ALLOCATED_BIT);
			return result;
		}

		std::string const heap_flags(VkMemoryHeapFlags flags) {
			std::string result;
			VKL_FLAGS_HELPER(result, flags, VK_MEMORY_HEAP, DEVICE_LOCAL_BIT);
			return result;
		}

		std::string const heap_size(uint64_t size) {
			return std::to_string((uint64_t)(size / 1024.0 / 1024.0)) + " MB";
		}

		std::string const queue_flags(VkQueueFlags flags) {
			std::string result;
			VKL_FLAGS_HELPER(result, flags, VK_QUEUE, GRAPHICS_BIT);
			VKL_FLAGS_HELPER(result, flags, VK_QUEUE, COMPUTE_BIT);
			VKL_FLAGS_HELPER(result, flags, VK_QUEUE, TRANSFER_BIT);
			VKL_FLAGS_HELPER(result, flags, VK_QUEUE, SPARSE_BINDING_BIT);
			return result;
		}

		std::string const extent(VkExtent3D extent) {
			return "{" +
				std::to_string(extent.width) + ", " +
				std::to_string(extent.height) + ", " +
				std::to_string(extent.depth) + "}";
		}
	}
}
