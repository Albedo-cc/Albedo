#include "camera.h"

namespace Albedo {
namespace Runtime
{

	Camera::Camera(std::shared_ptr<RHI::VulkanContext> vulkan_context, ProjectionMode mode/* = Projection::PERSPECTIVE*/) :
		m_vulkan_context{ std::move(vulkan_context) }
	{
		auto& extent = m_vulkan_context->m_swapchain_current_extent;
		
		m_parameters = Parameter
		{
			.projection_mode = mode,
			.aspect_ratio = static_cast<float>(extent.width) / extent.height
		};
	}

	Matrix4f Camera::GetViewingMatrix(bool update/* = false*/)
	{
		auto& extent = m_vulkan_context->m_swapchain_current_extent;
		// Projection Matrix
		static Matrix4f projection = GetProjectionMatrix();
		if (update) projection = GetProjectionMatrix();

		// View Matrix
		static Matrix4f view = GetViewMatrix();
		update |= m_is_moved;
		m_is_moved = false;
		if (update) view = GetViewMatrix();

		static Matrix4f viewing = projection * view;
		if (update) viewing = projection * view;
		return  viewing;
	}

	Matrix4f Camera::GetViewMatrix()
	{
		Matrix4f view = make_look_at_matrix(m_parameters.position, m_parameters.target, m_parameters.upward);
		return view;
	}

	Matrix4f Camera::GetProjectionMatrix()
	{
		Matrix4f projection;

		auto& extent = m_vulkan_context->m_swapchain_current_extent;
		m_parameters.aspect_ratio = static_cast<float>(extent.width) / extent.height;
		if (ProjectionMode::PERSPECTIVE == m_projection_mode)
		{
			projection = make_perspective_matrix(m_parameters.FOV,
				m_parameters.aspect_ratio, m_parameters.plane_near, m_parameters.plane_far);
		}
		else // ProjectionMode::ORTHOGRAPHICS
		{
			projection = make_orthographics_matrix(0, extent.width, extent.height, 0,
				m_parameters.plane_near, m_parameters.plane_far);
		}
		return projection;
	}

}} // namespace Albedo::Runtime