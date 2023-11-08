#pragma once

#include <Albedo.Pattern>
#include <Albedo.Core.Math>

#include <runtime/renderer/data/global_ubo.h>

namespace Albedo{
namespace APP
{

	class Camera final
        : public Pattern::Singleton<Camera>
	{
		friend class Runtime;
		friend class Renderer;
		friend class Pattern::Singleton<Camera>;
	public:
		enum ProjectionMode { Perspective, Orthographics };
		struct Parameters
		{
			struct
			{
				Vector3D front;
				Vector3D upward;
				Vector3D right;
			}coordinate;

			struct
			{
				Degree	range  = 60; //Vertical
				float	aspect = 16.0 / 9.0;
				float	near   = 0.1;
				float	far	   = 1000.0;
			}fov;

			struct
			{
				float	speed	 = 5.0; // ExpectedSpeed, coord/second
				float	velocity = 0.0; // Instantaneous, coord/frame
			}dashboard;

			Transform		tranform;
			ProjectionMode	projection = Perspective;
		};

		auto GetParameters()	const -> const Parameters& { return m_parameters; }
		auto GetViewMatrix()	-> const Matrix4x4&;
		auto GetProjectMatrix() -> const Matrix4x4&;

		auto SetParameters() -> Parameters& { m_view_outdated = true; m_proj_outdated = true; return m_parameters; }
		auto SetTransform()	 -> Transform&	{ m_view_outdated = true; return m_parameters.tranform; }

		auto IsZReversed() const { return true; }

	private:
		void Tick();

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
		bool m_view_outdated = true;
		bool m_proj_outdated = true;
		GlobalUBO::CameraData m_matrics;

	private:
		Camera();
	};

}} // namespace Albedo::APP