#include "forward_render_pass.h"

#include "pipelines/present_pipeline.h"

namespace Albedo {
namespace Runtime
{

	ForwardRenderPass::ForwardRenderPass(std::shared_ptr<RHI::VulkanContext> vulkan_context):
		RHI::RenderPass{ vulkan_context }
	{
		Initialize();
	}

	void ForwardRenderPass::create_attachments()
	{
		m_attachment_descriptions.resize(MAX_ATTACHMENT_COUNT);
		m_attachment_references.resize(MAX_ATTACHMENT_COUNT);
		// Present Attachment
		auto& present = m_attachment_descriptions[attachment_present_color];
		present.format					= m_context->m_swapchain_image_format;
		present.samples				= VK_SAMPLE_COUNT_1_BIT;
		present.loadOp					= VK_ATTACHMENT_LOAD_OP_CLEAR;
		present.storeOp				= VK_ATTACHMENT_STORE_OP_STORE;
		present.stencilLoadOp	= VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		present.stencilStoreOp	= VK_ATTACHMENT_STORE_OP_DONT_CARE;
		present.initialLayout		= VK_IMAGE_LAYOUT_UNDEFINED;
		present.finalLayout			= VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
		auto& present_ref = m_attachment_references[attachment_present_color];
		present_ref.attachment = attachment_present_color;
		present_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	}

	void ForwardRenderPass::create_frame_buffers()
	{
		m_frame_buffers.resize(m_context->m_swapchain_imageviews.size());

		for (size_t i = 0; i < m_context->m_swapchain_imageviews.size(); ++i)
		{
			VkImageView attachments[] = { m_context->m_swapchain_imageviews[i] };

			VkFramebufferCreateInfo framebufferCreateInfo
			{
				.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
				.renderPass = m_render_pass,
				.attachmentCount = static_cast<uint32_t>(m_attachment_descriptions.size()),
				.pAttachments = attachments,
				.width = m_context->m_swapchain_current_extent.width,
				.height = m_context->m_swapchain_current_extent.height,
				.layers = 1
			};
		}
	}

	void ForwardRenderPass::create_subpasses()
	{
		m_subpass_descriptions.resize(MAX_SUBPASS_COUNT);
		// Present Subpass
		auto& present = m_subpass_descriptions[subpass_present];
		present.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		present.colorAttachmentCount = 1;
		present.pColorAttachments = &m_attachment_references[attachment_present_color];
	}

	void ForwardRenderPass::create_pipelines()
	{
		m_graphics_pipelines.resize(MAX_PIPELINE_COUNT);
		// Present Pipeline
		auto& present = m_graphics_pipelines[pipeline_present];
		present = std::make_unique<PresentPipeline>(m_context, m_render_pass, subpass_present);
	}
	
}} // namespace Albedo::Runtime