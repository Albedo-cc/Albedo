#pragma once

#include <AlbedoRHI.hpp>

#include <core/math/math.h>

namespace Albedo {
namespace Runtime
{

	class Palette
	{
		friend class Easel;
		friend class Canvas;
	private:
		enum Layout // Same order as shaders
		{
			// Push Constant (glTF Local Matrix)
			// layout(set = 0) Descriptor Set UBO
			SET_UBO_MAX_BINGING					=	2,
			set_uniform_buffers							=	0,
				binding_camera_matrics					=	0,
				binding_light_parameters				=	1,

			// layout(set = 1) Material
			SET_MATERIALS_MAX_BINGING		=	1,
			set_materials										=	1,
				binding_pbr_base_color					=	0,
		};

	public:
		// SET 0
		inline static std::shared_ptr<RHI::DescriptorSetLayout> SET0_UBO_Layout;
		static void SetupCameraMatrics(std::shared_ptr<RHI::DescriptorSet> SET0_ubo, std::shared_ptr<RHI::VMA::Buffer> matrics);
		static void SetupLightParameters(std::shared_ptr<RHI::DescriptorSet> SET0_ubo, std::shared_ptr<RHI::VMA::Buffer> light_parameters);
		static void BindDescriptorSetUBO(std::shared_ptr<RHI::CommandBuffer> commandBuffer,
																	 RHI::GraphicsPipeline* pipeline,
																	 std::shared_ptr<RHI::DescriptorSet> ubo_set);
		// SET 1
		inline static std::shared_ptr<RHI::DescriptorSetLayout> SET1_Materials_Layout;
		static void SetupPBRBaseColor(std::shared_ptr<RHI::DescriptorSet> SET1_textures, std::shared_ptr<RHI::VMA::Image> base_color);
		static void BindDescriptorSetMaterial(std::shared_ptr<RHI::CommandBuffer> commandBuffer,
																			RHI::GraphicsPipeline* pipeline,
																			std::shared_ptr<RHI::DescriptorSet> material_set);
	};


}} // namespace Albedo::Runtime