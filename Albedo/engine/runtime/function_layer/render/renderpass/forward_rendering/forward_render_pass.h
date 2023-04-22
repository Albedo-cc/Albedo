#pragma once

#include <AlbedoRHI.hpp>

namespace Albedo {
namespace Runtime
{

	class ForwardRenderPass final :
		public RHI::RenderPass
	{
		enum Attachments : uint32_t
		{
			attachment_present_color,
			attachment_depth_stencil,

			MAX_ATTACHMENT_COUNT
		};

		enum Subpasses : uint32_t
		{
			subpass_present,

			MAX_SUBPASS_COUNT
		};

	public:
		enum Pipelines : uint32_t
		{
			pipeline_present,

			MAX_PIPELINE_COUNT
		};

	public:
		ForwardRenderPass() = delete;
		ForwardRenderPass(std::shared_ptr<RHI::VulkanContext> vulkan_context);

	private:
		virtual std::vector<VkClearValue>	set_attachment_clear_colors() override;
		virtual std::vector<VkSubpassDependency> set_subpass_dependencies() override;

		virtual void create_attachments() override;
		virtual void create_subpasses() override;
		virtual void create_framebuffers() override;
		virtual void create_pipelines() override;
	};
	
}} // namespace Albedo::Runtime