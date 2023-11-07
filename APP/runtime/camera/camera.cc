#include "camera.h"

#include <Albedo.Core.Log>
#include <Albedo.System.Control>

namespace Albedo{
namespace APP
{
	Camera::
	Camera()
	{
		// Init Matrics
		GetViewMatrix();
		GetProjectMatrix();

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
						m_parameters.speed;
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
						m_parameters.speed;
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
						m_parameters.speed;
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
						m_parameters.speed;
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
						m_parameters.speed;
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
						m_parameters.speed;
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
			float semiFOV = std::tan(p.fov.range / 2.0);
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