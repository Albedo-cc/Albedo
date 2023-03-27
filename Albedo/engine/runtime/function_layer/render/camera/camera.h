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
		static constexpr float MAX_FOV_Y = (PI_HALF - ONE_RADIAN);
		static constexpr float MIN_FOV_Y = ONE_RADIAN;
	public:
		enum class ProjectionMode { ORTHOGRAPHICS, PERSPECTIVE };
		struct Parameter
		{
			ProjectionMode projection_mode;
			Vector3f position			{ WORLD_CENTER };
			Vector3f forward			{ -WORLD_AXIS_Z };
			Vector3f upward			{ WORLD_AXIS_Y };
			
			float FOV = PI_QUARTER;
			float aspect_ratio;
			float plane_near{ -0.1 }, plane_far{ -100.0 };
		};
		Camera(std::shared_ptr<RHI::VulkanContext> vulkan_context, ProjectionMode mode = ProjectionMode::PERSPECTIVE);

		void SetProjectionMode(ProjectionMode mode) { m_parameters.projection_mode = mode; GetViewingMatrix(true); }
		Matrix4f GetViewingMatrix(bool update = false); // = M_Projection * M_View

	private:
		std::shared_ptr<RHI::VulkanContext> m_vulkan_context;
		ProjectionMode m_projection_mode;
		Parameter m_parameters;
		bool m_is_moved = true;
	};

}} // namespace Albedo::Runtime