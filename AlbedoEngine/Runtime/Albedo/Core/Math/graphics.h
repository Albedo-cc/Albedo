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

	struct Transform
	{
		Vector3D	scale		{ Vector3D::Ones() };
		Vector3D	translate	{ Vector3D::Zero() };
		union { struct { Degree roll, pitch, yaw; }; }rotate{ 0, 0, 0 };

		void GetModelMatrix(Affine3D& out) const;
		auto GetModelMatrix() const -> Matrix4x4;
		void GetViewMatrix(Affine3D& out) const;
		auto GetViewMatrix() const -> Matrix4x4;
	};

} // namespace Albedo