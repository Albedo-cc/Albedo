#include "vulkan_context.h"

namespace Albedo {
namespace RHI
{
	std::vector<uint32_t> VulkanContext::s_debug_message_statistics(4, 0);

	VulkanContext::VulkanContext(GLFWwindow* window) :
		m_window{ window }
	{
		enable_validation_layers();

		initialize_vulkan_instance();
		initialize_debug_messenger();
		initialize_surface();
		initialize_physical_device();
		initialize_logical_device();
	}

	VulkanContext::~VulkanContext()
	{
		destroy_logical_device();
		destroy_surface();
		destroy_debug_messenger();
		destroy_vulkan_instance();
	}

	void VulkanContext::enable_validation_layers()
	{
		if (!EnableValidationLayers) return;

		uint32_t layerCount;
		vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

		std::vector<VkLayerProperties> availableLayers(layerCount);
		vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

		for (const char* layerName : m_validation_layers)
		{
			bool layerFound = false;

			for (const auto& layerProperties : availableLayers)
			{
				if (strcmp(layerName, layerProperties.layerName) == 0)
				{
					layerFound = true;
					break;
				}
			}
			if (!layerFound) throw std::runtime_error(std::format("Failed to enable vulkan validation layer {}", layerName));
		}
	}

	void VulkanContext::initialize_vulkan_instance()
	{
		// Extensions
		std::vector<const char*> requiredExtensions;
		uint32_t glfwExtensionCount = 0;
		const char** glfwExtensions;
		glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount); // Include WSI extensions
		std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);
		if (EnableValidationLayers) extensions.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

		// Instance
		VkApplicationInfo appInfo
		{
			.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
			.pApplicationName = "Albedo RHI",
			.applicationVersion = VK_MAKE_VERSION(1, 0, 0),
			.pEngineName = "Albedo",
			.engineVersion = VK_MAKE_VERSION(1, 0, 0),
			.apiVersion = VK_API_VERSION_1_0,
		};

		VkInstanceCreateInfo instanceCreateInfo{};
		instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		instanceCreateInfo.pApplicationInfo = &appInfo;

		instanceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
		instanceCreateInfo.ppEnabledExtensionNames = extensions.data();

