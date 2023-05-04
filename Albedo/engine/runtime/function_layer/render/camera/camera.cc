#include "camera.h"

#include <runtime/function_layer/UI/UI_system.h>
#include <runtime/function_layer/control/control_system.h>
#include <runtime/function_layer/render/paintbox/palette.h>

#include <net/net.h>

namespace Albedo {
namespace Runtime
{

	Camera::Camera(std::shared_ptr<RHI::VulkanContext> vulkan_context):
		m_camera_matrix_buffer{ vulkan_context->m_memory_allocator->
		AllocateBuffer(sizeof(CameraMatrics), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
			true, true, false, true) } // Persistent Memory
	{
		// Initialize Camera Parameters
		m_parameters.right = m_parameters.front.cross(m_parameters.upward).normalized();

		// Note: we are using Vulkan Coordinate System
		Radian pitch{ degree_to_radian(m_parameters.pitch) };
		Radian yaw{ degree_to_radian(m_parameters.yaw) };
		m_parameters.front.x() = cos(pitch) * sin(yaw);
		m_parameters.front.y() = sin(pitch);
		m_parameters.front.z() = cos(pitch) * cos(yaw);
		m_parameters.front.normalize();

		UISystem::instance().RegisterUIEvent(
			"Camera Parameters", [this]()
			{
				// Sync Online Camera
				static bool sync_from_net = false;

				static auto& players = Net::NetModule::instance().ViewPlayers();
				static int32_t selected_player;
				static const Net::NetModule::PlayerState* selected_player_state = nullptr;

				bool update_view = false;
				bool update_projection = false;

				// Update Aspect Ratio
				float oldAspectRatio = m_parameters.aspect_ratio;
				m_parameters.aspect_ratio = UISystem::instance().GetMainSceneAspectRatio();
				if (oldAspectRatio != m_parameters.aspect_ratio) update_projection = true;

				ImGui::Begin("Camera Parameters");
				{
					update_view |= ImGui::InputFloat("Position X", &m_parameters.position[0]);
					update_view |= ImGui::InputFloat("Position Y", &m_parameters.position[1]);
					update_view |= ImGui::InputFloat("Position Z", &m_parameters.position[2]);
					ImGui::Separator();

					update_view |= ImGui::InputDouble("Yaw", &m_parameters.yaw);
					update_view |= ImGui::InputDouble("Pitch", &m_parameters.pitch);
					update_view |= ImGui::InputDouble("Roll", &m_parameters.roll);
					ImGui::Separator();

					update_projection |= ImGui::SliderFloat("FOV(Y)", &m_parameters.FOV_Y, MIN_FOV_Y, MAX_FOV_Y);
					update_projection |= ImGui::SliderFloat("Near Plane", &m_parameters.plane_near, MIN_NEAR_PLANE, m_parameters.plane_far);
					update_projection |= ImGui::SliderFloat("Far Plane", &m_parameters.plane_far, MIN_FAR_PLANE, MAX_FAR_PLANE);
					ImGui::Separator();

					if (ImGui::Checkbox("Focus", &m_parameters.is_focusing))
					{
						update_view |= true;
					}
					ImGui::SameLine();
					update_view |= ImGui::InputFloat3("", m_parameters.target.data());
					ImGui::Separator();

					m_parameters.is_active = UISystem::IsFocusingOnMainScene();
					ImGui::RadioButton("Active", m_parameters.is_active);
					ImGui::SameLine();
					ImGui::SliderFloat("Speed", &m_parameters.speed, 0.0, 2.0);
					ImGui::Separator();

					update_projection |= ImGui::Checkbox("Flip-Y", &m_parameters.flip_y);
					ImGui::SameLine();
					const char* projection_modes[]{ "Orthographic", "Perspective" };
					static int32_t projection_mode_index = (int32_t)(m_parameters.projection_mode);
					if (ImGui::Combo("Projection", &projection_mode_index,
						"Orthographic\0Perspective\0"))
					{
						update_projection |= true;
						m_parameters.projection_mode = ProjectionMode(projection_mode_index);
					}

					ImGui::SeparatorText("Online Functions");

					bool is_online = Net::NetModule::instance().IsOnline();
					if (!is_online) ImGui::BeginDisabled();
					{
						update_view |= ImGui::Checkbox("Sync Online Camera", &sync_from_net);
						if (!is_online) selected_player = -1;
						 
						static std::string surfaceDisplayName;
						static std::string comboDisplayName;
						if (ImGui::BeginCombo("Who's View", selected_player_state ? surfaceDisplayName.c_str() : "(NULL)"))
						{
							for (const auto& [id, state] : players)
							{
								auto& profile = state.profile;
								bool is_selected = (selected_player == id);
								comboDisplayName = std::format("{} ({})", profile.nickname(), profile.uid());
								if (ImGui::Selectable(comboDisplayName.c_str(), is_selected))
								{
									selected_player = id;
									ImGui::SetItemDefaultFocus();
									surfaceDisplayName = std::format("{} ({})", profile.nickname(), profile.uid());
									selected_player_state = &state;
								}
							}
							ImGui::EndCombo();
						}
					}
					if (!is_online) ImGui::EndDisabled();
				}
				ImGui::End();


				if (sync_from_net)
				{
					m_parameters.is_active = false;
					m_camera_matrics.matrix_view = selected_player_state->camera_view_matrix;
				}
				else
				{
					update_camera_rotation(); // [FIXME]: Avoid invalid updating
					if (update_view) update_view_matrix();
					if (update_projection) update_projection_matrix();
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
					if (!m_parameters.is_active) return;
					m_parameters.should_update = true;
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
					if (!m_parameters.is_active) return;
					m_parameters.should_update = true;
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
					if (!m_parameters.is_active) return;
					m_parameters.should_update = true;
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
					if (!m_parameters.is_active) return;
					m_parameters.should_update = true;
					m_parameters.position += m_parameters.right * m_parameters.speed;
				}
			});

		ControlSystem::instance().RegisterKeyboardEvent(
			KeyboardEventCreateInfo
			{
				.name = "Camera Move Down",
				.key = Keyboard::Key::E,
				.action = Action::Press,
				.event = [this]()
				{
					if (!m_parameters.is_active) return;
					m_parameters.should_update = true;
					m_parameters.position -= m_parameters.upward * m_parameters.speed;
				}
			});

		ControlSystem::instance().RegisterKeyboardEvent(
			KeyboardEventCreateInfo
			{
				.name = "Camera Move Up",
				.key = Keyboard::Key::Q,
				.action = Action::Press,
				.event = [this]()
				{
					if (!m_parameters.is_active) return;
					m_parameters.should_update = true;
					m_parameters.position += m_parameters.upward * m_parameters.speed;
				}
			});

		ControlSystem::instance().RegisterMouseScrollEvent(
			MouseScrollEventCreateInfo
			{
				.name = "Camera FOV",
				.event = [this](double x, double y)
				{
					if (!m_parameters.is_active) return;
					m_parameters.should_update = true;
					float new_fov_y = m_parameters.FOV_Y - (1.0 * y);
					m_parameters.FOV_Y = std::clamp(new_fov_y, MIN_FOV_Y, MAX_FOV_Y);
				}
			});
	}

