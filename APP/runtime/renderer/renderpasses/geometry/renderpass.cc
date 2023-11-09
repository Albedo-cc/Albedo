#include "renderpass.h"

#include "pipelines/triangles.h"
#include "pipelines/lines.h"
#include "pipelines/points.h"

namespace Albedo{
namespace APP
{

	GeometryPass::
	GeometryPass() : RenderPass{"Geometry", 3000}
	{
		BEGIN_BUILD(ZWrite);
		{
			// Add Attachments (From SystemTarget::MAX_SYSTEM_TARGET)
		
			// Set Framebuffers
			// [!---FIXING---!]
		
			// Add Subpasses
			// [0]: Triangles
			add_subpass(RenderPass::SubpassSetting
				{
					.name					 = "Geometry::Triangles",
					.pipeline				 = std::make_unique<TrianglesPipeline>(),
					.color_attachments		 = {get_system_target_reference(ST_Color)},
					.depth_stencil_attachment= get_system_target_reference(ST_ZBuffer),
					.source_stage_mask		 = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT |
											   VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
					.source_access_mask		 = 0x0,
					.destination_stage_mask	 = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT |
											   VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
					.destination_access_mask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT |
											   VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
				});
			// [1]: Lines
			/*add_subpass(RenderPass::SubpassSetting
				{
					.name					 = "Geometry::Lines",
					.pipeline				 = std::make_unique<LinesPipeline>(),
					.color_attachments		 = {get_system_target_reference(ST_Color)},
					.depth_stencil_attachment= get_system_target_reference(ST_ZBuffer),
					.source_stage_mask		 = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT |
											   VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
					.source_access_mask		 = 0x0,
					.destination_stage_mask	 = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT |
											   VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
					.destination_access_mask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT |
											   VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
				});*/
			// [2]: Points
			/*add_subpass(RenderPass::SubpassSetting
				{
					.name					 = "Geometry::Points",
					.pipeline				 = std::make_unique<PointsPipeline>(),
					.color_attachments		 = {get_system_target_reference(ST_Color)},
					.depth_stencil_attachment= get_system_target_reference(ST_ZBuffer),
					.source_stage_mask		 = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT |
											   VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
					.source_access_mask		 = 0x0,
					.destination_stage_mask	 = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT |
											   VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
					.destination_access_mask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT |
											   VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
				});*/
		}
		END_BUILD();
	}
	
}} // namespace Albedo::APP