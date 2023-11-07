#pragma once

#include <Albedo.Graphics>
#include <Albedo.Pattern>

#include "data/global_ubo.h"

namespace Albedo{
namespace APP
{
	
    class Renderer final
        : public Pattern::Singleton<Renderer>
    {
        friend class Runtime;
        friend class Pattern::Singleton<Renderer>;
        enum RenderPasses { Background, Geometry, MAX_RENDERPASS_COUNT };
    public:
        struct FrameContext
        {
            uint32_t frame_index = 0;
            std::weak_ptr<GRI::DescriptorSet> ubo;
        };
        auto SearchRenderPass(std::string_view name) const -> const std::unique_ptr<GRI::RenderPass>&;
        auto GetFrameContext() -> const FrameContext& { return m_frame_context; }

    private:
        void Initialize();
        void Destroy();
        void Tick();

    private:
        FrameContext m_frame_context;
        std::shared_ptr<GRI::Buffer>  m_global_ubo;
        
        std::vector<std::unique_ptr<GRI::RenderPass>> m_renderpasses;

        struct Frame
        {
            std::shared_ptr<GRI::DescriptorSet> ubo_descriptor_set{};
            GRI::Semaphore semaphore_image_available = GRI::Semaphore(SemaphoreType_Unsignaled);
            
            struct RenderPassResource
            {
                GRI::Semaphore  semaphore =GRI::Semaphore(SemaphoreType_Unsignaled);
                std::shared_ptr<GRI::CommandBuffer> commandbuffer = 
                    GRI::GetGlobalCommandPool(
				    CommandPoolType_Resettable,
				    QueueFamilyType_Graphics)
				    ->AllocateCommandBuffer({ .level = CommandBufferLevel_Primary });
            };
            std::vector<RenderPassResource> renderpasses;
        };
        std::vector<Frame> m_frames;

    private:
        void when_recreate_swapchain();
        
        void create_renderpasses();
        void create_frames();
        
        void create_decriptor_set_layouts();
        void update_frame_context(uint32_t frame_index);

    private:
        Renderer() = default;
    };

}} // namespace Albedo::APP