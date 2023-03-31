#pragma once

#include <AlbedoRHI.hpp>
#include <core/world/coordinate_system.h>

namespace Albedo {
namespace Runtime
{
	using namespace Albedo::Core;

	class Camera
	{
		static constexpr float MAX_FAR_PLANE = 65535.0; // If Z-Buffer is 16bits
		static constexpr float MAX_FOV_Y	= 89 * ONE_RADIAN;
		static constexpr float MIN_FOV_Y	= 10 * ONE_RADIAN;
	public:
		enum class ProjectionMode { ORTHOGRAPHICS, PERSPECTIVE };
		struct Parameter
		{
			ProjectionMode projection_mode;
			Vector3f position			{ WORLD_CENTER };
			Vector3f target				{ -WORLD_AXIS_Z };
			Vector3f upward			{ -WORLD_AXIS_Y };
			
			float FOV = MAX_FOV_Y;
			float aspect_ratio;
			float plane_near{ -0.1 }, plane_far{ -100.0 };
		};
		Camera(std::shared_ptr<RHI::VulkanContext> vulkan_context, ProjectionMode mode = ProjectionMode::PERSPECTIVE);

		void SetProjectionMode(ProjectionMode mode) { m_parameters.projection_mode = mode; GetViewingMatrix(true); }
		Matrix4f GetViewingMatrix(bool update = false); // = M_Projection * M_View
		Matrix4f GetViewMatrix();
		Matrix4f GetProjectionMatrix();

	private:
		std::shared_ptr<RHI::VulkanContext> m_vulkan_context;
		ProjectionMode m_projection_mode;
		Parameter m_parameters;
		bool m_is_moved = true;
	};

}} // namespace Albedo::Runtime