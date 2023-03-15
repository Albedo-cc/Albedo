#pragma once

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

#include <AlbedoLog.hpp>

#include <vector>
#include <format>
#include <optional>
#include <unordered_set>

namespace Albedo {
namespace RHI
{
#ifdef NDEBUG
	constexpr const bool EnableValidationLayers = false;
#else
	constexpr const bool EnableValidationLayers = true;
#endif

	class VulkanContext
	{
	public:
		VkDebugUtilsMessengerEXT	m_debug_messenger	= nullptr;
		VkAllocationCallbacks*			m_memory_allocator	= nullptr;
		VkInstance								m_instance					= nullptr;
		GLFWwindow*							m_window						= nullptr;
		VkSurfaceKHR							m_surface						= nullptr;
		VkPhysicalDevice						m_physical_device		= nullptr;
		VkPhysicalDeviceFeatures		m_physical_device_features;
		VkPhysicalDeviceMemoryProperties m_physical_device_memory_properties;

		VkDevice									m_device						= nullptr;
		std::optional<uint32_t>			m_device_queue_graphics;
		std::optional<uint32_t>			m_device_queue_present;
		std::optional<uint32_t>			m_device_queue_compute;
		std::optional<uint32_t>			m_device_queue_transfer;
		std::optional<uint32_t>			m_device_queue_sparsebinding;

	private:
		std::vector<std::pair<std::optional<uint32_t>*, float>> 
			m_required_queue_families_with_priorities{ 
				{&m_device_queue_graphics,		1.0f},
				{&m_device_queue_present,		1.0f }};
		std::vector<const char*>			m_validation_layers{ "VK_LAYER_KHRONOS_validation" };
		std::vector<const char*>			m_device_extensions{ VK_KHR_SWAPCHAIN_EXTENSION_NAME };

	public:
		VulkanContext() = delete;
		VulkanContext(GLFWwindow* window);
		~VulkanContext();

	private:
		// Initialization
		void enable_validation_layers();
		void initialize_vulkan_instance();
		void initialize_debug_messenger();
		void initialize_surface();
		void initialize_physical_device();
		void initialize_logical_device();
		// Destroy (reverse order of initialization)
		void destroy_logical_device();
		void destroy_surface();
		void destroy_debug_messenger();
		void destroy_vulkan_instance();

	private:
		// Physical Device Support
		bool check_physical_device_features_support();
		bool check_physical_device_queue_families_support();
		bool check_physical_device_extensions_support();
		bool check_physical_device_surface_support();

	private: 
		// Debug Messenger
		enum vulkan_message_type { VERBOSE, INFO, WARN, ERROR };
		static std::vector<uint32_t> s_debug_message_statistics;

		static auto GetDefaultDebuggerMessengerCreateInfo()
		{
			return VkDebugUtilsMessengerCreateInfoEXT
			{
				.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
				.messageSeverity =
				VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
				VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
				VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
				VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
				.messageType =
				VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
				VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
				VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
				.pfnUserCallback = VulkanContext::callback_debug_messenger,
				.pUserData = nullptr
			};
		}

		static VKAPI_ATTR VkBool32 VKAPI_CALL 
		callback_debug_messenger
			(
				VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
				VkDebugUtilsMessageTypeFlagsEXT messageType,
				const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
				void* pUserData
			)
		{
			if (VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT & messageSeverity)
			{
				VulkanContext::s_debug_message_statistics[VERBOSE]++;
			}
			else if (VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT & messageSeverity)
			{
				VulkanContext::s_debug_message_statistics[INFO]++;
				log::info(" [Vulkan]: {}", pCallbackData->pMessage);
			}
			else if (VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT & messageSeverity)
			{
				VulkanContext::s_debug_message_statistics[WARN]++;
				log::warn(" [Vulkan]: {}", pCallbackData->pMessage);
			}
			else if (VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT & messageSeverity)
			{
				VulkanContext::s_debug_message_statistics[ERROR]++;
				log::error(" [Vulkan]: {}", pCallbackData->pMessage);
			}
			else log::critical(" [Vulkan]: Unknow Message Severity {}", messageSeverity);

			return VK_FALSE; // Always return false
		}
	};

}} // namespace Albedo::RHI