#pragma once

#include <memory>
#include <vector>

#include <render/Common.hpp>

namespace vkl {
	typedef void * LibraryHandle;

	class Render {
	public:
		static std::shared_ptr<Render> Create();

		Render();
		~Render();

		std::vector<VkLayerProperties> const & getLayers() const {
			return m_layers;
		}

		std::vector<VkExtensionProperties> const & getExtensions() const {
			return m_extensions;
		}

		PFN_vkGetInstanceProcAddr                  GetInstanceProcAddr                  = nullptr;
		PFN_vkCreateInstance                       CreateInstance                       = nullptr;
		PFN_vkEnumerateInstanceLayerProperties     EnumerateInstanceLayerProperties     = nullptr;
		PFN_vkEnumerateInstanceExtensionProperties EnumerateInstanceExtensionProperties = nullptr;

	private:
		LibraryHandle m_library;

		std::vector<VkLayerProperties>     m_layers;
		std::vector<VkExtensionProperties> m_extensions;
	};
}
