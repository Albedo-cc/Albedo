#pragma once

#include <Albedo.Pattern>
#include <Albedo.Core.Math>
#include <Albedo.Core.World>

#include <optional>

namespace Albedo{
namespace APP
{

	class Camera final
        : public Pattern::Singleton<Camera>
	{
		friend class Runtime;
		friend class Pattern::Singleton<Camera>;
	public:
		enum ProjectionMode { Perspective, Orthographics };
		struct Parameters
		{
			struct
			{
				Vector3D front	{ World::Axis.Front };
				Vector3D upward	{ World::Axis.Up	};
				Vector3D right	{ World::Axis.Right };
			}coordinate;

			struct
			{
				Degree	range  = 60; //Vertical
				float	aspect = 16.0 / 9.0;
				float	near   = 0.1;
				float	far	   = 1000.0;
				bool	flipY  = false;
			}fov;

			Transform		tranform;
			float			speed = 0.2;
			ProjectionMode	projection = Perspective;
		};

		auto GetParameters()	const -> const Parameters& { return m_parameters; }
		auto GetViewMatrix()	-> const Matrix4x4&;
		auto GetProjectMatrix() -> const Matrix4x4&;

		auto SetParameters() -> Parameters& { m_view_matrix.reset(); m_proj_matrix.reset(); return m_parameters; }
		auto SetTransform()	 -> Transform&	{ m_view_matrix.reset(); return m_parameters.tranform; }

		static constexpr Degree MAX_FOV_Y				= 89;
		static constexpr Degree MIN_FOV_Y				= 10;

		static constexpr Degree MAX_EULER_ANGLE_PITCH	= 89;
		static constexpr Degree MIN_EULER_ANGLE_PITCH	= -89;
		static constexpr Degree MAX_EULER_ANGLE_YAW		= 180;
		static constexpr Degree MIN_EULER_ANGLE_YAW		= -180;

		static constexpr float MIN_NEAR_PLANE			= 0.01;
		static constexpr float MIN_FAR_PLANE			= 100.0;
		static constexpr float MAX_FAR_PLANE			= 65535.0; // If Z-Buffer is 16bits

	private:
		Parameters	m_parameters;
		std::optional<Matrix4x4> m_view_matrix{};
		std::optional<Matrix4x4> m_proj_matrix{};

	private:
		Camera();
	};

}} // namespace Albedo::APP