#pragma once

#include <Albedo.Core.Math>

namespace Albedo{
namespace APP
{
	
	class GlobalUBO
	{
		friend class Renderer;
	public:
		struct CameraData
		{
			Matrix4x4 view_matrix;
			Matrix4x4 proj_matrix;
		};

	private:
		CameraData placeholder_camera_data;
	};

}} // namespace Albedo::APP