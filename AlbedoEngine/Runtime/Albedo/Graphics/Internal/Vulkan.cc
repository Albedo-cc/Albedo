#include "Vulkan.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <unordered_set>
#include <functional>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <format>

namespace Albedo { namespace Graphics
{
	// Global Context
	std::unique_ptr<Vulkan> g_vk = std::make_unique<Vulkan>();

	Vulkan::Vulkan()
	{
		if (g_vk != nullptr) throw std::runtime_error("Vulkan is a singleton!");
	}

	void Vulkan::Initialize(const VulkanCreateInfo& createinfo)
	{
		ALBEDO_ASSERT(createinfo.app_window   != nullptr);
		instance.app_name	 = createinfo.app_name;
		instance.app_version = createinfo.app_version;
		window.pointer		 = createinfo.app_window;		  
		messenger.callback	 = createinfo.msg_callback? 
                               createinfo.msg_callback : messenger.callback;

		// Layers & Extensions
		if (IS_DEBUG_MODE)
		{
			// Instance Layers
			instance.layers.emplace_back("VK_LAYER_KHRONOS_validation");
			instance.layers.emplace_back("VK_LAYER_RENDERDOC_Capture");

			// Instance Extensions
			instance.extensions.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
			instance.extensions.emplace_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);

			// Device Extensions
			device.extensions.emplace_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
			device.extensions.emplace_back(VK_KHR_MAINTENANCE1_EXTENSION_NAME);
        }
        else
        {
			// Instance Layers

			// Instance Extensions

			// Device Extensions
			device.extensions.emplace_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
			device.extensions.emplace_back(VK_KHR_MAINTENANCE1_EXTENSION_NAME);
        }

		// GLFW Necessary Extensions
		std::vector<const char*> requiredExtensions;
		uint32_t glfwExtensionCount = 0;
		const char** glfwExtensions;
		glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount); // Include WSI extensions

		for (uint32_t i = 0; i < glfwExtensionCount; ++i)
		{
			instance.extensions.emplace_back(*(glfwExtensions + i));
		}

