#pragma once

#include "vulkan_manager.h"

namespace Albedo {
namespace RHI
{
	// Wrapper List
	class GraphicsPipeline;

	// Implementation
	class GraphicsPipeline
	{
	public:
		class Builder
		{
			friend class VulkanManager;
			Builder() = delete;
			Builder(VulkanContext& context);
		public:


			std::shared_ptr<GraphicsPipeline> Create();
		private:
			std::shared_ptr<GraphicsPipeline> m_graphics_pipeline;
		};
	public:
		VkPipeline pipeline = VK_NULL_HANDLE;
		VkPipelineLayout pipeline_layout = VK_NULL_HANDLE;

	private:
		friend class GraphicsPipeline::Builder;
		GraphicsPipeline() = delete;
		GraphicsPipeline(VulkanContext& context);
		~GraphicsPipeline();
	private:
		VulkanContext& m_context;
	};
	

}} // namespace Albedo::RHI