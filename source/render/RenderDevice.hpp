#pragma once

#include <memory>

#include <render/Common.hpp>

namespace vkl {
	class RenderInstance;

	class RenderDevice {
	public:
		static std::shared_ptr<RenderDevice> Create(std::shared_ptr<RenderInstance> renderInstance, VkPhysicalDevice physicalDevice);

		RenderDevice(std::shared_ptr<RenderInstance> renderInstance, VkPhysicalDevice physicalDevice);
		~RenderDevice();

		std::shared_ptr<RenderInstance> getInstance() const {
			return m_instance;
		}

		VkPhysicalDevice getPhysicalDevice() const {
			return m_physicalDevice;
		}

		VkDevice getDevice() const {
			return m_device;
		}

		// get from GetDeviceProcAddr with not null device
		PFN_vkDestroyDevice  DestroyDevice  = nullptr;
		PFN_vkDeviceWaitIdle DeviceWaitIdle = nullptr;

	private:
		std::shared_ptr<RenderInstance> m_instance;
		VkPhysicalDevice                m_physicalDevice;
		VkDevice                        m_device;
	};
}
