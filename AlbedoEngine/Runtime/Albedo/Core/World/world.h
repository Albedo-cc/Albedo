#pragma once

#include <Albedo/Core/Math/vector.h>

namespace Albedo
{

	/*   [Albedo World Coordinate]  * [Standard Vulkan Coordinate]  *
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

	/*  [Albedo Camera Coordinate]  *  [Standard Camera Coordinate] *
	* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	*								*								*
	*			   (Y)				*        (Y)					*
	*				|				*         |						*
	*				|				*         |     (Z)				*
	*				|				*         |     /				*
	*				|				*         |    /				*
	*				O---------(X)	*         |   /					*
	*			   /				*         |  /					*
	*		      /					*         | /					*
	*		     /					*         |/					*
	*		   (Z)					*         O---------------(X)	*
	*								*								*
	* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	* [Note]														*
	* Z-Reverse														*
	* https://developer.nvidia.com/blog/visualizing-depth-precision/*
	* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

	/*  [Albedo Screen Coordinate]  *   [Vulkan Screen Coordinate]  *
	* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	*								*								*
	*        (Y)					*         O---------------(X)	*
	*         |						*         |						*
	*         |     				*         |						*
	*         |     				*         |  					*
	*         |    					*         |   					*
	*         |   					*         |    					*
	*         |  					*         |     				*
	*         | 					*         |     				*
	*         |						*         |						*
	*         O---------------(X)	*        (Y)					*
	*								*								*
	* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	* [Note]														*
	* Y-Inversion(https://www.saschawillems.de/blog/2019/03/29/flip	*
	* ping-the-vulkan-viewport/#:~:text=Different%20coordinate%20sy	*
	* stems%20The%20cause%20for%20this%20is,left%20of%20the%20scree	*
	* n%2C%20with%20Y%20pointing%20downwards.)						*
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