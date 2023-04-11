#include "canvas.h"

#include <runtime/asset_layer/asset_manager.h>

namespace Albedo {
namespace Runtime
{

	std::shared_ptr<RHI::CommandBuffer> Canvas::
		BeginPainting(std::shared_ptr<RHI::RenderPass> renderPass)
	{
		m_command_buffer->Begin();
		renderPass->Begin(m_command_buffer);
		return m_command_buffer;
	}

	void Canvas::Paint(RHI::GraphicsPipeline* brush, TempModel& scene)
	{
		brush->Bind(m_command_buffer);

		// TEST (IMAGE)
		static auto sampler = m_vulkan_context->CreateSampler(VK_SAMPLER_ADDRESS_MODE_REPEAT);
		static auto image_data = AssetManager::instance().LoadTexture2D("watch_tower_512x512.png");

		static bool k = false;
		static auto m_image = m_vulkan_context->m_memory_allocator->
			AllocateImage(VK_IMAGE_ASPECT_COLOR_BIT,
				VK_IMAGE_USAGE_SAMPLED_BIT,
				image_data->width, image_data->height, image_data->channel,
				VK_FORMAT_R8G8B8A8_SRGB);
		if (!k)
		{
			k = true;
			m_image->Write(image_data->data);
			m_image->BindSampler(sampler);
		}

		// Update UBO (set = 0)
		m_palette.S0B0_camera_matrics->Write(&m_palette.GetCameraMatrics());
		m_palette.SET0_ubo->WriteBuffer(
			VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
			Palette::binding_camera_matrics,
			m_palette.S0B0_camera_matrics);

		// Update Textures (set = 0) [TEST]-=>S"DL:S"DLadwfghewafsdrsfgh
		m_palette.SET0_ubo->WriteImage(
			VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
			1,
			m_image);

		std::vector<VkDescriptorSet> boundSets
		{
			*m_palette.SET0_ubo // set = 0 (UBO)
		};

		vkCmdBindDescriptorSets(*m_command_buffer, 
			VK_PIPELINE_BIND_POINT_GRAPHICS, 
			brush->GetPipelineLayout(), 
			0, boundSets.size(), boundSets.data(),
			0, nullptr);

		scene.Draw(*m_command_buffer);
	}

	void Canvas::EndPainting(std::shared_ptr<RHI::RenderPass> renderPass)
	{
		renderPass->End(m_command_buffer);
		m_command_buffer->End();
		m_command_buffer->Submit(false, *syncmeta.fence_in_flight,
			{ *syncmeta.semaphore_image_available },
			{ *syncmeta.semaphore_render_finished },
			VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT);
	}

}} // namespace Albedo::Runtime