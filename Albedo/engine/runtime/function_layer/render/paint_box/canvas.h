#pragma once

#include <AlbedoRHI.hpp>

namespace Albedo {
namespace Runtime
{

	class Canvas // One Frame
	{
		friend class Easel;
	public:
		std::shared_ptr<RHI::CommandBuffer> BeginPainting() 
		{ 
			m_command_buffer->Begin();
			return m_command_buffer; 
		}
		void EndPainting()
		{
			m_command_buffer->End();
			m_command_buffer->Submit(false, *syncmeta.fence_in_flight,
				{ *syncmeta.semaphore_image_available },
				{ *syncmeta.semaphore_render_finished },
				VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT);
		}

	private:
		Canvas() = default; // Created and initialized by the Easel

	private:
		std::shared_ptr<RHI::CommandBuffer> m_command_buffer;

		struct SyncMeta
		{
			std::unique_ptr<RHI::Fence>			fence_in_flight;
			std::unique_ptr<RHI::Semaphore>	semaphore_image_available;
			std::unique_ptr<RHI::Semaphore>	semaphore_render_finished;
		};
		SyncMeta syncmeta; // Used by Easel
	};
	

}} // namespace Albedo::Runtime