		// Initialize
		create_instance();
		create_surface();
		create_physical_device();
		create_logical_device();
		create_swapchain();
		create_pipeline_cache();
	}

    Vulkan::~Vulkan() noexcept
    {
        ALBEDO_ASSERT(instance == VK_NULL_HANDLE && "Please call GRI::Terminate()");
    }

	void Vulkan::Terminate() noexcept
	{
        vkDeviceWaitIdle(device);

		destroy_pipeline_cache();
		destroy_swapchain();
		destroy_logical_device();
		destroy_surface();
		destroy_instance();
	}

	void Vulkan::create_instance()
	{
		// Enable Layers
        uint32_t layerCount = 0;
        vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

        std::vector<VkLayerProperties> availableLayers(layerCount);
        vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

        for (const char* layer : instance.layers)
        {
            bool layerFound = false;
            for (const auto& layerProperties : availableLayers)
            {
                if (strcmp(layer, layerProperties.layerName) == 0)
                {
                    layerFound = true;
                    break;
                }
            }
            if (!layerFound) throw std::runtime_error(std::format("Failed to enable the Vulkan Validation Layer {}", layer));
        }


        // Create Instance
        VkApplicationInfo appInfo
		{
			.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
			.pApplicationName   = instance.app_name,
			.applicationVersion = instance.app_version,
			.pEngineName        = ALBEDO_RHI_NAME,
			.engineVersion      = ALBEDO_RHI_VERSION,
			.apiVersion         = VULKAN_API_VERSION,
		};

        VkDebugUtilsMessengerCreateInfoEXT messengerCreateInfo
        {
            .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
            .messageSeverity=  
                //VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT    |
                VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
            .messageType    =      
                //VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT     |
                VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT  |
                VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
            .pfnUserCallback= messenger.callback,
            .pUserData      = nullptr
        };

        VkInstanceCreateInfo instanceCreateInfo
        {
            .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
            .pNext = IS_DEBUG_MODE ? (VkDebugUtilsMessengerCreateInfoEXT*)(&messengerCreateInfo) : nullptr,
            .flags = 0x0,
            .pApplicationInfo       = &appInfo,
            .enabledLayerCount      = static_cast<uint32_t>(instance.layers.size()),
            .ppEnabledLayerNames    = instance.layers.data(),
            .enabledExtensionCount  = static_cast<uint32_t>(instance.extensions.size()),
            .ppEnabledExtensionNames= instance.extensions.data(),
        };
        if (vkCreateInstance(&instanceCreateInfo, allocator, &instance.handle) != VK_SUCCESS)
			throw std::runtime_error("Failed to create the VkInstance");
        
        // Enable Messenger
        if (IS_DEBUG_MODE)
        {
            auto loadedFunction = (PFN_vkCreateDebugUtilsMessengerEXT)
                vkGetInstanceProcAddr(instance.handle, "vkCreateDebugUtilsMessengerEXT");
            if (loadedFunction == nullptr ||
                loadedFunction(instance, &messengerCreateInfo, allocator, &messenger.handle) != VK_SUCCESS)
            {
                throw std::runtime_error("Failed to create the Vulkan Debug Messenger!");
            }
        }
	}

	void Vulkan::destroy_instance()
	{
		if (IS_DEBUG_MODE)
		{
			auto loadFunction = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
			ALBEDO_ASSERT(loadFunction && "Failed to load function: vkDestroyDebugUtilsMessengerEXT");
			loadFunction(instance, messenger, allocator);
		}
        vkDestroyInstance(instance, allocator);
        instance.handle = VK_NULL_HANDLE;
	}

	void Vulkan::create_surface()
	{
       /*
		*  The window surface needs to be created right after the instance creation,
		*  because it can actually influence the physical device selection.
		*/
		if (glfwCreateWindowSurface(
			instance,
			window.pointer,
			allocator,
			&surface.handle) != VK_SUCCESS)
			throw std::runtime_error("Failed to create the Vulkan Window Surface!");
        surface;
	}

	void Vulkan::destroy_surface()
	{
        vkDestroySurfaceKHR(instance, surface, allocator);
        surface.handle = VK_NULL_HANDLE;
	}

    void Vulkan::create_physical_device()
    {
        uint32_t phyDevCnt = 0;
		vkEnumeratePhysicalDevices(instance, &phyDevCnt, nullptr);
		if (!phyDevCnt) throw std::runtime_error("Failed to enumerate GPUs with Vulkan support!");

		std::vector<VkPhysicalDevice> physicalDevices(phyDevCnt);
		vkEnumeratePhysicalDevices(instance, &phyDevCnt, physicalDevices.data());

		bool is_physical_device_suitable = false;
		for (const auto physicalDevice : physicalDevices)
		{
			GPU.handle = physicalDevice;
			if (check_physical_device_features_support() &&
				check_physical_device_queue_families_support() &&
				check_physical_device_extensions_support() &&
				check_physical_device_surface_support())
				is_physical_device_suitable = true;
			if (is_physical_device_suitable) break;
		}
		if (!is_physical_device_suitable)
			throw std::runtime_error("Failed to find a suitable GPU!");

		vkGetPhysicalDeviceFeatures(GPU, &GPU.features);
		vkGetPhysicalDeviceProperties(GPU, &GPU.properties);
		vkGetPhysicalDeviceMemoryProperties(GPU, &GPU.memory_properties);
    }

    bool Vulkan::check_physical_device_features_support()
    {
        // Properties
		vkGetPhysicalDeviceProperties(GPU, &GPU.properties);
		if (VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU != GPU.properties.deviceType)
			return false;

		// Basic Features
		vkGetPhysicalDeviceFeatures(GPU, &GPU.features);
		if (VK_TRUE != GPU.features.samplerAnisotropy)
			return false;

		// Advanced Features
		// Bindless
		/*static VkPhysicalDeviceDescriptorIndexingFeatures feature_descriptor_indexing
		{ .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES };

		GPU_features2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
		GPU_features2.pNext = &feature_descriptor_indexing;

		vkGetPhysicalDeviceFeatures2(GPU, &GPU_features2);
		if (feature_descriptor_indexing.descriptorBindingPartiallyBound != VK_TRUE ||
			feature_descriptor_indexing.runtimeDescriptorArray != VK_TRUE)
			return false;*/

		return true;
    }

    bool Vulkan::check_physical_device_queue_families_support()
    {
		uint32_t queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(GPU, &queueFamilyCount, nullptr);
		std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(GPU, &queueFamilyCount, queueFamilies.data());

		// Setup Selecting Strategy (Note: Once you change the strategy you must check "Reset Search" section!
		enum Goals { GOAL_1, GOAL_2, GOAL_3, MAX_GOALS };
		std::vector<std::function<bool(int family_idx)>> goals(MAX_GOALS);
		// Goal 1: Graphics and Present in the same queue family
		goals[GOAL_1] = [&](int family_idx)->bool
		{
			VkBool32 presentSupport = VK_FALSE;
			vkGetPhysicalDeviceSurfaceSupportKHR(GPU, family_idx, surface, &presentSupport);
			return VK_TRUE == presentSupport &&
				VK_QUEUE_GRAPHICS_BIT & queueFamilies[family_idx].queueFlags;
		};
		// Goal 2: Transfer Queue in a distinct queue family
		goals[GOAL_2] = [&](int family_idx)->bool
		{
			return VK_QUEUE_TRANSFER_BIT & queueFamilies[family_idx].queueFlags; 
		};
		// Goal 3: Compute Queue in a distinct queue family
		goals[GOAL_3] = [&](int family_idx)->bool
		{
			return VK_QUEUE_COMPUTE_BIT & queueFamilies[family_idx].queueFlags;
		};


		std::vector<int> selected(goals.size());
		std::vector<bool> visited(queueFamilies.size());

		auto search = [&](int cur_family_idx, Goals cur_goal_idx, auto&& _search) -> bool
		{
			if (false == goals[cur_goal_idx](cur_family_idx)) return false;
			else
			{
				selected[cur_goal_idx] = cur_family_idx;
				if (MAX_GOALS == cur_goal_idx + 1) return true;
			}
			
			visited[cur_family_idx] = true;
			for (int i = 0; i < queueFamilies.size(); ++i)
			{
				if (!visited[i] && _search(i, Goals(cur_goal_idx + 1), _search))
					return true;
			}
			visited[cur_family_idx] = false; // Trace back
			return false;
		};

		// Reset Search
		for (int i = 0; i < queueFamilies.size(); ++i)
		{
			std::fill(selected.begin(), selected.end(), ~0);
			std::fill(visited.begin(), visited.end(), false);

			if (search(i, GOAL_1, search))
			{
				device.queue_families.graphics.index = selected[GOAL_1];
				device.queue_families.present.index	 = selected[GOAL_1];
				device.queue_families.transfer.index = selected[GOAL_2];
				device.queue_families.compute.index  = selected[GOAL_3];
				ALBEDO_ASSERT(device.queue_families.graphics.index.has_value());
				return true;
			}
		} 

		// Check Required Queue Family
		return false;
    }

    bool Vulkan::check_physical_device_extensions_support()
    {
		uint32_t extensionCount;
		vkEnumerateDeviceExtensionProperties(GPU, nullptr, &extensionCount, nullptr);
		std::vector<VkExtensionProperties> availableExtensions(extensionCount);
		vkEnumerateDeviceExtensionProperties(GPU, nullptr, &extensionCount, availableExtensions.data());

		std::unordered_set<std::string> // Don't use const char* here!
			requiredExtensions(device.extensions.begin(), device.extensions.end());

		for (const auto& extension : availableExtensions)
		{
			requiredExtensions.erase(extension.extensionName);
		}
		return requiredExtensions.empty();
    }

    bool Vulkan::check_physical_device_surface_support()
    {
		// 1. Surface Formats
		uint32_t formatCount;
		vkGetPhysicalDeviceSurfaceFormatsKHR(GPU, surface, &formatCount, nullptr);
		if (formatCount)
		{
			surface.formats.resize(formatCount);
			vkGetPhysicalDeviceSurfaceFormatsKHR(GPU, surface, &formatCount, surface.formats.data());
		}

		// 2. Present Modes
		uint32_t presentModeCount;
		vkGetPhysicalDeviceSurfacePresentModesKHR(GPU, surface, &presentModeCount, nullptr);
		if (presentModeCount)
		{
			surface.present_modes.resize(presentModeCount);
			vkGetPhysicalDeviceSurfacePresentModesKHR(GPU, surface, &presentModeCount, surface.present_modes.data());
		}

		return !surface.formats.empty() && !surface.present_modes.empty();
    }

	void Vulkan::create_logical_device()
	{
		std::vector<VkDeviceQueueCreateInfo> deviceQueueCreateInfos;

		// Graphics & Present Queue Family
		ALBEDO_ASSERT(device.queue_families.graphics == device.queue_families.present);
		std::vector<float> graphics_queue_priorities;
		{
			for (const auto& queue : device.queue_families.graphics.queues)
				graphics_queue_priorities.emplace_back(queue.priority);

			deviceQueueCreateInfos.emplace_back(VkDeviceQueueCreateInfo
				{
					.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
					.queueFamilyIndex = device.queue_families.graphics.index.value(),
					.queueCount = static_cast<uint32_t>(graphics_queue_priorities.size()),
					.pQueuePriorities = graphics_queue_priorities.data()
				});
		}

		// Compute Queue Family
		ALBEDO_ASSERT(device.queue_families.compute != device.queue_families.graphics && 
			   device.queue_families.compute != device.queue_families.transfer);
		std::vector<float> compute_queue_priorities;
		{
			for (const auto& queue : device.queue_families.compute.queues) 
				compute_queue_priorities.emplace_back(queue.priority);

			deviceQueueCreateInfos.emplace_back(VkDeviceQueueCreateInfo
				{
					.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
					.queueFamilyIndex = device.queue_families.compute.index.value(),
					.queueCount = static_cast<uint32_t>(compute_queue_priorities.size()),
					.pQueuePriorities = compute_queue_priorities.data()
				});
		}

		// Transfer Queue Family
		ALBEDO_ASSERT(device.queue_families.transfer != device.queue_families.graphics);
		std::vector<float> transfer_queue_priorities;
		{
			for (const auto& queue : device.queue_families.transfer.queues) 
				transfer_queue_priorities.emplace_back(queue.priority);

			deviceQueueCreateInfos.emplace_back(VkDeviceQueueCreateInfo
				{
					.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
					.queueFamilyIndex = device.queue_families.transfer.index.value(),
					.queueCount = static_cast<uint32_t>(transfer_queue_priorities.size()),
					.pQueuePriorities = transfer_queue_priorities.data()
				});
		}

		VkDeviceCreateInfo deviceCreateInfo
		{
			.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
			
			.pNext = nullptr/*m_physical_device_features2 .has_value()? &(m_physical_device_features2.value()) : */,
			.queueCreateInfoCount = static_cast<uint32_t>(deviceQueueCreateInfos.size()),
			.pQueueCreateInfos = deviceQueueCreateInfos.data(),
			.enabledExtensionCount = static_cast<uint32_t>(device.extensions.size()),
			.ppEnabledExtensionNames = device.extensions.data(),
			.pEnabledFeatures = &GPU.features/*m_physical_device_features2.has_value() ? nullptr : &m_physical_device_features*/// (If pNext includes a VkPhysicalDeviceFeatures2, here should be NULL)
		};
		
		if (vkCreateDevice(GPU, &deviceCreateInfo, allocator, &device.handle) != VK_SUCCESS)
			throw std::runtime_error("Failed to create the logical device!");

		// Graphics & Present Queue Family
		ALBEDO_ASSERT(device.queue_families.graphics == device.queue_families.present);
		for (uint32_t i = 0; i < device.queue_families.graphics.queues.size(); ++i)
		{
			auto& queue_family = device.queue_families.graphics;
			vkGetDeviceQueue(device, queue_family, i, &queue_family.queues[i].handle);
			device.queue_families.present.queues[i].handle = queue_family.queues[i];
		}

		// Compute Queue Family
		for (uint32_t i = 0; i < device.queue_families.compute.queues.size(); ++i)
		{
			auto& queue_family = device.queue_families.compute;
			vkGetDeviceQueue(device, queue_family, i, &queue_family.queues[i].handle);
		}

		// Transfer Queue Family
		for (uint32_t i = 0; i < device.queue_families.transfer.queues.size(); ++i)
		{
			auto& queue_family = device.queue_families.transfer;
			vkGetDeviceQueue(device, queue_family, i, &queue_family.queues[i].handle);
		}
	}

	void Vulkan::destroy_logical_device()
	{
		vkDestroyDevice(device, allocator);
		device.handle = VK_NULL_HANDLE;
	}

	void Vulkan::create_swapchain()
	{
		if (!check_swap_chain_image_format_support())
			throw std::runtime_error(std::format("Failed to create the Vulkan Swap Chain - Image format is not supported!"));
		if (!check_swap_chain_depth_format_support())
			throw std::runtime_error(std::format("Failed to create the Vulkan Swap Chain - Depth format is not supported!"));
		if (!check_swap_chain_present_mode_support())
			throw std::runtime_error(std::format("Failed to create the Vulkan Swap Chain - Present mode is not supported!"));

		VkSurfaceCapabilitiesKHR current_surface_capabilities{};
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(GPU, surface, &current_surface_capabilities);

		// Begin(Choose Swap Extent (resolution of images in swap chain))
		{
			constexpr uint32_t SPECIAL_VALUE_OF_WINDOW_MANAGER = std::numeric_limits<uint32_t>::max(); // More details in textbook P85
			if (current_surface_capabilities.currentExtent.height == SPECIAL_VALUE_OF_WINDOW_MANAGER)
			{
				int width, height;
				glfwGetFramebufferSize(window, &width, &height);
				while (width <= 0 || height <= 0)
				{

					glfwGetFramebufferSize(window, &width, &height);
					glfwWaitEvents();
				}				

				swapchain.extent = VkExtent2D
				{
					.width = std::clamp(static_cast<uint32_t>(width),
							current_surface_capabilities.minImageExtent.width,
							current_surface_capabilities.maxImageExtent.width),
					.height = std::clamp(static_cast<uint32_t>(height),
							current_surface_capabilities.minImageExtent.height,
							current_surface_capabilities.maxImageExtent.height)
				};
			}
			else
			{
				while (current_surface_capabilities.currentExtent.width <= 0 ||
					current_surface_capabilities.currentExtent.height <= 0)
				{
					vkGetPhysicalDeviceSurfaceCapabilitiesKHR(GPU, surface, &current_surface_capabilities);
					glfwWaitEvents();
				}
				swapchain.extent = current_surface_capabilities.currentExtent;
			}
		} // End(Choose Swap Extent (resolution of images in swap chain)

		// Decide how many images we would like to have in the swap chain
		uint32_t swapchain_image_count = std::clamp(
				current_surface_capabilities.minImageCount + 1,
				current_surface_capabilities.minImageCount + 1,
				current_surface_capabilities.maxImageCount);
		ALBEDO_ASSERT(swapchain_image_count != 0 && "0 means no limits");
		swapchain.images.resize(swapchain_image_count);

		bool is_exclusive_device = (device.queue_families.graphics.index == device.queue_families.present.index);
		uint32_t queue_family_indices[2]
		{ 
			device.queue_families.graphics.index.value(), 
			device.queue_families.present.index.value() 
		};
		VkSwapchainCreateInfoKHR swapChainCreateInfo
		{
			.sType					= VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
			.surface				= surface,
			.minImageCount			= static_cast<uint32_t>(swapchain.images.size()), // Note that, this is just a minimum number of images in the swap chain, the implementation could make it more.
			.imageFormat			= swapchain.format,
			.imageColorSpace		= swapchain.color_space,
			.imageExtent			= swapchain.extent,
			.imageArrayLayers		= 1,
			.imageUsage				= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT |
									  VK_IMAGE_USAGE_TRANSFER_DST_BIT,
			.imageSharingMode		= is_exclusive_device ? VK_SHARING_MODE_EXCLUSIVE : VK_SHARING_MODE_CONCURRENT,
			.queueFamilyIndexCount	= is_exclusive_device ? 0U : 2U,
			.pQueueFamilyIndices	= is_exclusive_device ? nullptr : queue_family_indices,
			.preTransform			= current_surface_capabilities.currentTransform, // Do not want any pretransformation
			.compositeAlpha			= VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
			.presentMode			= swapchain.present_mode,
			.clipped				= VK_TRUE, // Means that we do not care about the color of pixels that are obscured for the best performance. (P89)
			.oldSwapchain			= VK_NULL_HANDLE
		};
		if (vkCreateSwapchainKHR(device, &swapChainCreateInfo, allocator, &swapchain.handle) != VK_SUCCESS)
			throw std::runtime_error("Failed to create the Vulkan Swap Chain!");

		// Retrieve the swap chain images
		vkGetSwapchainImagesKHR(device, swapchain, &swapchain_image_count, nullptr);
		swapchain.images.resize(swapchain_image_count);
		vkGetSwapchainImagesKHR(device, swapchain, &swapchain_image_count, swapchain.images.data());
	
		// Create Image Views
		swapchain.image_views.resize(swapchain_image_count);
		for (size_t idx = 0; idx < swapchain_image_count; ++idx)
		{
			VkImageViewCreateInfo imageViewCreateInfo
			{
				.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
				.image = swapchain.images[idx],
				.viewType = VK_IMAGE_VIEW_TYPE_2D,
				.format = swapchain.format,
				.components{
							.r = VK_COMPONENT_SWIZZLE_IDENTITY,
							.g = VK_COMPONENT_SWIZZLE_IDENTITY,
							.b = VK_COMPONENT_SWIZZLE_IDENTITY,
							.a = VK_COMPONENT_SWIZZLE_IDENTITY
							},
				.subresourceRange{
							.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
							.baseMipLevel = 0,
							.levelCount = 1,
							.baseArrayLayer = 0,
							.layerCount = 1
							}
			};
			if (vkCreateImageView(device, 
				&imageViewCreateInfo, 
				allocator,
				&swapchain.image_views[idx]) != VK_SUCCESS)
				throw std::runtime_error("Failed to create all image views");
		}

		// Reset Cursor
		swapchain.cursor = 0;
	}

	void Vulkan::destroy_swapchain()
	{
		for (auto imageview : swapchain.image_views)
		{
			vkDestroyImageView(device, imageview, allocator);
		}
		vkDestroySwapchainKHR(device, swapchain, allocator);
		swapchain.handle = VK_NULL_HANDLE;
	}

	void Vulkan::recreate_swapchain()
	{
		vkDeviceWaitIdle(device);
		VkSwapchainKHR new_swapchain_handle = VK_NULL_HANDLE;

		VkSurfaceCapabilitiesKHR current_surface_capabilities{};
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(GPU, surface, &current_surface_capabilities);

		// Begin(Choose Swap Extent (resolution of images in swap chain))
		{
			constexpr uint32_t SPECIAL_VALUE_OF_WINDOW_MANAGER = std::numeric_limits<uint32_t>::max(); // More details in textbook P85
			if (current_surface_capabilities.currentExtent.height == SPECIAL_VALUE_OF_WINDOW_MANAGER)
			{
				int width, height;
				glfwGetFramebufferSize(window, &width, &height);
				while (width <= 0 || height <= 0)
				{

					glfwGetFramebufferSize(window, &width, &height);
					glfwWaitEvents();
				}				

				swapchain.extent = VkExtent2D
				{
					.width = std::clamp(static_cast<uint32_t>(width),
							current_surface_capabilities.minImageExtent.width,
							current_surface_capabilities.maxImageExtent.width),
					.height = std::clamp(static_cast<uint32_t>(height),
							current_surface_capabilities.minImageExtent.height,
							current_surface_capabilities.maxImageExtent.height)
				};
			}
			else
			{
				while (current_surface_capabilities.currentExtent.width <= 0 ||
					current_surface_capabilities.currentExtent.height <= 0)
				{
					vkGetPhysicalDeviceSurfaceCapabilitiesKHR(GPU, surface, &current_surface_capabilities);
					glfwWaitEvents();
				}
				swapchain.extent = current_surface_capabilities.currentExtent;
			}
		} // End(Choose Swap Extent (resolution of images in swap chain)

		// Decide how many images we would like to have in the swap chain
		bool is_exclusive_device = (device.queue_families.graphics.index == device.queue_families.present.index);
		uint32_t queue_family_indices[2]
		{ 
			device.queue_families.graphics.index.value(), 
			device.queue_families.present.index.value() 
		};
		VkSwapchainCreateInfoKHR swapChainCreateInfo
		{
			.sType					= VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
			.surface				= surface,
			.minImageCount			= static_cast<uint32_t>(swapchain.images.size()), // Note that, this is just a minimum number of images in the swap chain, the implementation could make it more.
			.imageFormat			= swapchain.format,
			.imageColorSpace		= swapchain.color_space,
			.imageExtent			= swapchain.extent,
			.imageArrayLayers		= 1,
			.imageUsage				= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT |
									  VK_IMAGE_USAGE_TRANSFER_DST_BIT,
			.imageSharingMode		= is_exclusive_device ? VK_SHARING_MODE_EXCLUSIVE : VK_SHARING_MODE_CONCURRENT,
			.queueFamilyIndexCount	= is_exclusive_device ? 0U : 2U,
			.pQueueFamilyIndices	= is_exclusive_device ? nullptr : queue_family_indices,
			.preTransform			= current_surface_capabilities.currentTransform, // Do not want any pretransformation
			.compositeAlpha			= VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
			.presentMode			= swapchain.present_mode,
			.clipped				= VK_TRUE, // Means that we do not care about the color of pixels that are obscured for the best performance. (P89)
			.oldSwapchain			= swapchain,
		};
		if (vkCreateSwapchainKHR(device, &swapChainCreateInfo, allocator, &new_swapchain_handle) != VK_SUCCESS)
			throw std::runtime_error("Failed to create the Vulkan Swap Chain!");

		// Handover
		destroy_swapchain();
		swapchain.handle = new_swapchain_handle;

		// Retrieve the swap chain images
		uint32_t swapchain_image_count = swapchain.images.size();
		vkGetSwapchainImagesKHR(device, swapchain, &swapchain_image_count, swapchain.images.data());
	
		// Create Image Views
		swapchain.image_views.resize(swapchain_image_count);
		for (size_t idx = 0; idx < swapchain_image_count; ++idx)
		{
			VkImageViewCreateInfo imageViewCreateInfo
			{
				.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
				.image = swapchain.images[idx],
				.viewType = VK_IMAGE_VIEW_TYPE_2D,
				.format = swapchain.format,
				.components{
							.r = VK_COMPONENT_SWIZZLE_IDENTITY,
							.g = VK_COMPONENT_SWIZZLE_IDENTITY,
							.b = VK_COMPONENT_SWIZZLE_IDENTITY,
							.a = VK_COMPONENT_SWIZZLE_IDENTITY
							},
				.subresourceRange{
							.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
							.baseMipLevel = 0,
							.levelCount = 1,
							.baseArrayLayer = 0,
							.layerCount = 1
							}
			};
			if (vkCreateImageView(device, 
				&imageViewCreateInfo, 
				allocator,
				&swapchain.image_views[idx]) != VK_SUCCESS)
				throw std::runtime_error("Failed to create all image views");
		}

		// Reset Cursor
		swapchain.cursor = 0;
	}

	bool Vulkan::check_swap_chain_image_format_support()
	{
		for (const auto& surface_format : surface.formats)
		{
			if (surface_format.format == swapchain.format &&
				surface_format.colorSpace == swapchain.color_space)
			{
				return true;
			}	
		}
		return false;
	}

	bool Vulkan::check_swap_chain_depth_format_support()
	{
		// Deduce Channels
		//switch(m_swapchain_depth_stencil_format)
		/*if (VK_FORMAT_D32_SFLOAT == m_swapchain_depth_stencil_format)
		{
			m_swapchain_stencil_channel = 0;
			m_swapchain_depth_channel = 4;
		}
		else if (VK_FORMAT_D32_SFLOAT_S8_UINT == m_swapchain_depth_stencil_format)
		{
			m_swapchain_stencil_channel = 1;
			m_swapchain_depth_channel = 4;
		}
		else if (VK_FORMAT_D24_UNORM_S8_UINT == m_swapchain_depth_stencil_format)
		{
			m_swapchain_stencil_channel = 1;
			m_swapchain_depth_channel = 3;
		}
		else throw std::runtime_error("Failed to deduce the Depth Image Format!");*/

		VkFormatProperties format_properties{};
		vkGetPhysicalDeviceFormatProperties(GPU, swapchain.zbuffer_format, &format_properties);
		// 1. Tiling Linear
		if (VK_IMAGE_TILING_LINEAR == swapchain.zbuffer_format &&
			VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT & format_properties.linearTilingFeatures)
			return true;

		// 2. Tiling Optimal
		if (VK_IMAGE_TILING_OPTIMAL == swapchain.zbuffer_tiling &&
			VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT & format_properties.optimalTilingFeatures)
			return true;

		return false;
	}

	bool Vulkan::check_swap_chain_present_mode_support()
	{
		for (const auto& surface_present_mode : surface.present_modes)
		{
			if (surface_present_mode == swapchain.present_mode)
			{
				return true;
			}
		}
		return false;
	}

	void Vulkan::create_pipeline_cache()
	{
		std::vector<char> buffer;
		// Read Pipeline Cache File
		std::ifstream cache(pipeline_cache.file, std::ios::ate | std::ios::binary);
		if (cache.is_open())
		{
			size_t cache_size = static_cast<size_t>(cache.tellg());
			buffer.resize(cache_size);
			cache.seekg(0);
			cache.read(buffer.data(), cache_size);
			cache.close();
			// The data in the cache is controlled by each vendor driver implementation.
			auto cache_header = (VkPipelineCacheHeaderVersionOne*)(buffer.data());
			if (cache_header->deviceID != GPU.properties.deviceID ||
				cache_header->vendorID != GPU.properties.vendorID ||
				memcmp(cache_header->pipelineCacheUUID, GPU.properties.pipelineCacheUUID, VK_UUID_SIZE))
			{
				pipeline_cache.is_expired = true;
				buffer.clear();
			}
			else pipeline_cache.is_expired = false;
		}
		else pipeline_cache.is_expired = true;

		VkPipelineCacheCreateInfo pipelineCacheCreateInfo
		{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO,
			.initialDataSize = buffer.size(),
			.pInitialData = buffer.data(),
		};

		if (vkCreatePipelineCache(
			device,
			&pipelineCacheCreateInfo,
			allocator,
			&pipeline_cache.handle) != VK_SUCCESS)
			throw std::runtime_error("Failed to create Vulkan Pipeline Cache!");
	}

	void Vulkan::destroy_pipeline_cache()
	{
		if (pipeline_cache.is_expired)
		{
			std::ofstream cache(pipeline_cache.file, std::ios::trunc | std::ios::binary);
			if (cache.is_open())
			{
				size_t cache_size = 0;
				vkGetPipelineCacheData(device, pipeline_cache, &cache_size, nullptr);
				std::vector<char> cache_data(cache_size);
				vkGetPipelineCacheData(device, pipeline_cache, &cache_size, cache_data.data());
				cache.write(cache_data.data(), cache_data.size());
				cache.close();
			}
			else std::cerr << "Failed to save pipeline cache file!\n"; // Not throw in Deconstructor
		}
		vkDestroyPipelineCache(device, pipeline_cache, allocator);
		pipeline_cache.handle = VK_NULL_HANDLE;
	}

	VKAPI_ATTR VkBool32 VKAPI_CALL
        Vulkan::default_messenger_callback(
            VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
            VkDebugUtilsMessageTypeFlagsEXT messageType,
            const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
            void* pUserData)
		{
			switch (messageSeverity)
			{
			case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
				//std::cerr << "\n[Vulkan Verbose]: " << pCallbackData->pMessage;
				break;
			case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
				//std::cerr << "\n[Vulkan Info]: " << pCallbackData->pMessage;
				break;
			case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
				std::cerr << "\n[Vulkan Warn]: " << pCallbackData->pMessage;
				break;
			case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
				std::cerr << "\n[Vulkan Error]: " << pCallbackData->pMessage;
				break;
			default:
				std::cerr << "\n[Vulkan Exception]: Unknow Message Severity " << messageSeverity;
			}
			return VK_FALSE; // Always return VK_FALSE
		}

}} // namespace Albedo::Graphics