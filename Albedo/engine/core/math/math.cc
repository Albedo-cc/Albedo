#include "math.h"

namespace Albedo{
namespace Core
{

	Matrix4f make_look_at_matrix(const Vector3f& position, const Vector3f& target, const Vector3f& upward_direction)
	{
		Vector3f dir_forward = (target - position).normalized();
		Vector3f dir_right = dir_forward.cross(upward_direction).normalized();
		Vector3f dir_up = dir_right.cross(dir_forward);

		Matrix4f look_at_matrix;
		look_at_matrix.setIdentity();
		auto linear_block = look_at_matrix.block(0, 0, 3, 3);

		linear_block.row(0) = dir_right;
		linear_block.row(1) = dir_up;
		linear_block.row(2) = dir_forward;
		look_at_matrix(0, 3) = position.dot(dir_right);
		look_at_matrix(1, 3) = position.dot(dir_up);
		look_at_matrix(2, 3) = position.dot(dir_forward);

		return look_at_matrix;
	}

	Matrix4f make_orthographics_matrix(float left, float right, float top, float bottom, float near_plane, float far_plane)
	{
		Matrix4f orthographics_matrix;
		orthographics_matrix.setOnes();
		orthographics_matrix(0, 0) = 2.f / (right - left);
		orthographics_matrix(1, 1) = 2.f / (bottom - top);
		orthographics_matrix(2, 2) = 1.f / (far_plane - near_plane);
		orthographics_matrix(3, 0) = -(right + left) / (right - left);
		orthographics_matrix(3, 1) = -(bottom + top) / (bottom - top);
		orthographics_matrix(3, 2) = -near_plane / (far_plane - near_plane);

		return orthographics_matrix;
	}

	Matrix4f make_perspective_matrix(Radian FOV_y, float aspect_ratio, float near_plane, float far_plane)
	{
		Radian tan_half_fov_y = tan(FOV_y / 2.0);

		Matrix4f perspective_matrix;
		perspective_matrix.setZero();
		perspective_matrix(0, 0) = 1.0 / (aspect_ratio * tan_half_fov_y);
		perspective_matrix(1, 1) = 1.0 / tan_half_fov_y;
		perspective_matrix(2, 2) = far_plane / (far_plane - near_plane);
		perspective_matrix(3, 2) = 1.0f;
		perspective_matrix(2, 3) = (near_plane * far_plane) / (near_plane - far_plane);

		return perspective_matrix;
	}

	Matrix4f make_rotation_matrix(Vector3f rotation_axis, Radian radians)
	{
		AngleAxisf rotation_vector(radians, rotation_axis);

		Matrix4f rotation_matrix;
		rotation_matrix.setIdentity();
		rotation_matrix.block(0, 0, 3, 3) = rotation_vector.toRotationMatrix();

		return rotation_matrix;
	}

}} // namespace Albedo::Core