#pragma once

#include <memory>

#include <render/common.hpp>

namespace vkl {
	class physical_device_t;

	class device_t {
	public:
		static std::shared_ptr<device_t> create(std::shared_ptr<physical_device_t> physical_device);

		device_t(std::shared_ptr<physical_device_t> physical_device);
		~device_t();

		std::shared_ptr<physical_device_t> physical_device() const {
			return m_physical_device;
		}

		VkDevice handle() const {
			return m_handle;
		}

		// get from GetDeviceProcAddr with not null device
		PFN_vkDestroyDevice DestroyDevice = nullptr;
		PFN_vkDeviceWaitIdle DeviceWaitIdle = nullptr;

	private:
		std::shared_ptr<physical_device_t> m_physical_device = nullptr;
		VkDevice                           m_handle          = nullptr;
	};
}
