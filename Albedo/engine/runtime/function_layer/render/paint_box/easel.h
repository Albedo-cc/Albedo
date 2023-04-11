#pragma once

#include "canvas.h"
#include "palette.h"

#include <runtime/asset_layer/asset_layer_types.h>

namespace Albedo {
namespace Runtime
{
	class SModel;

	class Easel
	{
	public:
		enum Layout // Same order as shaders
		{
			MAX_SET_COUNT = 2,
			// layout(set = 0) Descriptor Set UBO
			set_uniform_buffers							=	0,
				binding_camera_matrics					=	0,
				binding_pbr_parameters					=	1,
				MAX_UNIFORM_BUFFER_COUNT	=	1+1,

			// layout(set = 1) Textures
			set_textures											=	1,
				binding_base_color							=	0,
				MAX_TEXTURE_COUNT					=	1,
		};

	public:
		using CID = size_t; // Canvas ID
		static constexpr CID MAX_CANVAS_COUNT = 2; // MAX IN-FLIGHT FRAMES
		struct Scene
		{
			std::shared_ptr<RHI::VMA::Buffer> vertices;
			std::shared_ptr<RHI::VMA::Buffer> indices;
			//std::vector<std::shared_ptr<RHI::Sampler>> samplers; // Future
			std::vector < std::shared_ptr<RHI::VMA::Image>> textures;

		};

	public:
		// You must get or present canvas via an easel.
		void SetScene(std::shared_ptr<SModel> scene_data);
		Canvas& GetCanvas() throw (RHI::VulkanContext::swapchain_error);
		Palette& GetPalette() { return m_palettes[m_current_canvas]; } // Modify Render Data

		void PresentCanvas(bool switch_canvas = true) throw (RHI::VulkanContext::swapchain_error);

	public:
		Easel() = delete;
		Easel(std::shared_ptr<RHI::VulkanContext> vulkan_context);

	private:
		std::shared_ptr<RHI::VulkanContext> m_vulkan_context;
		std::shared_ptr<RHI::CommandPool> m_command_pool; // Resetable
		std::shared_ptr<RHI::DescriptorPool> m_descriptor_pool;

		Scene m_scene;

		CID m_current_canvas = 0;
		std::vector<Canvas> m_canvases;
		std::vector<Palette> m_palettes;
	};

}} // namespace Albedo::Runtime