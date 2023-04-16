#include "UI_render_pass.h"

namespace Albedo {
namespace Runtime
{

	UIRenderPass::UIRenderPass(std::shared_ptr<RHI::VulkanContext> vulkan_context) :
		RHI::RenderPass{ vulkan_context }
	{
		Initialize();
	}

	std::vector<VkClearValue>	UIRenderPass::
		set_attachment_clear_colors()
	{
		return std::vector<VkClearValue> // // Note that the order of clearValues should be identical to the order of your attachments.
		{
			VkClearValue // 1. Color Attachment
			{
				.color = { {0.0,0.0,0.0,1.0} }
			}
		};
	}

	std::vector<VkSubpassDependency> UIRenderPass::
		set_subpass_dependencies()
	{
		std::vector<VkSubpassDependency> dependencies(MAX_SUBPASS_COUNT);
		// UI Subpass Dependency
		auto& ui = dependencies[subpass_UI];
		ui.srcSubpass = VK_SUBPASS_EXTERNAL; // Implicit subpass (First subpass set in srcSubpass and Last subpass set in dstSubpass)
		ui.dstSubpass = 0; // Must higher than srcSubpass, 0 refers to this subpass is the first and only one
		ui.srcStageMask = // The stages to start subpass
			VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		ui.dstStageMask =  // The stages to complete
			VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		ui.srcAccessMask = 0;
		ui.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

		return dependencies;
	}

	void UIRenderPass::create_attachments()
	{
		m_attachment_descriptions.resize(MAX_ATTACHMENT_COUNT);
		m_attachment_references.resize(MAX_ATTACHMENT_COUNT);
		// UI Attachment
		{
			auto& ui = m_attachment_descriptions[attachment_UI_color];
			ui.format = m_context->m_swapchain_image_format;
			ui.samples = VK_SAMPLE_COUNT_1_BIT;
			ui.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD; // [M]
			ui.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
			ui.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			ui.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
			ui.initialLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR; // [M]
			ui.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

			auto& present_ref = m_attachment_references[attachment_UI_color];
			present_ref.attachment = attachment_UI_color;
			present_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		}
	}

	void UIRenderPass::create_subpasses()
	{
		m_subpass_descriptions.resize(MAX_SUBPASS_COUNT);
		// UI Subpass
		auto& ui = m_subpass_descriptions[subpass_UI];
		ui.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		ui.colorAttachmentCount = 1;
		ui.pColorAttachments = &m_attachment_references[attachment_UI_color];
	}

	void UIRenderPass::create_framebuffers()
	{
		auto& extent = m_context->m_swapchain_current_extent;
		for (size_t i = 0; i < m_context->m_swapchain_imageviews.size(); ++i)
		{
			std::vector<VkImageView> attachments{ m_context->m_swapchain_imageviews[i] };
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

	void UIRenderPass::create_pipelines()
	{
		// Created By Dear ImGUI
	}

}} // namespace Albedo::Runtime