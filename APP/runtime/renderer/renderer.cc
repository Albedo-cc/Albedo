#include "renderer.h"

#include <Albedo.Core.Log>

#include "renderpasses/background/renderpass.h"
#include "renderpasses/geometry/renderpass.h"
#include "renderpasses/surface/renderpass.h"

#include "../camera/camera.h"

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

			// Update Resources
			update_frame_context(GRI::GetRenderTargetCursor());

			// Render
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

			// Present
			GRI::PresentFrame({ frame.renderpasses.back().semaphore });
		}
		catch (GRI::SIGNAL_RECREATE_SWAPCHAIN)
		{
			Log::Debug("Recreating Swapchain...");
			when_recreate_swapchain();
		}
	}

	const std::unique_ptr<GRI::RenderPass>&
	Renderer::
	SearchRenderPass(std::string_view name) const
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
		m_frames.clear();
		m_global_ubo.reset();
		m_renderpasses.clear();
	}

	void Renderer::when_recreate_swapchain()
	{
		create_renderpasses();
		create_frames();
	}

	void
	Renderer::
	create_decriptor_set_layouts()
	{
		// Add Descriptor Set Layouts
		GRI::RegisterGlobalDescriptorSetLayout(
			"GlobalUBO_Camera",
			GRI::DescriptorSetLayout::Create({
				VkDescriptorSetLayoutBinding
				{
					.binding = 0,
					.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC,
					.descriptorCount = 1,
					.stageFlags = VK_SHADER_STAGE_VERTEX_BIT,

				}
			}));
	}

	void
	Renderer::
	create_renderpasses()
	{
		// System Render Passes
		m_renderpasses.resize(RenderPasses::MAX_RENDERPASS_COUNT);
		m_renderpasses[RenderPasses::Background] = std::make_unique<BackgroundPass>();
		m_renderpasses[RenderPasses::Geometry]	 = std::make_unique<GeometryPass>();

		// User Render Passes
		// ...

		// Sort Render Passes by Priority
		std::sort(m_renderpasses.begin(), m_renderpasses.end(),
			[](const std::unique_ptr<GRI::RenderPass>& a,
			   const std::unique_ptr<GRI::RenderPass>& b)
			->bool
			{
				return a->GetPriority() > b->GetPriority();
			});
	}

	void
	Renderer::
	create_frames()
	{
		if (m_frames.size() != GRI::GetRenderTargetCount())
		{
			m_frames.resize(GRI::GetRenderTargetCount());

			m_global_ubo = GRI::Buffer::Create(
				{
					.size = m_frames.size() * GRI::PadUniformBufferSize(sizeof(GlobalUBO)),
					.usage= VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
					.mode = VK_SHARING_MODE_EXCLUSIVE,
					.properties = static_cast<GRI::Buffer::Property>(
						GRI::Buffer::Property::Persistent |
						GRI::Buffer::Property::Writable),
				});

			std::vector<VkWriteDescriptorSet> writes;
			for (auto& frame : m_frames)
			{
				frame.renderpasses.resize(m_renderpasses.size());
				frame.ubo_descriptor_set = GRI::GetGlobalDescriptorPool()
					->AllocateDescriptorSet(GRI::GetGlobalDescriptorSetLayout("GlobalUBO_Camera"));

				writes.emplace_back(frame.ubo_descriptor_set
						->BindToBuffer(
						0,
						m_global_ubo,
						0, // Do not use static offset, instead, using Dynamic Binding.
						sizeof(GlobalUBO::CameraData)));
			}
			GRI::UpdateDescriptorSets(writes);
		}
	}

	void
	Renderer::
	update_frame_context(uint32_t frame_index)
	{
		auto& current_frame = m_frames[frame_index];
		
		// Update Context
		auto& ctx = m_frame_context;
		ctx.frame_index = frame_index;
		ctx.ubo = current_frame.ubo_descriptor_set;

		// Update Global UBO
		auto& camera = Camera::GetInstance();

		m_global_ubo->Write(&camera.m_matrics,
							sizeof(GlobalUBO::CameraData),
							GRI::PadUniformBufferSize(sizeof(GlobalUBO)) * frame_index);
	}

}} // namespace Albedo::APP