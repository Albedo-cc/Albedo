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

	void ForwardRenderPass::create_framebuffers()
	{
		auto& extent = m_context->m_swapchain_current_extent;
		for (size_t i = 0; i < m_context->m_swapchain_imageviews.size(); ++i)
		{
			std::vector<VkImageView> attachments{ m_context->m_swapchain_imageviews[i],
																					  m_context->m_swapchain_depth_stencil_image->GetImageView() };
			VkFramebufferCreateInfo framebufferCreateInfo
			{
				.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
				.renderPass = m_render_pass,
				.attachmentCount = static_cast<uint32_t>(attachments.size()),
				.pAttachments = attachments.data(),
				.width = extent.width,
				.height = extent.height,
				.layers = 1
			};

			auto& framebuffer = m_framebuffers.emplace_back();
			if (vkCreateFramebuffer(
				m_context->m_device,
				&framebufferCreateInfo,
				m_context->m_memory_allocation_callback,
				&framebuffer) != VK_SUCCESS)
				throw std::runtime_error("Failed to create the Vulkan Framebuffer!");
		}
	}

	void ForwardRenderPass::create_attachments()
	{
		m_attachment_descriptions.resize(MAX_ATTACHMENT_COUNT);
		m_attachment_references.resize(MAX_ATTACHMENT_COUNT);
		// Present Attachment
		{
			auto& present = m_attachment_descriptions[attachment_present_color];
			present.format = m_context->m_swapchain_image_format;
			present.samples = VK_SAMPLE_COUNT_1_BIT;
			present.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
			present.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
			present.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			present.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
			present.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			present.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

			auto& present_ref = m_attachment_references[attachment_present_color];
			present_ref.attachment = attachment_present_color;
			present_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		}
		// Depth Stencil Attachment
		{
			auto& depthStencil = m_attachment_descriptions[attachment_depth_stencil];
			depthStencil.format = m_context->m_swapchain_depth_stencil_format;
			depthStencil.samples = VK_SAMPLE_COUNT_1_BIT;
			depthStencil.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
			depthStencil.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
			depthStencil.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			depthStencil.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
			depthStencil.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED; // Do not care about
			depthStencil.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

			auto& depthStencil_ref = m_attachment_references[attachment_depth_stencil];
			depthStencil_ref.attachment = attachment_depth_stencil;
			depthStencil_ref.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
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
		present.pDepthStencilAttachment = &m_attachment_references[attachment_depth_stencil]; // A subpass can only use a single depth (+stencil) attachment.
	}

	std::vector<VkClearValue> ForwardRenderPass::
		set_attachment_clear_colors()
	{
		return std::vector<VkClearValue> // // Note that the order of clearValues should be identical to the order of your attachments.
		{
			VkClearValue // 1. Color Attachment
			{
				.color = { {0.0,0.0,0.0,1.0} }
			},
			VkClearValue // 2. Depth Stencil Attachment
			{
				.depthStencil = {1.0, 0} // The range of depths in the depth buffer is 0.0 to 1.0 in Vulkan
			}
		};
	}

	std::vector<VkSubpassDependency> ForwardRenderPass::
		set_subpass_dependencies()
	{
		std::vector<VkSubpassDependency> dependencies(MAX_SUBPASS_COUNT);
		// Present Subpass Dependency
		auto& present = dependencies[subpass_present];
		present.srcSubpass = VK_SUBPASS_EXTERNAL; // Implicit subpass (First subpass set in srcSubpass and Last subpass set in dstSubpass)
		present.dstSubpass = 0; // Must higher than srcSubpass, 0 refers to this subpass is the first and only one
		present.srcStageMask = // The stages to start subpass
			VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | 
			VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT; // For Depth Test
		present.dstStageMask =  // The stages to complete
			VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT |
			VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT; // For Depth Test
		present.srcAccessMask = 0;
		present.dstAccessMask = 
			VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT |
			VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT; // For Depth Test

		return dependencies;
	}

	void ForwardRenderPass::create_pipelines()
	{
		m_graphics_pipelines.resize(MAX_PIPELINE_COUNT);
		// Present Pipeline
		auto& present = m_graphics_pipelines[pipeline_present];
		present = new PresentPipeline(m_context, m_render_pass, subpass_present);
	}
	
}} // namespace Albedo::Runtime