#pragma once

#include "canvas.h"

namespace Albedo {
namespace Runtime
{

	class Easel
	{
	public:
		using CID = size_t; // Canvas ID
		static constexpr CID MAX_CANVAS_COUNT = 2; // MAX IN-FLIGHT FRAMES

	public:
		// You must get or present canvas via an easel.
		Canvas& WaitCanvas() throw (RHI::VulkanContext::swapchain_error);
		void PresentCanvas(std::vector<VkSemaphore> wait_semaphores, bool switch_canvas = true) throw (RHI::VulkanContext::swapchain_error);

	public:
		Easel() = delete;
		Easel(std::shared_ptr<RHI::VulkanContext> vulkan_context);

	private:
		std::shared_ptr<RHI::VulkanContext> m_vulkan_context;

		CID m_current_canvas = 0;
		std::vector<Canvas> m_canvases;
	};

}} // namespace Albedo::Runtime