		auto messengerCreateInfo = VulkanContext::GetDefaultDebuggerMessengerCreateInfo();
		if (EnableValidationLayers)
		{
			instanceCreateInfo.enabledLayerCount = static_cast<uint32_t>(m_validation_layers.size());
			instanceCreateInfo.ppEnabledLayerNames = m_validation_layers.data();
			instanceCreateInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)(&messengerCreateInfo);
		}
		else
		{
			instanceCreateInfo.enabledLayerCount = 0;
			instanceCreateInfo.ppEnabledLayerNames = nullptr;
			instanceCreateInfo.pNext = nullptr;
		}

		if (vkCreateInstance(&instanceCreateInfo, nullptr, &m_instance) != VK_SUCCESS)
			throw std::runtime_error("Failed to create the VkInstance");
	}

	void VulkanContext::initialize_debug_messenger()
	{
		if (!EnableValidationLayers) return;

		auto messengerCreateInfo = VulkanContext::GetDefaultDebuggerMessengerCreateInfo();
		auto loadedFunction = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(m_instance, "vkCreateDebugUtilsMessengerEXT");
		if (loadedFunction == nullptr ||
			loadedFunction(m_instance, &messengerCreateInfo, nullptr, &m_debug_messenger) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to set up vulkan debug messenger!");
		}
	}

	void VulkanContext::initialize_surface()
	{
		if (glfwCreateWindowSurface(
			m_instance,
			m_window,
			m_memory_allocator,
			&m_surface) != VK_SUCCESS)
			throw std::runtime_error("Failed to create window surface!");
	}


	void VulkanContext::initialize_physical_device()
	{
		uint32_t phyDevCnt = 0;
		vkEnumeratePhysicalDevices(m_instance, &phyDevCnt, nullptr);
		if (!phyDevCnt) throw std::runtime_error("Failed to enumerate GPUs with Vulkan support!");

		std::vector<VkPhysicalDevice> physicalDevices(phyDevCnt);
		vkEnumeratePhysicalDevices(m_instance, &phyDevCnt, physicalDevices.data());

		bool is_physical_device_suitable = false;
		for (const auto& physicalDevice : physicalDevices)
		{
			m_physical_device = physicalDevice;
			if (check_physical_device_features_support() &&
				check_physical_device_queue_families_support() &&
				check_physical_device_extensions_support() &&
				check_physical_device_surface_support())
				is_physical_device_suitable = true;
			if (is_physical_device_suitable) break;
		}
		if (!is_physical_device_suitable)
			throw std::runtime_error("Failed to find a suitable GPU!");

		vkGetPhysicalDeviceFeatures(m_physical_device, &m_physical_device_features);
		vkGetPhysicalDeviceMemoryProperties(m_physical_device, &m_physical_device_memory_properties);
	}

	void VulkanContext::initialize_logical_device()
	{
		std::vector<VkDeviceQueueCreateInfo> deviceQueueCreateInfos;
		std::unordered_map<uint32_t, size_t> visitedIndicesAndSize;
		for (const auto& [familyRequirement, priorities] : builder.familyRequirementsAndPriorities)
		{
			auto familyIndex = this->queueFamilies.find(familyRequirement);
			//[Vulkan Tutorial - P77]: If the queue families are the same, then we only need to pass its index once.
			if (visitedIndicesAndSize.find(familyIndex) != visitedIndicesAndSize.end())
			{
				if (builder.familyRequirementsAndPriorities[familyRequirement].size() != visitedIndicesAndSize[familyIndex])
					throw std::runtime_error("[SphDevice Builder Error]>> You are creating more than 1 queues at same QueueFamily but have different size!");
				continue;
			}
			else visitedIndicesAndSize[familyIndex] = builder.familyRequirementsAndPriorities[familyRequirement].size();

			deviceQueueCreateInfos.emplace_back(VkDeviceQueueCreateInfo
				{
					.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
					.queueFamilyIndex = familyIndex,
					.queueCount = static_cast<uint32_t>(priorities.size()),
					.pQueuePriorities = priorities.data()
				});
		}

		VkDeviceCreateInfo deviceCreateInfo
		{
			.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
			.queueCreateInfoCount = static_cast<uint32_t>(deviceQueueCreateInfos.size()),
			.pQueueCreateInfos = deviceQueueCreateInfos.data(),
			.enabledExtensionCount = static_cast<uint32_t>(Sapphire::context.deviceExtensions.size()),
			.ppEnabledExtensionNames = Sapphire::context.deviceExtensions.data(),
			.pEnabledFeatures = &deviceFeatures
		};
	}

	void VulkanContext::destroy_logical_device()
	{
		vkDeviceWaitIdle(m_device);
	}

	void VulkanContext::destroy_debug_messenger()
	{
		if (s_debug_message_statistics[VERBOSE]) log::info("[Vulkan Messenger] VERBOSE: {}", s_debug_message_statistics[VERBOSE]);
		if (s_debug_message_statistics[INFO]) log::info("[Vulkan Messenger] INFO: {}", s_debug_message_statistics[INFO]);
		if (s_debug_message_statistics[WARN]) log::info("[Vulkan Messenger] WARN: {}", s_debug_message_statistics[WARN]);
		if (s_debug_message_statistics[ERROR]) log::info("[Vulkan Messenger] ERROR: {}", s_debug_message_statistics[ERROR]);
		auto loadFunction = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(m_instance, "vkDestroyDebugUtilsMessengerEXT");
		if (loadFunction != nullptr) loadFunction(m_instance, m_debug_messenger, m_memory_allocator);
		else
		{
			log::error("Failed to load function: vkDestroyDebugUtilsMessengerEXT");
			exit(-1);
		}
	}

	void VulkanContext::destroy_surface()
	{
		vkDestroySurfaceKHR(m_instance, m_surface, m_memory_allocator);
	}

	void VulkanContext::destroy_vulkan_instance()
	{
		vkDestroyInstance(m_instance, m_memory_allocator);
	}

	bool VulkanContext::check_physical_device_features_support()
	{
		VkPhysicalDeviceProperties phyDevProperties;
		VkPhysicalDeviceFeatures phyDevFeatures;
		vkGetPhysicalDeviceProperties(m_physical_device, &phyDevProperties);
		vkGetPhysicalDeviceFeatures(m_physical_device, &phyDevFeatures);

		//if (!phyDevFeatures.samplerAnisotropy) return false;

		/*if(phyDevProperties.deviceType != VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU ||
			phyDevFeatures.geometryShader != VK_TRUE)
			return false;*/
		return true;
	}

	bool VulkanContext::check_physical_device_queue_families_support()
	{
		// Find Queue Families
		m_device_queue_graphics.reset();
		m_device_queue_present.reset();
		m_device_queue_compute.reset();
		m_device_queue_transfer.reset();
		m_device_queue_sparsebinding.reset();

		uint32_t queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(m_physical_device, &queueFamilyCount, nullptr);
		std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(m_physical_device, &queueFamilyCount, queueFamilies.data());

		int idx = 0;
		for (const auto& queueFamily : queueFamilies)
		{
			bool graphicsSupport = false;
			bool computeSupport = false;
			bool transferSupport = false;
			bool sparseBindingSupport = false;
			VkBool32 presentSupport = VK_FALSE;

			if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) graphicsSupport = true;
			if (queueFamily.queueFlags & VK_QUEUE_COMPUTE_BIT) computeSupport = true;
			if (queueFamily.queueFlags & VK_QUEUE_TRANSFER_BIT) transferSupport = true;
			if (queueFamily.queueFlags & VK_QUEUE_SPARSE_BINDING_BIT) sparseBindingSupport = true;
			vkGetPhysicalDeviceSurfaceSupportKHR(m_physical_device, idx, m_surface, &presentSupport);

			// Any queue family with VK_QUEUE_GRAPHICS_BIT or VK_QUEUE_COMPUTE_BIT capabilities already implicitly support VK_QUEUE_TRANSFER_BIT operations
			if (graphicsSupport && !m_device_queue_graphics.has_value())
				m_device_queue_graphics = idx;
			if (computeSupport && !m_device_queue_compute.has_value())
				m_device_queue_compute = idx;
			if (transferSupport && !m_device_queue_transfer.has_value())
				m_device_queue_transfer = idx;
			if (sparseBindingSupport && !m_device_queue_sparsebinding.has_value())
				m_device_queue_sparsebinding = idx;
			if (presentSupport == VK_TRUE && !m_device_queue_present.has_value())
				m_device_queue_present = idx;

			// The Graphics Queue Index is better to be same as the Present Queue Index.
			if (graphicsSupport && (presentSupport == VK_TRUE) &&
				(m_device_queue_graphics != m_device_queue_present))
			{
				m_device_queue_graphics = idx;
				m_device_queue_present = idx;
			}
			++idx;
		}// End Loop - find family

		// Check Required Queue Family
		for (const auto&[queue_family, priority] : m_required_queue_families_with_priorities)
		{
			if (!queue_family->has_value()) return false;
		}
		return true;
	}

	bool VulkanContext::check_physical_device_extensions_support()
	{
		uint32_t extensionCount;
		vkEnumerateDeviceExtensionProperties(m_physical_device, nullptr, &extensionCount, nullptr);
		std::vector<VkExtensionProperties> availableExtensions(extensionCount);
		vkEnumerateDeviceExtensionProperties(m_physical_device, nullptr, &extensionCount, availableExtensions.data());
		std::unordered_set<std::string> requiredExtensions(m_device_extensions.begin(), m_device_extensions.end());
		for (const auto& extension : availableExtensions)
			requiredExtensions.erase(extension.extensionName);

		return requiredExtensions.empty();
	}


	bool VulkanContext::check_physical_device_surface_support()
	{
		// 1. Surface Capabilities
		VkSurfaceCapabilitiesKHR capabilities;
		std::vector<VkSurfaceFormatKHR> formatAndColorSpace; // 1.Format e.g. VK_FORMAT_R8G8B8A8_SRGB 2. ColorSpace, e.g. VK_COLOR_SPACE_SRGB_NONLINEAR_KHR
		std::vector<VkPresentModeKHR> presentModes;

		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_physical_device, m_surface,&capabilities);

		// 2. Surface Formats
		uint32_t formatCount;
		vkGetPhysicalDeviceSurfaceFormatsKHR(m_physical_device, m_surface, &formatCount, nullptr);
		if (formatCount)
		{
			formatAndColorSpace.resize(formatCount);
			vkGetPhysicalDeviceSurfaceFormatsKHR(m_physical_device, m_surface, &formatCount, formatAndColorSpace.data());
		}

		// 3. Present Modes
		uint32_t presentModeCount;
		vkGetPhysicalDeviceSurfacePresentModesKHR(m_physical_device, m_surface, &presentModeCount, nullptr);
		if (presentModeCount)
		{
			presentModes.resize(presentModeCount);
			vkGetPhysicalDeviceSurfacePresentModesKHR(m_physical_device, m_surface, &presentModeCount, presentModes.data());
		}

		return !formatAndColorSpace.empty() && !presentModes.empty();
	}

}} // namespace Albedo::RHI