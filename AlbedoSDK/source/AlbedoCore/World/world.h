#pragma once

#include <AlbedoCore/Math/vector.h>

namespace Albedo
{

	/*  [ Albedo World Coordinate ] * [Standard Vulkan Coordinate]  *
	* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	*								*								*
	*        (Y)					*         O---------------(X)	*
	*         |						*         |\					*
	*         |     (Z)				*         | \					*
	*         |     /				*         |  \					*
	*         |    /				*         |   \					*
	*         |   /					*         |    \				*
	*         |  /					*         |     \				*
	*         | /					*         |     (Z)				*
	*         |/					*         |						*
	*         O---------------(X)	*        (Y)					*
	*								*								*
	* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	* [Note]														*
	* We transfer "Vulkan Coordinate" to "Albedo World Coordinate"	*
	* by enabling VK_KHR_maintenance1 device extension.				*
	* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	class World
	{
	/* [World Coordinate]
	 * Example:
	 * 1. World::Center
	 * 2. World::Axis.X / World::Axis.Right / World::Axis.Pitch
	 */
	public:
		static inline const Vector3D Center{0, 0, 0};
		static inline const union
		{
			struct { const Vector3D X, Y, Z; };
			struct { const Vector3D Right, Up, Front; };
			struct { const Vector3D Pitch, Yaw, Roll; };
		}Axis{ {{1, 0, 0}, {0, 1, 0}, {0, 0, 1}} };

	private:
		World()							 = delete;
		World(const World&)				 = delete;
		World(World&&)					 = delete;
		World& operator = (const World&) = delete;
		World& operator = (World&&)		 = delete;
	};

} // namespace Albedo