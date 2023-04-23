#include "camera.h"

#include <runtime/function_layer/UI/UI_system.h>
#include <runtime/function_layer/control/control_system.h>
#include <runtime/function_layer/render/paintbox/palette.h>

namespace Albedo {
namespace Runtime
{

	Camera::Camera(std::shared_ptr<RHI::VulkanContext> vulkan_context):
		m_camera_matrix_buffer{ vulkan_context->m_memory_allocator->
		AllocateBuffer(sizeof(CameraMatrics), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
			true, true, false, true) } // Persistent Memory
	{
		m_parameters.right = m_parameters.front.cross(m_parameters.upward).normalized();

		UISystem::instance().RegisterUIEvent(
			"Camera Parameters", [this]()
			{
				ImGui::Begin("Camera Parameters");
				static bool picked = false;
				picked |= ImGui::InputFloat("Position X", &m_parameters.position[0]);
				picked |= ImGui::InputFloat("Position Y", &m_parameters.position[1]);
				picked |= ImGui::InputFloat("Position Z", &m_parameters.position[2]);
				ImGui::Separator();
				
				picked |= ImGui::SliderFloat("FOV(Y)", &m_parameters.FOV_Y, MIN_FOV_Y, MAX_FOV_Y);
				picked |= ImGui::SliderFloat("Near Plane", &m_parameters.plane_near, MIN_NEAR_PLANE, m_parameters.plane_far);
				picked |= ImGui::SliderFloat("Far Plane", &m_parameters.plane_far, MIN_FAR_PLANE, MAX_FAR_PLANE);
				ImGui::Separator();

				if (ImGui::Checkbox("Focus", &m_parameters.is_focusing) && m_parameters.is_focusing)
				{
					picked = true;
					m_parameters.is_moving = false;
					update_view_matrix();
				}
				ImGui::SameLine();
				picked |= ImGui::InputFloat3("", m_parameters.target.data());
				ImGui::Separator();

				picked |= ImGui::Checkbox("Flip-Y", &m_parameters.flip_y);
				ImGui::SameLine();
				ImGui::SliderFloat("Speed", &m_parameters.speed, 0.0, 5.0);
				ImGui::Separator();

				ImGui::End();
				if (picked)
				{
					update_view_matrix();
					update_projection_matrix();
				}
			}
		);

		ControlSystem::instance().RegisterKeyboardEvent(
			KeyboardEventCreateInfo
			{
				.name = "Camera Move Front",
				.key = Keyboard::Key::W,
				.action = Action::Press,
				.event = [this]()
				{
					m_parameters.is_moving = true;
					m_parameters.position += m_parameters.front * m_parameters.speed;
				}
			});

		ControlSystem::instance().RegisterKeyboardEvent(
			KeyboardEventCreateInfo
			{
				.name = "Camera Move Left",
				.key = Keyboard::Key::A,
				.action = Action::Press,
				.event = [this]()
				{
					m_parameters.is_moving = true;
					m_parameters.position -= m_parameters.right * m_parameters.speed;
				}
			});

		ControlSystem::instance().RegisterKeyboardEvent(
			KeyboardEventCreateInfo
			{
				.name = "Camera Move Back",
				.key = Keyboard::Key::S,
				.action = Action::Press,
				.event = [this]()
				{
					m_parameters.is_moving = true;
					m_parameters.position -= m_parameters.front * m_parameters.speed;
				}
			});

		ControlSystem::instance().RegisterKeyboardEvent(
			KeyboardEventCreateInfo
			{
				.name = "Camera Move Right",
				.key = Keyboard::Key::D,
				.action = Action::Press,
				.event = [this]()
				{
					m_parameters.is_moving = true;
					m_parameters.position += m_parameters.right * m_parameters.speed;
				}
			});
	}

	std::shared_ptr<RHI::VMA::Buffer> Camera::
		GetCameraMatrics()
	{
		if (m_parameters.is_moving)
		{
			update_view_matrix();
			update_projection_matrix();
			m_parameters.is_moving = false;
		}

		m_camera_matrix_buffer->Write(&m_camera_matrics);
		return m_camera_matrix_buffer;
	}

	void Camera::update_view_matrix()
	{
		if (m_parameters.is_focusing) m_camera_matrics.matrix_view =
			make_look_at_matrix(m_parameters.position, m_parameters.target, m_parameters.upward);
		else m_camera_matrics.matrix_view =
			make_look_at_matrix(m_parameters.position, m_parameters.position + m_parameters.front, m_parameters.upward);
	}

	void Camera::update_projection_matrix()
	{
		if (ProjectionMode::PERSPECTIVE == m_parameters.projection_mode)
		{
			m_camera_matrics.matrix_projection = make_perspective_matrix(m_parameters.FOV_Y,
				m_parameters.aspect_ratio, m_parameters.plane_near, m_parameters.plane_far);
			if (m_parameters.flip_y) m_camera_matrics.matrix_projection(1, 1) *= -1.0;
		}
		else // ProjectionMode::ORTHOGRAPHICS
		{
			float tanHalfFOV = tan(m_parameters.FOV_Y / 2.0);
			float height = 2.0 * (tanHalfFOV * m_parameters.plane_near);
			float width = height * m_parameters.aspect_ratio;
			m_camera_matrics.matrix_projection = 
				make_orthographics_matrix(0, width, height, 0,
				m_parameters.plane_near, m_parameters.plane_far);
		}
	}

}} // namespace Albedo::Runtime