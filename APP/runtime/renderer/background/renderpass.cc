#include "renderpass.h"

#include "pipelines/skybox.h"

namespace Albedo{
namespace APP
{

	BackgroundPass::
	BackgroundPass() : GRI::RenderPass{"Background", 10000}
	{
		BEGIN_BUILD();
		{
			// Add Attachments (From SystemTarget::MAX_SYSTEM_TARGET)
		
			// Add Subpasses
			// [0]: Triangles
			add_subpass(GRI::RenderPass::SubpassSetting
				{
					.name					 = "Background::Skybox",
					.pipeline				 = std::make_unique<SkyboxPipeline>(),
					.color_attachments		 = {get_system_target_reference(ST_Color)},
					//.depth_stencil_attachment= get_system_target_reference(ST_ZBuffer),
					.source_stage_mask		 = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT |
											   VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
					.source_access_mask		 = 0x0,
					.destination_stage_mask	 = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
					.destination_access_mask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
				});
		}
		END_BUILD();
	}
	
}} // namespace Albedo::APP