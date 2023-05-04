#pragma once

#include <AlbedoRHI.hpp>
#include <core/world/coordinate_system.h>

namespace Albedo {
namespace Runtime
{
	using namespace Albedo::Core;

	class Camera
	{
		static constexpr float MAX_FOV_Y	= 89;
		static constexpr float MIN_FOV_Y	= 10;
		static constexpr float MAX_FAR_PLANE		= 65535.0; // If Z-Buffer is 16bits
		static constexpr float MIN_FAR_PLANE		= 100.0;
		static constexpr float MIN_NEAR_PLANE	= 0.01;
		static constexpr Degree MAX_EULER_ANGLE_PITCH	= 89;
		static constexpr Degree MIN_EULER_ANGLE_PITCH	= -89;
		static constexpr Degree MAX_EULER_ANGLE_YAW		= 180;
		static constexpr Degree MIN_EULER_ANGLE_YAW		= -180;

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
			
			float FOV_Y = 60;
			bool	 flip_y = true;
			float aspect_ratio = 16.0 / 9.0; // 1920 : 1080
			float plane_near{ 0.1 }, plane_far{ 100.0 };
			
			bool is_active = false; // ImGui::IsWindowFocused()
			bool should_update = false;
			float speed = 0.2;

			// Camera Rotate
			Degree pitch{ 0 }, yaw{ 0 }, roll{ 0 };
		};

		struct CameraMatrics
		{
			Matrix4f matrix_projection;
			Matrix4f matrix_view;
		};
		std::shared_ptr<RHI::VMA::Buffer> GetCameraMatricsBuffer();
		const CameraMatrics& GetCameraMatrics() const { return m_camera_matrics; }

		void SetPosition(Vector3f position);
		void SetProjectionMode(ProjectionMode mode);
		
	public:
		Camera(std::shared_ptr<RHI::VulkanContext> vulkan_context);

	private:
		void update_camera_rotation();
		void update_projection_matrix();
		void update_view_matrix();

	private:
		Parameter m_parameters;

		CameraMatrics m_camera_matrics;
		std::shared_ptr<RHI::VMA::Buffer> m_camera_matrix_buffer;
	};

}} // namespace Albedo::Runtime