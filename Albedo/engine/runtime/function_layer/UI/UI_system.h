#pragma once

#include <AlbedoLog.hpp>
#include <AlbedoRHI.hpp>

namespace Albedo {
namespace Runtime
{

	class UISystem
	{
	public:
		void Initialize(std::shared_ptr<RHI::RenderPass> render_pass, uint32_t subpass);
		void Render(std::shared_ptr<RHI::CommandBuffer> commandBuffer); // Call in Render System

	public:
		UISystem() = delete;
		UISystem(std::shared_ptr<RHI::VulkanContext> vulkan_context);
		~UISystem();

	private:
		std::shared_ptr<RHI::VulkanContext> m_vulkan_context;
		std::shared_ptr<RHI::DescriptorPool> m_descriptor_pool;

		std::optional<bool> m_should_render; // Init after calling Initialize()

	private:
		void create_descriptor_pool();
	};

}} // namespace Albedo::Runtime