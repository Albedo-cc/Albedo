#pragma once

#include <Eigen/Core>

namespace Albedo {
namespace Runtime
{
#ifndef ALBEDO_RUNTIME_CORE_HIGH_PRECISION_MODE

	using Vector2 = Eigen::Vector2f;
	using Vector3 = Eigen::Vector3f;
	using Vector4 = Eigen::Vector4f;

	using Matrix2 = Eigen::Matrix2f;
	using Matrix3 = Eigen::Matrix3f;
	using Matrix4 = Eigen::Matrix4f;

#else // Note that double vector is multi-slot variable (OpenGL Wiki)

	using Vector2 = Eigen::Vector2d;
	using Vector3 = Eigen::Vector3d;
	using Vector4 = Eigen::Vector4d;

	using Matrix2 = Eigen::Matrix2d;
	using Matrix3 = Eigen::Matrix3d;
	using Matrix4 = Eigen::Matrix4d;

#endif
}} // namespace Albedo::Runtime