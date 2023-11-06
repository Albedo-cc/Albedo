#include "camera.h"

#include <Albedo.System.Control>

namespace Albedo{
namespace APP
{
	Camera::
	Camera()
	{
		// Register Control Events
		/*ControlSystem::RegisterControlEvent(new ControlEvent
			{

			});*/
	}

	const Matrix4x4&
	Camera::
	GetViewMatrix()
	{
		if (!m_view_matrix.has_value())
		{
			m_view_matrix = m_parameters.tranform.GetModelMatrix();
		}
		return m_view_matrix.value();
	}

	const Matrix4x4&
	Camera::
	GetProjectMatrix()
	{
		// [NOTE]: This matrix is Z-Reversed!
		if (!m_proj_matrix.has_value())
		{
			auto& p = m_parameters;
			float semiFOV = std::tan(p.fov.range / 2.0);
			float n = p.fov.near;
			float f = p.fov.far;
			if (ProjectionMode::Perspective == p.projection)
			{
				m_proj_matrix = Matrix4x4
				{
					{1.0f / (p.fov.aspect * semiFOV), 0				, 0				, 0},
					{0								, 1.0f / semiFOV, 0				, 0},
					{0								, 0				, n / (n - f)	, -f * n / (n - f)},
					{0								, 0				, 1				, 0},
				};
				/*m_proj_matrix = Matrix4x4
				{
					{1.0f / (p.fov.aspect * semiFOV), 0				, 0				, 0},
					{0								, 1.0f / semiFOV, 0				, 0},
					{0								, 0				, f / (f - n)	, -f * n / (f - n)},
					{0								, 0				, 1				, 0},
				};*/
			}
			else // ProjectionMode::Orthographics
			{
				m_proj_matrix = Matrix4x4
				{
					{1.0f / (p.fov.aspect * n * semiFOV), 0						, 0				, 0},
					{0									, 1.0f / (n * semiFOV)	, 0				, 0},
					{0									, 0						, 1 / (n - f)	, -f / (n - f)},
					{0									, 0						, 1				, 0},
				};
			}
		}
		return m_proj_matrix.value();
	}	

}} // namespace Albedo::APP