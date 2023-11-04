#include "renderer.h"

#include <Albedo.Core.Log>

#include "background/renderpass.h"
#include "geometry/renderpass.h"
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
			auto& frame = m_frames[GRI::GetRenderTargetCursor()];
			GRI::WaitNextFrame(frame.semaphore_image_available, VK_NULL_HANDLE);

			for (size_t passidx = 0; passidx < frame.renderpasses.size(); ++passidx)
			{
				auto& renderpass = frame.renderpasses[passidx];
				renderpass.commandbuffer->Begin();
				{
					auto subpass_iter = m_renderpasses[passidx]->Begin(renderpass.commandbuffer);
					{
						do
						{
							subpass_iter.Begin();
							subpass_iter.End();
						} while (subpass_iter.Next());
					}
					m_renderpasses[passidx]->End(renderpass.commandbuffer);
				}
				renderpass.commandbuffer->End();

				GRI::CommandBuffer::SubmitInfo submitinfo
				{
					.wait_stages = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
					.wait_semaphores = {frame.semaphore_image_available},
					.signal_semaphores = {renderpass.semaphore},
				};
				if (passidx) submitinfo.wait_semaphores[0] = frame.renderpasses[passidx - 1].semaphore;

				renderpass.commandbuffer->Submit(submitinfo);
			}

			GRI::PresentFrame({ frame.renderpasses.back().semaphore });
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
		for (const auto& renderpass : m_renderpasses)
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
		GRI::RegisterGlobalDescriptorSetLayout(
			"NULL",
			GRI::DescriptorSetLayout::Create({
				VkDescriptorSetLayoutBinding
				{
					.binding = 0,
					.descriptorCount = 0, // Disable for now
				}
			}));
	}

	void
	Renderer::
	create_renderpasses()
	{
		// System Render Passes
		//m_renderpasses.emplace_back(new BackgroundPass());
		m_renderpasses.emplace_back(new GeometryPass());

		// User Render Passes
		// ...

		// Sort Render Passes by Priority
		std::sort(m_renderpasses.begin(), m_renderpasses.end(),
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
		for (auto& renderpass : m_renderpasses)
		{
			delete renderpass;
		}	
		m_renderpasses.clear();
	}


	void
	Renderer::
	create_frames()
	{
		m_frames.resize(GRI::GetRenderTargetCount());
		for (auto& frame : m_frames)
		{
			frame.renderpasses.resize(m_renderpasses.size());
		}
	}

	void
	Renderer::
	destory_frames()
	{
		m_frames.clear();
	}

}} // namespace Albedo::APP