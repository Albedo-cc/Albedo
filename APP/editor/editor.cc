#include "editor.h"

namespace Albedo{
namespace APP
{

	void
	Editor::
	Initialize()
	{
		sm_main_camera = std::make_shared<GRI::Image>
			(GRI::Image::CreateInfo{
				.aspect = VK_IMAGE_ASPECT_COLOR_BIT,
				.usage  = VK_IMAGE_USAGE_SAMPLED_BIT |
							VK_IMAGE_USAGE_TRANSFER_DST_BIT,
				.format = GRI::GetCurrentRenderTarget()->GetFormat(),
				.extent = {800, 600, 1},
				.mipLevels	 = 1,
				.arrayLayers = 1,
				.samples = VK_SAMPLE_COUNT_1_BIT,
				.tiling  = VK_IMAGE_TILING_OPTIMAL,
			});

		auto commandbuffer = 
			GRI::GetGlobalCommandPool(CommandPoolType_Transient, QueueFamilyType_Graphics)
			->AllocateCommandBuffer({ .level = CommandBufferLevel_Primary });
		commandbuffer->Begin();
		{
			sm_main_camera->ConvertLayout(commandbuffer, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);	
		}
		commandbuffer->End();
		commandbuffer->Submit({});

		UISystem::RegisterUIEvent(new UIEvent
		("Main Camera", []()->void
		{
			GRI::ScreenShot(sm_main_camera);

			ImGui::Begin("Main Camera");
			{
				ImGui::Button("Hello World");
				ImGui::Image(*sm_main_camera, { 800, 600 });
			}
			ImGui::End();
		}));
	}

	void
	Editor::
	Terminate()
	{
		sm_main_camera.reset();
	}

	void
	Editor::
    Tick(std::shared_ptr<GRI::CommandBuffer> commandbuffer)
	{
		UISystem::Process(commandbuffer);
	}
	
}} // namespace Albedo::APP