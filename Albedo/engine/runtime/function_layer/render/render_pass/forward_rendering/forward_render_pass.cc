#include "forward_render_pass.h"

#include "pipelines/present_pipeline.h"

namespace Albedo {
namespace Runtime
{
	void ForwardRenderPass::Render(std::shared_ptr<RHI::CommandBuffer> command_buffer)
	{
		assert(command_buffer->IsRecording() && "You cannot Render() before beginning the command buffer!");

		for (auto& graphics_pipeline : m_graphics_pipelines)
		{
			graphics_pipeline->Draw(command_buffer);
		}
	}

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

	std::vector<VkSubpassDependency> ForwardRenderPass::
		set_subpass_dependencies()
	{
		std::vector<VkSubpassDependency> dependencies(MAX_SUBPASS_COUNT);
		// Present Subpass Dependency
		auto& present = dependencies[subpass_present];
		present.srcSubpass = VK_SUBPASS_EXTERNAL; // Implicit subpass (First subpass set in srcSubpass and Last subpass set in dstSubpass)
		present.dstSubpass = 0; // Must higher than srcSubpass, 0 refers to this subpass is the first and only one
		present.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT; // The stages to start subpass
		present.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT; // The stages to complete
		present.srcAccessMask = 0;
		present.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT; // Writable

		return dependencies;
	}

	void ForwardRenderPass::create_pipelines()
	{
		m_graphics_pipelines.resize(MAX_PIPELINE_COUNT);
		// Present Pipeline
		auto& present = m_graphics_pipelines[pipeline_present];
		present = std::make_unique<PresentPipeline>(m_context, m_render_pass, subpass_present);
	}
	
}} // namespace Albedo::Runtime