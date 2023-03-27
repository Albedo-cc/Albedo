#pragma once

#include <Eigen/Dense>
#include <Eigen/Geometry>

namespace Albedo{
namespace Core
{

	using namespace Eigen; // Eigen stores matrices in column-major order by default!
	using Radian = double;

	// PI
	constexpr Radian PI = 3.141592653589793;	// 180
	constexpr Radian PI_HALF			= PI	/ 2.0;		// 90
	constexpr Radian PI_QUARTER	= PI	/ 4.0;		// 45
	constexpr Radian PI_SIXTHS		= PI / 6.0;		// 30
	constexpr Radian TWO_PI			= PI * 2.0;		// 360
	constexpr Radian ONE_RADIAN = PI / 180.0;	// 1

	// Graphics
	Matrix4f make_look_at_matrix(const Vector3f& position, const Vector3f& target, const Vector3f& upward_direction);
	Matrix4f make_orthographics_matrix(float left, float right, float top, float bottom, float near_plane, float far_plane);
	Matrix4f make_perspective_matrix(Radian FOV_y, float aspect_ratio, float near_plane, float far_plane);
	Matrix4f make_rotation_matrix(Vector3f rotation_axis, Radian radians);

}} // namespace Albedo::Core