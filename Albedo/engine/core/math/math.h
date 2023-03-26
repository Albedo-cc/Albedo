#pragma once

#include <Eigen/Dense>
#include <Eigen/Geometry>

namespace Albedo 
{
	using namespace Eigen; // Eigen stores matrices in column-major order by default!

	constexpr const double PI = 3.141592653589793;	// 180��
	constexpr const double PI_HALF			= PI	/ 2.0;		// 90��
	constexpr const double PI_QUARTER	= PI	/ 4.0;		// 45��
	constexpr const double PI_SIXTHS		= PI / 6.0;		// 30��
	constexpr const double TWO_PI			= PI * 2.0;		// 360��
	constexpr const double ONE_RADIAN = PI / 180.0;	// 1��

} // namespace Albedo::Runtime