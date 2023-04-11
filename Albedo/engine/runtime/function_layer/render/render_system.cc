#include "render_system.h"


#include "render_pass/forward_rendering/forward_render_pass.h"

namespace Albedo {
namespace Runtime
{
	void RenderSystem::Update()
	{
		try
		{
			auto& canvas = m_easel.GetCanvas(); // Wait for Next Canvas
			auto& palette = canvas.GetPalette();
			
			static time::StopWatch timer{};

			auto& camera_data = palette.GetCameraMatrics();
			camera_data.matrix_model = glm::rotate(glm::mat4x4(1.0f),
													0.1f * (float)ONE_DEGREE * static_cast<float>(timer.split().milliseconds()),
													glm::vec3(0.0f, 0.0f, 1.0f));

			//make_rotation_matrix(WORLD_AXIS_Z,  * timer.split().milliseconds()).setIdentity();
			camera_data.matrix_view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f),
																	glm::vec3(0.0f, 0.0f, 0.0f),
																	glm::vec3(0.0f, 0.0f, 1.0f));
			//m_camera.GetViewMatrix();//m_camera.GetViewingMatrix();
			camera_data.matrix_projection = glm::perspective(glm::radians(45.0f),
														(float)m_vulkan_context->m_swapchain_current_extent.width /
														(float)m_vulkan_context->m_swapchain_current_extent.height,
														0.1f,
														10.0f);
			camera_data.matrix_projection[1][1] *= -1.0f;

			auto cmdbuffer = canvas.BeginPainting(m_render_passes[render_pass_forward]);
			{
				auto& pipelines = m_render_passes[render_pass_forward]->GetGraphicsPipelines();

				for (auto& model : m_models)
				{
					canvas.Paint(pipelines[ForwardRenderPass::pipeline_present], model);
				}
			}
			canvas.EndPainting(m_render_passes[render_pass_forward]);

			m_easel.PresentCanvas();
		}
		catch (RHI::VulkanContext::swapchain_error& swapchian_recreation)
		{
			handle_window_resize();
		}
	}

	RenderSystem::RenderSystem(std::weak_ptr<WindowSystem> window_system) :
		m_vulkan_context{ RHI::VulkanContext::Create(window_system.lock()->GetWindow()) },
		m_window_system{ std::move(window_system) },
		m_camera{ m_vulkan_context },
		m_easel{ m_vulkan_context }
	{
		// Window
		WindowSystem::SetFramebufferResizeCallback([this]() { handle_window_resize(); });

		// Render Passes
		create_render_passes();

		load_models();
	}

	void RenderSystem::create_render_passes()
	{
		m_render_passes.clear();
		m_render_passes.resize(MAX_RENDER_PASS_COUNT);
		m_render_passes[render_pass_forward] = std::make_shared<ForwardRenderPass>(m_vulkan_context);
	}

	void RenderSystem::load_models()
	{
		static std::vector<ModelVertexIndex>
		triangle_indices
		{
			0, 1, 2, 2, 3, 0,
			4, 5, 6, 6, 7, 4
		};

		static std::vector<ModelVertex> 
		triangle_vertices
		{	// [ X		Y		  Z ]		[ R		G		  B ]	  [ U		   V]
			{ {-0.5f, -0.5f, 0.0f},	{1.0f, 1.0f, 1.0f}, {1.0f, 0.0f} },
			{ {0.5f, -0.5f, 0.0f},		{0.0f, 1.0f, 0.0f}, {0.0f, 0.0f} },
			{ {0.5f, 0.5f, 0.0f},		{0.0f, 0.0f, 1.0f}, {0.0f, 1.0f} },
			{ {-0.5f, 0.5f, 0.0f},		{1.0f, 1.0f, 1.0f}, {1.0f, 1.0f} },

			{ {-0.5f, -0.5f, -0.5f},	{1.0f, 1.0f, 1.0f}, {1.0f, 0.0f} },
			{ {0.5f, -0.5f, -0.5f},	{0.0f, 1.0f, 0.0f}, {0.0f, 0.0f} },
			{ {0.5f, 0.5f, -0.5f},		{0.0f, 0.0f, 1.0f}, {0.0f, 1.0f} },
			{ {-0.5f, 0.5f, -0.5f},	{1.0f, 1.0f, 1.0f}, {1.0f, 1.0f} }
		};
		m_models.emplace_back(m_vulkan_context, triangle_vertices, triangle_indices, 0);
	}

	void RenderSystem::handle_window_resize()
	{
		log::warn("Window Resized!");
		m_vulkan_context->RecreateSwapChain();
		create_render_passes();		 // Recreate Render Passes
		m_camera.GetViewingMatrix(true); // Update
	}

}} // namespace Albedo::Runtime