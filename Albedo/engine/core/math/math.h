#pragma once

#include <Eigen/Dense>
#include <Eigen/Geometry>

namespace Albedo 
{
	using namespace Eigen; // Eigen stores matrices in column-major order by default!

	constexpr const double PI = 3.141592653589793;	// 180бу
	constexpr const double PI_HALF			= PI	/ 2.0;		// 90бу
	constexpr const double PI_QUARTER	= PI	/ 4.0;		// 45бу
	constexpr const double PI_SIXTHS		= PI / 6.0;		// 30бу
	constexpr const double TWO_PI			= PI * 2.0;		// 360бу
	constexpr const double ONE_RADIAN = PI / 180.0;	// 1бу

} // namespace Albedo::Runtime