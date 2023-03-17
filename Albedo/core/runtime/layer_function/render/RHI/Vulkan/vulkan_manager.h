#pragma once

#include "vulkan_context.h"
#include "vulkan_wrapper.h"

#include <string_view>
#include <fstream>
#include <optional>
#include <unordered_map>

namespace Albedo {
namespace RHI
{
	class VulkanManager
	{
	public:
		const VulkanContext& GetVulkanContext() const { return m_context; }

		GraphicsPipeline::Builder BuildGraphicsPipeline();

	public:
		VulkanManager() = delete; // Not Support
		VulkanManager(GLFWwindow* window);
		~VulkanManager();

	private:
		VulkanContext m_context;

	private:
		VkShaderModule create_shader_module(std::string_view shader_file);
	};

}} // namespace Albedo::RHI