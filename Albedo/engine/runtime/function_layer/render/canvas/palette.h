#pragma once

#include <AlbedoRHI.hpp>

namespace Albedo {
namespace Runtime
{

	class Palette
	{
	public:
		enum Layout
		{
			// layout(set = 0) Descriptor Set UBO
			set_uniform_buffers			=		0,
				binding_camera_matrics		=		0,
				binding_pbr_parameters		=		1,

			// layout(set = 1) Textures
			set_textures					=		1,
				binding_base_color		=		0
		};

	public:
		std::shared_ptr<RHI::DescriptorPool> m_descriptor_pool;

		std::shared_ptr<RHI::DescriptorSet> ubo_matrics;
		std::shared_ptr<RHI::VMA::Buffer> ubo_camera_matrics;
		std::shared_ptr<RHI::VMA::Buffer> ubo_pbr_parameters;
		std::shared_ptr<RHI::DescriptorSet> sampler_pbr_textures;

	public:
		//Palette() = delete;
		Palette()
		{
			//descriptorPool->AllocateDescriptorSet()
		}
	};

}} // namespace Albedo::Runtime