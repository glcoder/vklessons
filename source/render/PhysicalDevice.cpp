#include <iostream>
#include <algorithm>

#include <render/Render.hpp>
#include <render/RenderInstance.hpp>
#include <render/PhysicalDevice.hpp>

namespace vkl {
	PhysicalDevice::PhysicalDevice(std::shared_ptr<RenderInstance> renderInstance, VkPhysicalDevice device)
		: m_renderInstance(renderInstance), m_device(device), m_features(), m_properties() {
	}

	PhysicalDevice::~PhysicalDevice(){
	}

	std::shared_ptr<PhysicalDevice> PhysicalDevice::Create(std::shared_ptr<RenderInstance> renderInstance, VkPhysicalDevice device) {
		std::shared_ptr<PhysicalDevice> instance = std::make_shared<PhysicalDevice>(renderInstance, device);

		VKL_GET_PROC_INSTANCE(renderInstance->getRender(), renderInstance->getInstance(), GetDeviceProcAddr);
		VKL_GET_PROC_INSTANCE(renderInstance->getRender(), renderInstance->getInstance(), GetPhysicalDeviceFeatures);
		VKL_GET_PROC_INSTANCE(renderInstance->getRender(), renderInstance->getInstance(), GetPhysicalDeviceProperties);
		VKL_GET_PROC_INSTANCE(renderInstance->getRender(), renderInstance->getInstance(), EnumerateDeviceLayerProperties);
		VKL_GET_PROC_INSTANCE(renderInstance->getRender(), renderInstance->getInstance(), EnumerateDeviceExtensionProperties);
		VKL_GET_PROC_INSTANCE(renderInstance->getRender(), renderInstance->getInstance(), CreateDevice);

		instance->GetPhysicalDeviceFeatures(device, &instance->m_features);
		instance->GetPhysicalDeviceProperties(device, &instance->m_properties);

		std::cout << "Device: " << instance->m_properties.deviceName << std::endl << std::endl;

		uint32_t layersCount = 0;
		VkResult const layersCountResult = instance->EnumerateDeviceLayerProperties(device, &layersCount, nullptr);
		if (layersCountResult != VK_SUCCESS) {
			std::cerr << "Failed to call vkEnumerateDeviceLayerProperties: " << layersCountResult << std::endl;
			return nullptr;
		}

		instance->m_layers.resize(layersCount);

		VkResult const layersResult = instance->EnumerateDeviceLayerProperties(device, &layersCount, instance->m_layers.data());
		if (layersResult != VK_SUCCESS) {
			std::cerr << "Failed to call vkEnumerateDeviceLayerProperties: " << layersResult << std::endl;
			return nullptr;
		}

		std::cout << "Available Device layers:" << std::endl;
		std::for_each(std::begin(instance->m_layers), std::end(instance->m_layers), [](VkLayerProperties const & l){
			std::cout << l.layerName <<
				" " << VKL_FORMAT_VERSION(l.specVersion) <<
				" " << VKL_FORMAT_VERSION(l.implementationVersion) <<
				" " << l.description <<
				std::endl;
			
		});
		std::cout << std::endl;

		uint32_t extensionsCount = 0;
		VkResult const extensionsCountResult = instance->EnumerateDeviceExtensionProperties(device, nullptr, &extensionsCount, nullptr);
		if (extensionsCountResult != VK_SUCCESS) {
			std::cerr << "Failed to call vkEnumerateDeviceExtensionProperties: " << extensionsCountResult << std::endl;
			return nullptr;
		}

		instance->m_extensions.resize(extensionsCount);

		VkResult const extensionsResult = instance->EnumerateDeviceExtensionProperties(device, nullptr, &extensionsCount, instance->m_extensions.data());
		if (extensionsResult != VK_SUCCESS) {
			std::cerr << "Failed to call vkEnumerateDeviceExtensionProperties: " << extensionsResult << std::endl;
			return nullptr;
		}

		std::cout << "Available Device extensions:" << std::endl;
		std::for_each(std::begin(instance->m_extensions), std::end(instance->m_extensions), [](VkExtensionProperties const & e){
			std::cout << e.extensionName << " " << VKL_FORMAT_VERSION(e.specVersion) << std::endl;
		});
		std::cout << std::endl;

		return instance;
	}
}
