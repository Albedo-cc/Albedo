#include "renderer.h"

#include <AlbedoCore/Log/log.h>
#include "forward/geometry/renderpass.h"
#include "surface/renderpass.h"

#include <algorithm>

namespace Albedo{
namespace APP
{

	void
	Renderer::
	Tick()
	{
		try
		{
			// Wait for the next frame
			auto& frame = sm_frames[GRI::GetRenderTargetCursor()];
			GRI::WaitNextFrame(frame.semaphore_image_available, VK_NULL_HANDLE);

			frame.commandbuffer_geometry->Begin();
			{
				sm_renderpasses[Geometry]->Begin(frame.commandbuffer_geometry);
				sm_renderpasses[Geometry]->End(frame.commandbuffer_geometry);
			}
			frame.commandbuffer_geometry->End();
			frame.commandbuffer_geometry->Submit(
				{
					.wait_stages	   = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
					.wait_semaphores   = {frame.semaphore_image_available},
					.signal_semaphores = {frame.semaphore_geometry_pass},	
				});

			frame.commandbuffer_surface->Begin();
			{
				sm_renderpasses[Surface]->Begin(frame.commandbuffer_surface);
				sm_renderpasses[Surface]->End(frame.commandbuffer_surface);
			}
			frame.commandbuffer_surface->End();
			frame.commandbuffer_surface->Submit(
				{
					.wait_stages	   = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
					.wait_semaphores   = {frame.semaphore_geometry_pass},
					.signal_semaphores = {frame.semaphore_surface_pass},	
				});

			GRI::PresentFrame({ frame.semaphore_surface_pass });
		}
		catch (GRI::SIGNAL_RECREATE_SWAPCHAIN)
		{
			Log::Debug("Recreating Swapchain...");
			destory_renderpasses();
			create_renderpasses();
		}
	}

	const GRI::RenderPass*
	Renderer::SearchRenderPass(std::string_view name)
	throw(std::runtime_error)
	{
		for (const auto& renderpass : sm_renderpasses)
		{
			if (renderpass->GetName() == name) return renderpass;
		}
		Log::Error("Failed to find renderpass({})!", name.data());
	}

	void
	Renderer::
	Initialize()
	{
		create_decriptor_set_layouts();
		create_renderpasses();
		create_frames();
	}

	void
	Renderer::
	Destroy()
	{
		GRI::WaitDeviceIdle();
		destory_renderpasses();
		destory_frames();
	}

	void
	Renderer::
	create_decriptor_set_layouts()
	{
				// Add Descriptor Set Layouts
		GRI::CreateGlobalDescriptorSetLayout(
			"NULL",
			{
				VkDescriptorSetLayoutBinding
				{
					.binding = 0,
					.descriptorCount = 0, // Disable for now
				}
			});
	}

	void
	Renderer::
	create_renderpasses()
	{
		sm_renderpasses.emplace_back(new GeometryPass());
		sm_renderpasses.emplace_back(new SurfacePass());

		// Sort Render Passes by Priority
		std::sort(sm_renderpasses.begin(), sm_renderpasses.end(),
			[](const GRI::RenderPass* a,
			   const GRI::RenderPass* b)
			->bool
			{
				return a->GetPriority() > b->GetPriority();
			});
	}

	void
	Renderer::
	destory_renderpasses()
	{
		for (auto& renderpass : sm_renderpasses)
		{
			delete renderpass;
		}	
		sm_renderpasses.clear();
	}


	void
	Renderer::
	create_frames()
	{
		sm_frames.resize(GRI::GetRenderTargetCount());
		for (auto& frame : sm_frames)
		{
			frame.commandbuffer_geometry = 
				GRI::GetGlobalCommandPool(CommandPoolType_Resettable, QueueFamilyType_Graphics)
				->AllocateCommandBuffer({ .level = CommandBufferLevel_Primary });

			frame.commandbuffer_surface = 
				GRI::GetGlobalCommandPool(CommandPoolType_Resettable, QueueFamilyType_Graphics)
				->AllocateCommandBuffer({ .level = CommandBufferLevel_Primary });
		}
	}

	void
	Renderer::
	destory_frames()
	{
		sm_frames.clear();
	}

}} // namespace Albedo::APP