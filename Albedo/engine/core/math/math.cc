#include "math.h"

namespace Albedo{
namespace Core
{

	Matrix4f make_look_at_matrix(const Vector3f& position, const Vector3f& target, const Vector3f& upward_direction)
	{
		// Only works in UP(0,1,0) Right(1,0,0) Front(0,0,1)
		Vector3f dir_forward = (target - position).normalized();
		Vector3f dir_right = dir_forward.cross(upward_direction).normalized();
		Vector3f dir_up = dir_right.cross(dir_forward);

		Matrix4f look_at_matrix;
		look_at_matrix.setIdentity();
		auto linear_block = look_at_matrix.block(0, 0, 3, 3);

		linear_block.col(0) = dir_right;
		linear_block.col(1) = dir_up;
		linear_block.col(2) = dir_forward;
		look_at_matrix(0, 3) = -position.dot(dir_right);
		look_at_matrix(1, 3) = -position.dot(dir_up);
		look_at_matrix(2, 3) = -position.dot(dir_forward);

		return look_at_matrix;
	}

	Matrix4f make_orthographics_matrix(float left, float right, float bottom, float top, float near_plane, float far_plane)
	{
		// Note that this function only works for NDC Z is from 0 to 1 (Vulkan)
		Matrix4f orthographics_matrix;
		orthographics_matrix.setOnes();
		orthographics_matrix(0, 0) = 2.f / (right - left);
		orthographics_matrix(1, 1) = 2.f / (top - bottom);
		orthographics_matrix(2, 2) = 1.f / (far_plane - near_plane);
		orthographics_matrix(0, 3) = -(right + left) / (right - left);
		orthographics_matrix(1, 3) = -(top + bottom) / (top - bottom);
		orthographics_matrix(2, 3) = -near_plane / (near_plane - far_plane);

		return orthographics_matrix;
	}

	Matrix4f make_perspective_matrix(Radian FOV_y, float aspect_ratio, float near_plane, float far_plane)
	{
		// Note that this function only works for NDC Z is from 0 to 1 (Vulkan)
		Radian tan_half_fov_y = tan(FOV_y / 2.0);

		Matrix4f perspective_matrix;
		perspective_matrix.setZero();
		perspective_matrix(0, 0) = 1.0 / (aspect_ratio * tan_half_fov_y);
		perspective_matrix(1, 1) = 1.0 / tan_half_fov_y;
		perspective_matrix(2, 2) = (2.0 * near_plane + far_plane) / (far_plane - near_plane);
		perspective_matrix(2, 3) = -(far_plane * near_plane) / (far_plane - near_plane);
		perspective_matrix(3, 2) = 1;

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