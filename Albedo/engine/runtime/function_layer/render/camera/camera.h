#pragma once

#include <AlbedoRHI.hpp>
#include <core/math/math.h>

namespace Albedo {
namespace Runtime
{

	/* [ Vulkan Coordinate System ]
	* 
	- The X axis increasing to the right
	- The Y axis increasing downwards
	- The Z axis increasing into the screen
	- The Origin at the top-left corner of the screen
	- The Depth range from 0 to 1
	*
	* This means that Vulkan¡¯s coordinate system is left-handed and
	   Y-inverted compared to the standard Cartesian coordinate system. 
	*/

	class Camera
	{
	public:
		enum class ProjectionMode { ORTHOGRAPHICS, PERSPECTIVE };
		Camera(ProjectionMode mode = ProjectionMode::PERSPECTIVE);

		void SetProjectionMode(ProjectionMode mode) { m_projection_mode = mode; GetViewingMatrix(true); }
		Matrix4f GetViewingMatrix(bool update = false); // = M_Projection * M_View

	protected:
		std::shared_ptr<RHI::VulkanContext> m_vulkan_context;
		ProjectionMode m_projection_mode;

		Vector3f m_position;
		Vector3f m_lookat;
		Vector3f m_updirection;

		float m_plane_near = -0.1f;
		float m_plane_far = -10.0f;
	};

}} // namespace Albedo::Runtime