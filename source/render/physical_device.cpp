#include <iostream>

#include <render/instance.hpp>
#include <render/physical_device.hpp>

namespace vkl {
	void PrintDeviceInformation(std::shared_ptr<physical_device_t> device);
	void PrintLayersInformation(std::shared_ptr<physical_device_t> device);
	void PrintExtensionsInformation(std::shared_ptr<physical_device_t> device);
	void PrintMemoryInformation(std::shared_ptr<physical_device_t> device);
	void PrintQueueFamilyInformation(std::shared_ptr<physical_device_t> device);

	physical_device_t::physical_device_t(std::shared_ptr<instance_t> instance, VkPhysicalDevice handle)
		: m_instance(instance), m_handle(handle) {
	}

	physical_device_t::~physical_device_t() {
	}

	std::shared_ptr<physical_device_t> physical_device_t::create(std::shared_ptr<instance_t> instance, VkPhysicalDevice handle) {
		std::shared_ptr<physical_device_t> device = std::make_shared<physical_device_t>(instance, handle);
		
		instance->GetPhysicalDeviceFeatures(handle, &device->m_features);
		instance->GetPhysicalDeviceProperties(handle, &device->m_properties);
		instance->GetPhysicalDeviceMemoryProperties(handle, &device->m_memory);

		uint32_t count = 0;
		instance->GetPhysicalDeviceQueueFamilyProperties(device->handle(), &count, nullptr);

		device->m_queues.resize(count);
		instance->GetPhysicalDeviceQueueFamilyProperties(device->handle(), &count, device->m_queues.data());

		PrintDeviceInformation(device);
		PrintLayersInformation(device);
		PrintExtensionsInformation(device);
		PrintMemoryInformation(device);
		PrintQueueFamilyInformation(device);

		return device;
	}

	uint32_t physical_device_t::memory_index(VkMemoryPropertyFlags flags) {
		for (uint32_t i = 0; i < m_memory.memoryTypeCount; ++i) {
			VkMemoryType const &type = m_memory.memoryTypes[i];
			if ((type.propertyFlags & flags) == flags)
				return type.heapIndex;
		}
		return VK_MAX_MEMORY_TYPES;
	}

	uint32_t physical_device_t::queue_index(VkQueueFlags flags) {
		for (uint32_t i = 0; i < m_queues.size(); ++i) {
			VkQueueFamilyProperties const & queue = m_queues[i];
			if ((queue.queueFlags & flags) == flags)
				return i;
		}
		return (uint32_t)m_queues.size();
	}

	void PrintDeviceInformation(std::shared_ptr<physical_device_t> device) {
		std::cout << device->properties().deviceName <<
			" | " << format::device_type(device->properties().deviceType) <<
			" | " << format::version(device->properties().apiVersion) <<
			" | " << format::version(device->properties().driverVersion) <<
			std::endl << std::endl;
	}

	void PrintLayersInformation(std::shared_ptr<physical_device_t> device) {
		uint32_t count = 0;
		VkResult const count_result = device->instance()->EnumerateDeviceLayerProperties(device->handle(), &count, nullptr);
		if (count_result != VK_SUCCESS) {
			std::cerr << "vkEnumerateDeviceLayerProperties failed: " << format::result(count_result) << std::endl;
			return;
		}

		std::vector<VkLayerProperties> layers(count);
		VkResult const result = device->instance()->EnumerateDeviceLayerProperties(device->handle(), &count, layers.data());
		if (result != VK_SUCCESS) {
			std::cerr << "vkEnumerateDeviceLayerProperties failed: " << format::result(result) << std::endl;
			return;
		}

		std::cout << "Vulkan Device layers:" << std::endl;
		for (VkLayerProperties const & layer : layers) {
			std::cout << layer.layerName <<
				" | " << format::version(layer.specVersion) <<
				" | " << format::version(layer.implementationVersion) <<
				" | " << layer.description <<
				std::endl;
		}
		std::cout << std::endl;
	}

	void PrintExtensionsInformation(std::shared_ptr<physical_device_t> device) {
		uint32_t count = 0;
		VkResult const count_result = device->instance()->EnumerateDeviceExtensionProperties(device->handle(), nullptr, &count, nullptr);
		if (count_result != VK_SUCCESS) {
			std::cerr << "vkEnumerateDeviceExtensionProperties failed: " << format::result(count_result) << std::endl;
			return;
		}

		std::vector<VkExtensionProperties> extensions(count);
		VkResult const result = device->instance()->EnumerateDeviceExtensionProperties(device->handle(), nullptr, &count, extensions.data());
		if (result != VK_SUCCESS) {
			std::cerr << "vkEnumerateDeviceExtensionProperties failed: " << format::result(result) << std::endl;
			return;
		}

		std::cout << "Vulkan Device extensions:" << std::endl;
		for (VkExtensionProperties const & extension : extensions) {
			std::cout << extension.extensionName <<
				" | " << format::version(extension.specVersion) <<
				std::endl;
		}
		std::cout << std::endl;
	}

	void PrintMemoryInformation(std::shared_ptr<physical_device_t> device) {
		std::cout << "Vulkan Device memory types:" << std::endl;
		for (uint32_t i = 0; i < device->memory().memoryTypeCount; ++i) {
			VkMemoryType const &type = device->memory().memoryTypes[i];
			std::cout << "type " << i <<
				" | heap " << type.heapIndex <<
				" | " << format::memory_flags(type.propertyFlags) <<
				std::endl;
		}
		std::cout << std::endl;

		std::cout << "Vulkan Device memory heaps:" << std::endl;
		for (uint32_t i = 0; i < device->memory().memoryHeapCount; ++i) {
			VkMemoryHeap const &heap = device->memory().memoryHeaps[i];
			std::cout << "heap " << i <<
				" | size " << format::heap_size(heap.size) <<
				" | " << format::heap_flags(heap.flags) <<
				std::endl;
		}
		std::cout << std::endl;
	}

	void PrintQueueFamilyInformation(std::shared_ptr<physical_device_t> device) {
		std::cout << "Vulkan Device queue families:" << std::endl;
		for (uint32_t i = 0; i < device->queues().size(); ++i) {
			VkQueueFamilyProperties const & queue = device->queues().at(i);
			VkExtent3D const & extent = queue.minImageTransferGranularity;
			std::cout << "queue " << i <<
				" | count " << queue.queueCount <<
				" | " << format::queue_flags(queue.queueFlags) <<
				" | " << queue.timestampValidBits <<
				" | " << format::extent(queue.minImageTransferGranularity) <<
				std::endl;
		}
		std::cout << std::endl;
	}
}
