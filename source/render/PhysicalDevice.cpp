#include <iostream>

#include <Config.h>
#include <render/RenderInstance.hpp>
#include <render/PhysicalDevice.hpp>
#include <render/RenderDevice.hpp>

namespace vkl {
	void PrintDeviceInformation(VkPhysicalDeviceProperties const & properties);
	void PrintLayersInformation(std::shared_ptr<RenderInstance> instance, VkPhysicalDevice device);
	void PrintExtensionsInformation(std::shared_ptr<RenderInstance> instance, VkPhysicalDevice device);

	PhysicalDevice::PhysicalDevice(std::shared_ptr<RenderInstance> instance, VkPhysicalDevice device)
		: m_instance(instance), m_device(device), m_features(), m_properties() {

	}

	PhysicalDevice::~PhysicalDevice() {
	}

	std::shared_ptr<PhysicalDevice> PhysicalDevice::Create(std::shared_ptr<RenderInstance> instance, VkPhysicalDevice device) {
		std::shared_ptr<PhysicalDevice> physicalDevice = std::make_shared<PhysicalDevice>(instance, device);
		
		instance->GetPhysicalDeviceFeatures(device, &physicalDevice->m_features);
		instance->GetPhysicalDeviceProperties(device, &physicalDevice->m_properties);

		PrintDeviceInformation(physicalDevice->m_properties);
		PrintLayersInformation(instance, device);
		PrintExtensionsInformation(instance, device);

		return physicalDevice;
	}

	void PrintDeviceInformation(VkPhysicalDeviceProperties const & properties) {
		std::cout << properties.deviceName <<
			" | " << FormatDeviceType(properties.deviceType) <<
			" | " << FormnatVersion(properties.apiVersion) <<
			" | " << FormnatVersion(properties.driverVersion) <<
			std::endl << std::endl;
	}

	void PrintLayersInformation(std::shared_ptr<RenderInstance> instance, VkPhysicalDevice device) {
		uint32_t count = 0;
		VkResult const countResult = instance->EnumerateDeviceLayerProperties(device, &count, nullptr);
		if (countResult != VK_SUCCESS) {
			std::cerr << "vkEnumerateDeviceLayerProperties failed: " << FormatResult(countResult) << std::endl;
			return;
		}

		std::vector<VkLayerProperties> layers(count);
		VkResult const result = instance->EnumerateDeviceLayerProperties(device, &count, layers.data());
		if (result != VK_SUCCESS) {
			std::cerr << "vkEnumerateDeviceLayerProperties failed: " << FormatResult(result) << std::endl;
			return;
		}

		std::cout << "Vulkan Device layers:" << std::endl;
		for (VkLayerProperties const & layer : layers) {
			std::cout << layer.layerName <<
				" | " << FormnatVersion(layer.specVersion) <<
				" | " << FormnatVersion(layer.implementationVersion) <<
				" | " << layer.description <<
				std::endl;
		}
		std::cout << std::endl;
	}

	void PrintExtensionsInformation(std::shared_ptr<RenderInstance> instance, VkPhysicalDevice device) {
		uint32_t count = 0;
		VkResult const countResult = instance->EnumerateDeviceExtensionProperties(device, nullptr, &count, nullptr);
		if (countResult != VK_SUCCESS) {
			std::cerr << "vkEnumerateDeviceExtensionProperties failed: " << FormatResult(countResult) << std::endl;
			return;
		}

		std::vector<VkExtensionProperties> extensions(count);
		VkResult const result = instance->EnumerateDeviceExtensionProperties(device, nullptr, &count, extensions.data());
		if (result != VK_SUCCESS) {
			std::cerr << "vkEnumerateDeviceExtensionProperties failed: " << FormatResult(result) << std::endl;
			return;
		}

		std::cout << "Vulkan Device extensions:" << std::endl;
		for (VkExtensionProperties const & extension : extensions) {
			std::cout << extension.extensionName <<
				" | " << FormnatVersion(extension.specVersion) <<
				std::endl;
		}
		std::cout << std::endl;
	}
}
