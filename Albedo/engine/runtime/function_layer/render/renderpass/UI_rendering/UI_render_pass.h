#pragma once

#include <AlbedoRHI.hpp>

namespace Albedo {
namespace Runtime
{

	class UIRenderPass : public RHI::RenderPass
	{
		enum Attachments : uint32_t
		{
			attachment_UI_color,

			MAX_ATTACHMENT_COUNT
		};

	public:
		enum Subpasses : uint32_t
		{
			subpass_UI,

			MAX_SUBPASS_COUNT
		};

		UIRenderPass() = delete;
		UIRenderPass(std::shared_ptr<RHI::VulkanContext> vulkan_context);

	private:
		virtual std::vector<VkClearValue>	set_attachment_clear_colors() override;
		virtual std::vector<VkSubpassDependency> set_subpass_dependencies() override;

		virtual void create_attachments() override;
		virtual void create_subpasses() override;
		virtual void create_framebuffers() override;
		virtual void create_pipelines() override;
	};

}} // namespace Albedo::Runtime