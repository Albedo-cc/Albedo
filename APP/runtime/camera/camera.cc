#include "camera.h"

#include <Albedo.Core.Log>
#include <Albedo.Core.World>
#include <Albedo.System.Control>
#include <Albedo.Editor>

#include <runtime/runtime.h>

namespace Albedo{
namespace APP
{
	void
	Camera::
	Tick()
	{
		auto& settings = m_parameters;
		// Adjust Speed
		settings.dashboard.velocity = settings.dashboard.speed / Runtime::GetFPS();
	}


	Camera::
	Camera() :
		m_parameters
		{ 
			.coordinate
			{
				.front	= World::Axis.Front,
				.upward = World::Axis.Up,
				.right  = World::Axis.Right,
			},
			.tranform
			{
				.scale	   = Vector3D::Ones(),
				.translate = World::Center,
				.rotate    = {0, 0, 0}, // (Row, Pitch, Yaw)
			},
		}
	{
		// Init Matrics
		GetViewMatrix();
		GetProjectMatrix();

		// Rigister Editor Events
		Editor::RegisterUIEvent(new UIEvent
			{
				"Camera::Dashboard",[this]()->void
				{
					ImGui::Begin("Camera Dashboard");
					{
						ImGui::InputFloat("Speed",
							&m_parameters.dashboard.speed);

						auto& vm = GetViewMatrix();
						auto& pm = GetProjectMatrix();
						ImGui::Text("View Matrix");
						ImGui::Text(
							"%.2f, %.2f, %.2f, %.2f\n"
							"%.2f, %.2f, %.2f, %.2f\n"
							"%.2f, %.2f, %.2f, %.2f\n"
							"%.2f, %.2f, %.2f, %.2f\n"
						, vm(0,0), vm(0,1), vm(0,2), vm(0,3)
						, vm(1,0), vm(1,1), vm(1,2), vm(1,3)
						, vm(2,0), vm(2,1), vm(2,2), vm(2,3)
						, vm(3,0), vm(3,1), vm(3,2), vm(3,3)
						);
						ImGui::Text("Project Matrix");
						ImGui::Text(
							"%.2f, %.2f, %.2f, %.2f\n"
							"%.2f, %.2f, %.2f, %.2f\n"
							"%.2f, %.2f, %.2f, %.2f\n"
							"%.2f, %.2f, %.2f, %.2f\n"
						, pm(0,0), pm(0,1), pm(0,2), pm(0,3)
						, pm(1,0), pm(1,1), pm(1,2), pm(1,3)
						, pm(2,0), pm(2,1), pm(2,2), pm(2,3)
						, pm(3,0), pm(3,1), pm(3,2), pm(3,3)
						);
					}
					ImGui::End();
				}
			});

		// Register Control Events
		ControlSystem::RegisterKeyboardEvent(
			KeyboardEventCreateInfo
			{
				.name	= "Camera::MoveForward",
				.key	= Keyboard::W,
				.action = ActionType::Press,
				.callback = [this]()
				{
					auto& transform = SetTransform();

					transform.translate +=
						m_parameters.coordinate.front *
						m_parameters.dashboard.velocity;
				}
			}
		);

		ControlSystem::RegisterKeyboardEvent(
			KeyboardEventCreateInfo
			{
				.name	= "Camera::MoveBack",
				.key	= Keyboard::S,
				.action = ActionType::Press,
				.callback = [this]()
				{
					auto& transform = SetTransform();

					transform.translate -=
						m_parameters.coordinate.front *
						m_parameters.dashboard.velocity;
				}
			}
		);

		ControlSystem::RegisterKeyboardEvent(
			KeyboardEventCreateInfo
			{
				.name	= "Camera::MoveRight",
				.key	= Keyboard::D,
				.action = ActionType::Press,
				.callback = [this]()
				{
					auto& transform = SetTransform();

					transform.translate +=
						m_parameters.coordinate.right *
						m_parameters.dashboard.velocity;
				}
			}
		);

		ControlSystem::RegisterKeyboardEvent(
			KeyboardEventCreateInfo
			{
				.name	= "Camera::MoveLeft",
				.key	= Keyboard::A,
				.action = ActionType::Press,
				.callback = [this]()
				{
					auto& transform = SetTransform();

					transform.translate -=
						m_parameters.coordinate.right *
						m_parameters.dashboard.velocity;
				}
			}
		);

		ControlSystem::RegisterKeyboardEvent(
			KeyboardEventCreateInfo
			{
				.name	= "Camera::MoveUp",
				.key	= Keyboard::Q,
				.action = ActionType::Press,
				.callback = [this]()
				{
					auto& transform = SetTransform();

					transform.translate +=
						m_parameters.coordinate.upward *
						m_parameters.dashboard.velocity;
				}
			}
		);

		ControlSystem::RegisterKeyboardEvent(
			KeyboardEventCreateInfo
			{
				.name	= "Camera::MoveDown",
				.key	= Keyboard::E,
				.action = ActionType::Press,
				.callback = [this]()
				{
					auto& transform = SetTransform();

					transform.translate -=
						m_parameters.coordinate.upward *
						m_parameters.dashboard.velocity;
				}
			}
		);
	}

	const Matrix4x4&
	Camera::
	GetViewMatrix()
	{
		if (m_view_outdated)
		{
			m_matrics.view_matrix = m_parameters.tranform.GetViewMatrix();
			m_view_outdated = false;
		}
		return m_matrics.view_matrix;
	}

	const Matrix4x4&
	Camera::
	GetProjectMatrix()
	{
		// [NOTE]: This matrix is Z-Reversed!
		if (m_proj_outdated)
		{
			auto& p = m_parameters;
			float semiFOV = Tan(p.fov.range) / 2.0;
			float n = p.fov.near;
			float f = p.fov.far;
			if (ProjectionMode::Perspective == p.projection)
			{
				m_matrics.proj_matrix = Matrix4x4
				{
					{1.0f / (p.fov.aspect * semiFOV), 0				, 0				, 0},
					{0								, 1.0f / semiFOV, 0				, 0},
					{0								, 0				, n / (n - f)	, -f * n / (n - f)},
					{0								, 0				, 1				, 0},
				};
			}
			else // ProjectionMode::Orthographics
			{
				m_matrics.proj_matrix = Matrix4x4
				{
					{1.0f / (p.fov.aspect * n * semiFOV), 0						, 0				, 0},
					{0									, 1.0f / (n * semiFOV)	, 0				, 0},
					{0									, 0						, 1 / (n - f)	, -f / (n - f)},
					{0									, 0						, 1				, 0},
				};
			}
			m_proj_outdated = false;
		}
		return m_matrics.proj_matrix;
	}

}} // namespace Albedo::APP