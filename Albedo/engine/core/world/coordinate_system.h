#pragma once

#include <core/math/math.h>

namespace Albedo{
namespace Core
{

	/* [ Vulkan Coordinate System ]
	* 
	- The X axis increasing to the right
	- The Y axis increasing downwards
	- The Z axis increasing into the screen
	- The Origin at the top-left corner of the screen
	- The Depth range from 0 to 1
	*
	* This means that Vulkan's coordinate system is left-handed and
	   Y-inverted compared to the standard Cartesian coordinate system. 
	*/

	const Vector3f WORLD_CENTER	{ 0, 0, 0 };
	const Vector3f WORLD_AXIS_X		{ 1, 0, 0 };
	const Vector3f WORLD_AXIS_Y		{ 0, 1, 0 };
	const Vector3f WORLD_AXIS_Z		{ 0, 0, 1 };

}} // namespace Albedo::Core