	std::shared_ptr<RHI::VMA::Buffer> Camera::
		GetCameraMatricsBuffer()
	{
		if (m_parameters.should_update)
		{
			update_view_matrix();
			update_projection_matrix();
			m_parameters.should_update = false;
		}

		m_camera_matrix_buffer->Write(&m_camera_matrics);
		return m_camera_matrix_buffer;
	}

	void Camera::SetPosition(Vector3f position)
	{
		m_parameters.position = std::move(position);
		m_parameters.should_update = true;
	}

	void Camera::SetProjectionMode(ProjectionMode mode)
	{
		m_parameters.projection_mode = mode;
		m_parameters.should_update = true;
	}

	void Camera::update_camera_rotation()
	{
		if (m_parameters.is_active)
		{
			if (Action::Press == ControlSystem::instance().GetMouseButtonAction(Mouse::Button::Left))
			{
				static float sensitivity = 0.05;
				{
					auto& cursor = ControlSystem::GetCursor();
					m_parameters.yaw = std::clamp(
						m_parameters.yaw + cursor.delta_x * sensitivity,
						MIN_EULER_ANGLE_YAW,
						MAX_EULER_ANGLE_YAW);

					m_parameters.pitch = std::clamp(
						m_parameters.pitch + cursor.delta_y * sensitivity,
						MIN_EULER_ANGLE_PITCH,
						MAX_EULER_ANGLE_PITCH);
					
					// Note: we are using Vulkan Coordinate System
					Radian pitch{ degree_to_radian(m_parameters.pitch) };
					Radian yaw{ degree_to_radian(m_parameters.yaw) };
					m_parameters.front.x() = cos(pitch) * sin(yaw);
					m_parameters.front.y() = sin(pitch);
					m_parameters.front.z() = cos(pitch) * cos(yaw);
					m_parameters.front.normalize();

					update_view_matrix();
				}
			}
		} // End update_camera_view()
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
			m_camera_matrics.matrix_projection = make_perspective_matrix(
				m_parameters.FOV_Y * ONE_DEGREE,
				m_parameters.aspect_ratio, m_parameters.plane_near, m_parameters.plane_far);
			if (m_parameters.flip_y) m_camera_matrics.matrix_projection(1, 1) *= -1.0;
		}
		else // ProjectionMode::ORTHOGRAPHICS
		{
			float tanHalfFOV = tan(m_parameters.FOV_Y * ONE_DEGREE / 2.0);
			float top = tanHalfFOV * m_parameters.plane_near;
			float right = top * m_parameters.aspect_ratio;
			m_camera_matrics.matrix_projection = 
				make_orthographics_matrix(-right, right,  -top, top,
				m_parameters.plane_near, m_parameters.plane_far);
		}
	}

}} // namespace Albedo::Runtime