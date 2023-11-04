#pragma once

#include "float.h"
#include "angle.h"
#include "vector.h"
#include "matrix.h"

#include <Eigen/Geometry>

namespace Albedo
{
	using Affine2D = Eigen::Affine2f;
	using Affine3D = Eigen::Affine3f;

	using Affine2DHP = Eigen::Affine2d;
	using Affine3DHP = Eigen::Affine3d;

	using AngleAxis   = Eigen::AngleAxisf;
	using AngleAxisHP = Eigen::AngleAxisd;

} // namespace Albedo