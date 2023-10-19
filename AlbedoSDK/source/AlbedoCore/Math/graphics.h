//#pragma once
//
//#include "../float.h"
//#include "../angle.h"
//#include "../vector.h"
//#include "../matrix.h"
//
//namespace Albedo
//{
//
//	Matrix4x4 MakeLookAtMatrix(
//		const Vector3& position, 
//		const Vector3& target, 
//		const Vector3& upward_direction)
//	{
//		// Only works in UP(0,1,0) Right(1,0,0) Front(0,0,1)
//		Vector3 dir_forward = (target - position).normalized();
//		Vector3 dir_right = dir_forward.cross(upward_direction).normalized();
//		Vector3 dir_up = dir_right.cross(dir_forward);
//
//		Matrix4x4 look_at_matrix;
//		look_at_matrix.setIdentity();
//		auto linear_block = look_at_matrix.block(0, 0, 3, 3);
//
//		linear_block.col(0) = dir_right;
//		linear_block.col(1) = dir_up;
//		linear_block.col(2) = dir_forward;
//		look_at_matrix(0, 3) = -position.dot(dir_right);
//		look_at_matrix(1, 3) = -position.dot(dir_up);
//		look_at_matrix(2, 3) = -position.dot(dir_forward);
//
//		return look_at_matrix;
//	}
//
//} // namespace Albedo