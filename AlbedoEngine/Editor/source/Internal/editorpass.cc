#include "editorpass.h"

#include <imgui.h>
#include <Albedo/Core/Log/log.h>
#include <Albedo/Core/Norm/assert.h>
#include <Albedo/Core/Norm/assert.h>
#include <Albedo/Graphics/Internal/Vulkan.h>

#include "../editor.h"
#include "pipelines/dearimgui.h"

namespace Albedo
{

	EditorPass::
	EditorPass() : RenderPass{ "Editor", 0 }
	{
		auto color_index = add_attachment(AttachmentSetting
			{
				.description
				{
					.format			= g_vk->swapchain.format,
					.samples		= VK_SAMPLE_COUNT_1_BIT,
					.loadOp			= VK_ATTACHMENT_LOAD_OP_CLEAR,
					.storeOp		= VK_ATTACHMENT_STORE_OP_STORE,
					.stencilLoadOp	= VK_ATTACHMENT_LOAD_OP_DONT_CARE,
					.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
					.initialLayout	= VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
					.finalLayout	= VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
				},
				.clearColor{{0.01,0.01,0.01, 1.0}}, // Dark Gray
			});

		uint32_t subpassidx = 0;
		subpassidx = add_subpass(SubpassSetting
			{
				.name					 = "Editor::ImGui",
				.pipeline				 = std::make_unique<ImGuiPipeline>(),
				.color_attachments		 = {{color_index, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL}},
				//.depth_stencil_attachment= get_system_target_reference(ST_ZBuffer),
				.source_stage_mask		 = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
				.source_access_mask		 = 0x0,
				.destination_stage_mask	 = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
				.destination_access_mask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
			});
		ALBEDO_ASSERT(Subpass::ImGui == subpassidx);

		ALBEDO_ASSERT(VK_SUBPASS_EXTERNAL == uint32_t(-1));
		std::vector<VkSubpassDescription> subpass_descriptions(m_subpasses.size());
		std::vector<VkSubpassDependency>  subpass_dependencies(m_subpasses.size());
		for (uint32_t i = 0; i < subpass_dependencies.size(); ++i)
		{
			// Subpass Descriptions
			subpass_descriptions[i].pipelineBindPoint	= VK_PIPELINE_BIND_POINT_GRAPHICS;
			subpass_descriptions[i].inputAttachmentCount= m_subpasses[i].input_attachments.size();
			subpass_descriptions[i].pInputAttachments	= m_subpasses[i].input_attachments.data();
			subpass_descriptions[i].colorAttachmentCount= m_subpasses[i].color_attachments.size();
			subpass_descriptions[i].pColorAttachments	= m_subpasses[i].color_attachments.data();
			subpass_descriptions[i].pDepthStencilAttachment = m_subpasses[i].depth_stencil_attachment.has_value()?
				&m_subpasses[i].depth_stencil_attachment.value() : VK_NULL_HANDLE;
			subpass_descriptions[i].flags = 0x0;
			// Subpass Dependencies
			subpass_dependencies[i].srcSubpass = i - 1;
			subpass_dependencies[i].dstSubpass = i;
			subpass_dependencies[i].srcStageMask  = m_subpasses[i].source_stage_mask;
			subpass_dependencies[i].srcAccessMask = m_subpasses[i].source_access_mask;
			subpass_dependencies[i].dstStageMask  = m_subpasses[i].destination_stage_mask;
			subpass_dependencies[i].dstAccessMask = m_subpasses[i].destination_access_mask;
			subpass_dependencies[i].dependencyFlags = 0x0;
		}

		VkRenderPassCreateInfo renderPassCreateInfo
		{
			.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
			.attachmentCount = static_cast<uint32_t>(m_attachments.descriptions.size()),
			.pAttachments	 = m_attachments.descriptions.data(),
			.subpassCount	 = static_cast<uint32_t>(subpass_descriptions.size()),
			.pSubpasses		 = subpass_descriptions.data(),
			.dependencyCount = static_cast<uint32_t>(subpass_dependencies.size()),
			.pDependencies	 = subpass_dependencies.data()
		};

		if (vkCreateRenderPass(
			g_vk->device,
			&renderPassCreateInfo,
			g_vk->allocator,
			&m_handle) != VK_SUCCESS)
			Log::Fatal("Failed to create the Albedo::Editor Render Pass!");

		// Create Framebuffers			
		m_framebuffers.resize(g_vk->swapchain.images.size());
		
		for (size_t i = 0; i < m_framebuffers.size(); ++i)
		{
			auto& framebuffer = m_framebuffers[i];

			framebuffer.render_targets.emplace_back(g_vk->swapchain.image_views[i]);

			VkFramebufferCreateInfo framebufferCreateInfo
			{
				.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
				.renderPass = m_handle,
				.attachmentCount = static_cast<uint32_t>(framebuffer.render_targets.size()),
				.pAttachments = framebuffer.render_targets.data(),
				.width  = g_vk->swapchain.extent.width,
				.height = g_vk->swapchain.extent.height,
				.layers = 1
			};

			if (vkCreateFramebuffer(
				g_vk->device,
				&framebufferCreateInfo,
				g_vk->allocator,
				&framebuffer.handle) != VK_SUCCESS)
				Log::Fatal("Failed to create the Vulkan Framebuffer!");
		}
	}

} // namespace Albedo