#pragma once

#include "vulkan_context.h"

namespace Albedo {
namespace RHI
{

	class VulkanManager
	{
	public:
		VulkanManager() = delete; // Not Support
		VulkanManager(GLFWwindow* window);
		~VulkanManager();

	private:
		VulkanContext					m_context;

	private:

	};

}} // namespace Albedo::RHI