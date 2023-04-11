#include "render_system.h"


#include "render_pass/forward_rendering/forward_render_pass.h"

namespace Albedo {
namespace Runtime
{
	void RenderSystem::Update()
	{
			// Wait for previous frame
			auto & current_frame_state = RenderSystemContext::GetCurrentFrameState();
			current_frame_state.m_fence_in_flight->Wait();
			try
			{
				wait_for_next_image_index(current_frame_state);
				current_frame_state.m_fence_in_flight->Reset();

				auto& current_commandbuffer = current_frame_state.m_command_buffer;

				static UniformBuffer UBO;
				static time::StopWatch timer{};

				UBO.matrix_model = glm::rotate(glm::mat4x4(1.0f),
														0.1f * (float)ONE_DEGREE * static_cast<float>(timer.split().milliseconds()),
														glm::vec3(0.0f, 0.0f, 1.0f));

				//make_rotation_matrix(WORLD_AXIS_Z,  * timer.split().milliseconds()).setIdentity();
				UBO.matrix_view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f),
																		glm::vec3(0.0f, 0.0f, 0.0f),
																		glm::vec3(0.0f, 0.0f, 1.0f));
				//m_camera.GetViewMatrix();//m_camera.GetViewingMatrix();
				UBO.matrix_projection = glm::perspective(glm::radians(45.0f),
															(float)m_vulkan_context->m_swapchain_current_extent.width /
															(float)m_vulkan_context->m_swapchain_current_extent.height,
															0.1f,
															10.0f);
				UBO.matrix_projection[1][1] *= -1.0f;
				//m_camera.GetProjectionMatrix();

				current_frame_state.m_uniform_buffer->Write(&UBO);
				current_frame_state.m_global_descriptor_set->WriteBuffer(
					VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
					RenderSystemContext::global_descriptor_set_binding_matrics,
					current_frame_state.m_uniform_buffer);

				current_frame_state.m_global_descriptor_set->WriteImage(
					VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
					RenderSystemContext::global_descriptor_set_binding_textures,
					m_image);

				current_commandbuffer->Begin();
				{
					m_render_passes[render_pass_forward]->Begin(current_commandbuffer);
					{
						auto& pipelines = m_render_passes[render_pass_forward]->GetGraphicsPipelines();

						pipelines[ForwardRenderPass::pipeline_present]->Bind(current_commandbuffer);

						VkDescriptorSet descriptorSets[] = { *(current_frame_state.m_global_descriptor_set) };
						vkCmdBindDescriptorSets(*current_commandbuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
							pipelines[ForwardRenderPass::pipeline_present]->GetPipelineLayout(), 0, 1, descriptorSets, 0, nullptr);
						for (auto& model : m_models)
						{
							model.Draw(*current_commandbuffer);
						}
					}
					m_render_passes[render_pass_forward]->End(current_commandbuffer);
				}
				current_commandbuffer->End();

				current_commandbuffer->Submit(false,
					*current_frame_state.m_fence_in_flight,
					{ *current_frame_state.m_semaphore_image_available },
					{ *current_frame_state.m_semaphore_render_finished },
					VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT);

				m_vulkan_context->PresentSwapChain(*current_frame_state.m_semaphore_render_finished);

				RenderSystemContext::SwitchToNextFrame();
			}
			catch (RHI::VulkanContext::swapchain_error& swapchian_recreation)
			{
				handle_window_resize();
			}
	}


	RenderSystem::RenderSystem(std::weak_ptr<WindowSystem> window_system) :
		m_vulkan_context{ RHI::VulkanContext::Create(window_system.lock()->GetWindow()) },
		m_window_system{ std::move(window_system) },
		m_camera{ m_vulkan_context }
	{
		time::StopWatch timer{};
		// Async Loading Assets
		auto image_futures = begin_loading_images();
		auto model_futures = AssetManager::instance().AsyncLoadModel("Cube/Cube.gltf", ASSET_USAGE_RENDER_BIT);
		// Initialize Render System Context
		RenderSystemContext::Initialize(m_vulkan_context);

		// Render Passes
		create_render_passes();

		// Load Assets
		load_models();
		end_loading_images(image_futures);
		auto model = model_futures->WaitResult();
		log::info("Render System Initialization Time : {} ms", timer.split().microseconds());
	}

	void RenderSystem::wait_for_next_image_index(FrameState& current_frame_state)
	{
		if (m_window_system.lock()->IsResized(true)) 
			throw RHI::VulkanContext::swapchain_error();

		m_vulkan_context->NextSwapChainImageIndex(
			*current_frame_state.m_semaphore_image_available, VK_NULL_HANDLE);
	}

	void RenderSystem::create_render_passes()
	{
		m_render_passes.clear();
		m_render_passes.resize(MAX_RENDER_PASS_COUNT);
		m_render_passes[render_pass_forward] = std::make_unique<ForwardRenderPass>(m_vulkan_context);
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

	std::vector<std::shared_ptr<ImageFuture>> RenderSystem::
		begin_loading_images()
	{
		std::vector<std::shared_ptr<ImageFuture>> image_tasks(1);
		image_tasks[0] = AssetManager::instance().AsyncLoadTexture2D("watch_tower_512x512.png");
		return image_tasks;
	}

	void RenderSystem::end_loading_images(std::vector<std::shared_ptr<ImageFuture>> image_futures)
	{
		auto sampler = m_vulkan_context->CreateSampler(VK_SAMPLER_ADDRESS_MODE_REPEAT);
		
		for (auto& image_future : image_futures)
		{
			auto image_data = image_future->WaitResult();
			m_image = m_vulkan_context->m_memory_allocator->
				AllocateImage(VK_IMAGE_ASPECT_COLOR_BIT,
					VK_IMAGE_USAGE_SAMPLED_BIT,
					image_data->width, image_data->height, image_data->channel,
					VK_FORMAT_R8G8B8A8_SRGB);
			m_image->Write(image_data->data);
			m_image->BindSampler(sampler);
		}
	}

	void RenderSystem::handle_window_resize()
	{
		log::warn("Window Resized!");
		m_vulkan_context->RecreateSwapChain();
		create_render_passes();		 // Recreate Render Passes
		m_camera.GetViewingMatrix(true); // Update
	}

}} // namespace Albedo::Runtime