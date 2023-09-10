#pragma once

#include "float.h"
#include <Eigen/Core>

namespace Albedo
{

	using Matrix	= Eigen::MatrixX<FloatType>;
	using Matrix2x2 = Eigen::Matrix2<FloatType>;
	using Matrix3x3 = Eigen::Matrix3<FloatType>;
	using Matrix4x4	= Eigen::Matrix4<FloatType>;

} // namespace Albedo