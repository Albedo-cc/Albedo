#include "renderpass.h"

#include "pipelines/present.h"

namespace Albedo{
namespace APP
{
	SurfacePass::
	SurfacePass() : GRI::RenderPass{ "Surface", 0 }
	{
		// Add Attachments (From SystemTarget::MAX_SYSTEM_TARGET)
		
		// Set Framebuffers
		// [!---FIXING---!]
		
		// Add Subpasses
		// [0]: Present
		add_subpass(GRI::RenderPass::SubpassSetting
			{
				.name					 = "Surface::Present",
				.pipeline				 = std::make_unique<PresentPipeline>(),
				.color_attachments		 = {get_system_target_reference(ST_Color)},
				//.depth_stencil_attachment= get_system_target_reference(ST_ZBuffer),
				.source_stage_mask		 = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
				.source_access_mask		 = 0x0,
				.destination_stage_mask	 = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
				.destination_access_mask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
			});

		// Build Render Pass
		BUILD_ALL();
	}
	
}} // namespace Albedo::APP