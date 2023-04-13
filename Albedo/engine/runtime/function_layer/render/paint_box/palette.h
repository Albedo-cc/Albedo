#pragma once

#include <AlbedoRHI.hpp>

#include <core/math/math.h>

namespace Albedo {
namespace Runtime
{

	class Palette
	{
		friend class Scene;
		friend class Easel;
		friend class Canvas;
	private:
		enum Layout // Same order as shaders
		{
			MAX_SET_COUNT = 2,
			// layout(set = 0) Descriptor Set UBO
			set_uniform_buffers							=	0,
				binding_camera_matrics					=	0,
				binding_light_parameters				=	1,
				MAX_UNIFORM_BUFFER_COUNT	=	1 + 1,

			// layout(set = 1) Textures
			set_textures											=	1,
				binding_pbr_base_color					=	0,
				MAX_TEXTURE_COUNT					=	1,
		};

	public:
		// SET 0
		void SetupCameraMatrics(std::shared_ptr<RHI::VMA::Buffer> matrics);
		void SetupLightParameters(std::shared_ptr<RHI::VMA::Buffer> light_parameters);
		// SET 1
		void SetupPBRBaseColor(std::shared_ptr<RHI::VMA::Image> base_color);

	private:
		std::shared_ptr<RHI::DescriptorSet>		SET0_ubo;
		std::shared_ptr<RHI::DescriptorSet>		SET1_texture;

	private:
		void initialize(std::shared_ptr<RHI::VulkanContext> vulkan_context, std::shared_ptr<RHI::DescriptorPool> descriptorPool);
	};


}} // namespace Albedo::Runtime