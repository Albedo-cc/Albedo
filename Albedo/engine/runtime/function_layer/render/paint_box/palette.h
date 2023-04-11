#pragma once

#include <AlbedoRHI.hpp>

namespace Albedo {
namespace Runtime
{

	class Palette
	{
		friend class Canvas;
	public: // Interfaces
		struct UBO_Camera_Matrics
		{
			glm::mat4x4 matrix_model;
			glm::mat4x4 matrix_view;
			glm::mat4x4 matrix_projection;
		};
		UBO_Camera_Matrics camera_matrics;

	private: // Hide Descriptor Sets and Memory Objects to users and bind them automatically
		std::shared_ptr<RHI::DescriptorSet> m_ubo_matrics;
		std::shared_ptr<RHI::VMA::Buffer> m_ubo_camera_matrics;
		std::shared_ptr<RHI::VMA::Buffer> m_ubo_pbr_parameters;

		std::shared_ptr<RHI::DescriptorSet> m_sampler_pbr_textures;
		std::vector<std::shared_ptr<RHI::VMA::Image>> m_textures;
	};

}} // namespace Albedo::Runtime