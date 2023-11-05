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
        enum RenderPasses { /*Background,*/ Geometry, MAX_RENDERPASS_COUNT };
    public:
        auto SearchRenderPass(std::string_view name) const -> const std::unique_ptr<GRI::RenderPass>&;

    private:
        void Initialize();
        void Destroy();
        void Tick();

    private:
        std::shared_ptr<GRI::Buffer>  m_global_ubo;
        std::vector<std::unique_ptr<GRI::RenderPass>> m_renderpasses;

        struct Frame
        {
            GlobalUBO      ubo_data{};
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
        void update_global_ubo(Frame& current_frame);

    private:
        Renderer() = default;
    };

}} // namespace Albedo::APP