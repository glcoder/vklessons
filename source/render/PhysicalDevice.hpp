#pragma once

#include <memory>
#include <vector>

#include <render/Common.hpp>

namespace vkl {
	class RenderInstance;
	class RenderDevice;

	class PhysicalDevice {
	public:
		static std::shared_ptr<PhysicalDevice> Create(std::shared_ptr<RenderInstance> renderInstance, VkPhysicalDevice device);

		PhysicalDevice(std::shared_ptr<RenderInstance> renderInstance, VkPhysicalDevice device);
		~PhysicalDevice();

		std::shared_ptr<RenderInstance> getInstance() const {
			return m_instance;
		}

		VkPhysicalDevice getDeivce() const {
			return m_device;
		}

		VkPhysicalDeviceFeatures const & getFeatures() const {
			return m_features;
		}

		VkPhysicalDeviceProperties const & getProperties() const {
			return m_properties;
		}

		bool isDescrete() const {
			return m_properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU;
		}

	private:
		std::shared_ptr<RenderInstance> m_instance;
		VkPhysicalDevice                m_device;
		VkPhysicalDeviceFeatures        m_features;
		VkPhysicalDeviceProperties      m_properties;
	};
}
