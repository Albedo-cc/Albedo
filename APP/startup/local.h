#pragma once

#include <vulkan/vulkan.h>

#include <Albedo.Core.Log>

namespace
{
	using namespace Albedo;

    // RHI Messenger
	VKAPI_ATTR VkBool32 VKAPI_CALL
		messenger_callback(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserData)
	{
		switch (messageSeverity)
		{
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
			break;
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
			Log::Info("{}", pCallbackData->pMessage);
			break;
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
			Log::Warn("{}", pCallbackData->pMessage);
			break;
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
			Log::Error("{}", pCallbackData->pMessage);
			break;
		default:
			Log::Fatal("{}", "Unknow Message Severity");
		}
		return VK_FALSE; // Always return VK_FALSE
	}
}