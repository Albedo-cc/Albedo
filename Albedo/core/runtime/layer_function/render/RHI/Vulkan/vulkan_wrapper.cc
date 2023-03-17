#include "vulkan_wrapper.h"

namespace Albedo {
namespace RHI
{
	GraphicsPipeline::GraphicsPipeline(VulkanContext& context):
		m_context{ context }
	{

	}

	GraphicsPipeline::~GraphicsPipeline()
	{
		vkDestroyPipelineLayout(m_context.m_device, pipeline_layout, m_context.m_memory_allocator);
		vkDestroyPipeline(m_context.m_device, pipeline, m_context.m_memory_allocator);
	}

}} // namespace Albedo::RHI