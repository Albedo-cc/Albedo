#include "camera.h"

namespace Albedo {
namespace Runtime
{

	Camera::Camera(ProjectionMode mode/* = Projection::PERSPECTIVE*/) :
		m_projection_mode{ mode }
	{
		GetViewingMatrix(true);
	}

	Matrix4f Camera::GetViewingMatrix(bool update/* = false*/)
	{
		static Matrix4f projection;
		static Matrix4f view_translate;
		static Matrix4f view_rotate;
		static auto view_rotate_block = view_rotate.block(0, 0, 3, 3);

		// Projection Matrix
		if (update)
		{
			view_translate.setIdentity();
			view_rotate(3, 3) = 1;
			Matrix4f prjection_translate;	prjection_translate.setIdentity();
			Matrix4f prjection_rotate;		prjection_rotate.setIdentity();
			auto& extent = m_vulkan_context->m_swapchain_current_extent;
			if (ProjectionMode::PERSPECTIVE == m_projection_mode)
			{

			}
			else //ProjectionMode::ORTHOGRAPHICS
			{
				prjection_translate.col(3) = Vector4f{ -extent.width / 2.0f,  -extent.height / 2.0f, -(m_plane_near + m_plane_far) / 2.0f, 1.0f };
				prjection_rotate.diagonal() = Vector4f{ 2.0f / extent.width , 2.0f / extent.height, 2.0f / m_plane_near - m_plane_far, 1.0f };
			}
			projection = prjection_rotate * prjection_translate;
		}

		// View Matrix
		view_translate.col(3) = (-m_position).homogeneous();
		view_rotate_block.row(0) = m_lookat.cross(m_updirection);
		view_rotate_block.row(1) = m_updirection;
		view_rotate_block.row(2) = -m_lookat;

		return projection * (view_rotate * view_translate); // Viewing Matrix
	}

}} // namespace Albedo::Runtime