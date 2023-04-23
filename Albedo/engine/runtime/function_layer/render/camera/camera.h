#pragma once

#include <AlbedoRHI.hpp>
#include <core/world/coordinate_system.h>

namespace Albedo {
namespace Runtime
{
	using namespace Albedo::Core;

	class Camera
	{
		static constexpr float MAX_FOV_Y	= 89 * ONE_DEGREE;
		static constexpr float MIN_FOV_Y	= 10 * ONE_DEGREE;
		static constexpr float MAX_FAR_PLANE		= 65535.0; // If Z-Buffer is 16bits
		static constexpr float MIN_FAR_PLANE		= 100.0;
		static constexpr float MIN_NEAR_PLANE	= 0.01;

	public:
		enum class ProjectionMode { ORTHOGRAPHICS, PERSPECTIVE };
		struct Parameter
		{
			ProjectionMode projection_mode = ProjectionMode::PERSPECTIVE;
			Vector3f position			{ WORLD_CENTER };
			Vector3f front				{ WORLD_AXIS_Z };
			Vector3f upward			{ WORLD_AXIS_Y };
			Vector3f right;				//Cross(front, upward)

			bool is_focusing = false;
			Vector3f target				{ 0.0, 0.0, -1.0 };
			
			float FOV_Y = 60 * ONE_DEGREE;
			bool flip_y = true;
			float aspect_ratio = 16.0 / 9.0; // 1920 : 1080
			float plane_near{ 0.1 }, plane_far{ 100.0 };
			
			bool is_active = false; // ImGui::IsWindowFocused()
			bool is_moving = false;
			float speed = 1.0;
		};

		struct CameraMatrics
		{
			Matrix4f matrix_projection;
			Matrix4f matrix_view;
		};
		std::shared_ptr<RHI::VMA::Buffer> GetCameraMatrics();

		void SetPosition(Vector3f position) { m_parameters.position = position; }
		void SetProjectionMode(ProjectionMode mode) 
		{ m_parameters.projection_mode = mode; update_projection_matrix(); }
		
	public:
		Camera(std::shared_ptr<RHI::VulkanContext> vulkan_context);

	private:
		void update_projection_matrix();
		void update_view_matrix();

	private:
		Parameter m_parameters;

		CameraMatrics m_camera_matrics;
		std::shared_ptr<RHI::VMA::Buffer> m_camera_matrix_buffer;
	};

}} // namespace Albedo::Runtime