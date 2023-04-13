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
		void PresentCanvas(bool switch_canvas = true) throw (RHI::VulkanContext::swapchain_error);

	public:
		Easel() = delete;
		Easel(std::shared_ptr<RHI::VulkanContext> vulkan_context);

	private:
		std::shared_ptr<RHI::VulkanContext> m_vulkan_context;
		std::shared_ptr<RHI::CommandPool> m_command_pool; // Resetable
		std::shared_ptr<RHI::DescriptorPool> m_descriptor_pool;

		CID m_current_canvas = 0;
		std::vector<Canvas> m_canvases;

	private:
		static constexpr size_t MAX_DESCRIPTOR_POOL_SET_COUNT
		{ Easel::MAX_CANVAS_COUNT * Palette::MAX_SET_COUNT };

		static constexpr size_t MAX_DESCRIPTOR_POOL_SIZE_UBO
		{ Easel::MAX_CANVAS_COUNT * Palette::MAX_UNIFORM_BUFFER_COUNT };

		static constexpr size_t MAX_DESCRIPTOR_POOL_SIZE_TEXTURE
		{ Easel::MAX_CANVAS_COUNT * Palette::MAX_TEXTURE_COUNT };
	};

}} // namespace Albedo::Runtime