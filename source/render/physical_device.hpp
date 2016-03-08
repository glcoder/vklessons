#pragma once

#include <memory>
#include <vector>

#include <render/common.hpp>

namespace vkl {
	class instance_t;
	class device_t;

	class physical_device_t {
	public:
		static std::shared_ptr<physical_device_t> create(std::shared_ptr<instance_t> instance, VkPhysicalDevice handle);

		physical_device_t(std::shared_ptr<instance_t> instance, VkPhysicalDevice handle);
		~physical_device_t();

		std::shared_ptr<instance_t> instance() const {
			return m_instance;
		}

		VkPhysicalDevice handle() const {
			return m_handle;
		}

		VkPhysicalDeviceFeatures const & features() const {
			return m_features;
		}

		VkPhysicalDeviceProperties const & properties() const {
			return m_properties;
		}

		VkPhysicalDeviceMemoryProperties const & memory() const {
			return m_memory;
		}

		std::vector<VkQueueFamilyProperties> const & queues() const {
			return m_queues;
		}

		std::vector<std::shared_ptr<device_t>> const & devices() const {
			return m_devices;
		}

		uint32_t memory_index(VkMemoryPropertyFlags flags);

		uint32_t queue_index(VkQueueFlags flags);

	private:
		std::shared_ptr<instance_t>      m_instance   = nullptr;
		VkPhysicalDevice                 m_handle     = nullptr;
		VkPhysicalDeviceFeatures         m_features   = {};
		VkPhysicalDeviceProperties       m_properties = {};
		VkPhysicalDeviceMemoryProperties m_memory     = {};

		std::vector<VkQueueFamilyProperties>   m_queues;
		std::vector<std::shared_ptr<device_t>> m_devices;
	};
}
