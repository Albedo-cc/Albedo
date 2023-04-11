#pragma once

#include <AlbedoRHI.hpp>

#include "../scene/model.h" // TEST

#include "palette.h"

namespace Albedo {
namespace Runtime
{

	class Canvas // One Frame
	{
		friend class Easel;
	public:
		std::shared_ptr<RHI::CommandBuffer>	BeginPainting(std::shared_ptr<RHI::RenderPass> renderPass);
		void																	Paint(RHI::GraphicsPipeline* brush, TempModel& scene); // TEST
		void																	EndPainting(std::shared_ptr<RHI::RenderPass> renderPass);
		
		Palette& GetPalette() { return m_palette; } // Modify Render Data

	public:
		Canvas() = default; // Created and initialized by the Easel

		struct SyncMeta
		{
			std::unique_ptr<RHI::Fence>			fence_in_flight;
			std::unique_ptr<RHI::Semaphore>	semaphore_image_available;
			std::unique_ptr<RHI::Semaphore>	semaphore_render_finished;
		};
		SyncMeta syncmeta; // Used by Easel

	private:
		std::shared_ptr<RHI::VulkanContext> m_vulkan_context;
		std::shared_ptr<RHI::CommandBuffer> m_command_buffer;
		Palette m_palette;
	};
	

}} // namespace Albedo::Runtime