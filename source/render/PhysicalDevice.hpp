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

		std::shared_ptr<RenderInstance> getRenderInstance() const {
			return m_renderInstance;
		}

		VkPhysicalDevice getDevice() const {
			return m_device;
		}

		VkPhysicalDeviceFeatures const & getFeatures() const {
			return m_features;
		}

		VkPhysicalDeviceProperties const & getProperties() const {
			return m_properties;
		}

		std::vector<VkLayerProperties> const & getLayers() const {
			return m_layers;
		}

		std::vector<VkExtensionProperties> const & getExtensions() const {
			return m_extensions;
		}

		PFN_vkGetDeviceProcAddr                  GetDeviceProcAddr                  = nullptr;
		PFN_vkGetPhysicalDeviceFeatures          GetPhysicalDeviceFeatures          = nullptr;
		PFN_vkGetPhysicalDeviceProperties        GetPhysicalDeviceProperties        = nullptr;
		PFN_vkEnumerateDeviceLayerProperties     EnumerateDeviceLayerProperties     = nullptr;
		PFN_vkEnumerateDeviceExtensionProperties EnumerateDeviceExtensionProperties = nullptr;
		PFN_vkCreateDevice                       CreateDevice                       = nullptr;

	private:
		std::shared_ptr<RenderInstance> m_renderInstance;
		VkPhysicalDevice                m_device;
		VkPhysicalDeviceFeatures        m_features;
		VkPhysicalDeviceProperties      m_properties;

		std::vector<VkLayerProperties>     m_layers;
		std::vector<VkExtensionProperties> m_extensions;
	};
}
