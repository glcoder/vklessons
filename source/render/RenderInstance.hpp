#pragma once

#include <memory>
#include <vector>

#include <render/Common.hpp>

namespace vkl {
	class Render;
	class PhysicalDevice;

	class RenderInstance {
	public:
		static std::shared_ptr<RenderInstance> Create(std::shared_ptr<Render> render);

		RenderInstance(std::shared_ptr<Render> render);
		~RenderInstance();

		std::shared_ptr<Render> getRender() const {
			return m_render;
		}

		VkInstance getInstance() const {
			return m_instance;
		}

		std::vector<std::shared_ptr<PhysicalDevice>> const & getDevices() const {
			return m_devices;
		}

		PFN_vkDestroyInstance          DestroyInstance          = nullptr;
		PFN_vkEnumeratePhysicalDevices EnumeratePhysicalDevices = nullptr;

	private:
		std::shared_ptr<Render> m_render;
		VkInstance              m_instance;

		std::vector<std::shared_ptr<PhysicalDevice>> m_devices;
	};
}
