#pragma once

#include <Eigen/Core>

namespace Albedo
{

	using Matrix	= Eigen::MatrixX<float>;
	using Matrix2x2 = Eigen::Matrix2<float>;
	using Matrix3x3 = Eigen::Matrix3<float>;
	using Matrix4x4	= Eigen::Matrix4<float>;

	using MatrixHP		= Eigen::MatrixX<double>;
	using Matrix2x2HP	= Eigen::Matrix2<double>;
	using Matrix3x3HP	= Eigen::Matrix3<double>;
	using Matrix4x4HP	= Eigen::Matrix4<double>;

} // namespace Albedo