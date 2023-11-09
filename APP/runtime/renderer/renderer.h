#pragma once

#include <Albedo.Pattern>
#include <Albedo.Graphics.Widgets>

#include "data/global_ubo.h"

namespace Albedo{
namespace APP
{
    using namespace Albedo::Graphics;
	
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
            std::weak_ptr<DescriptorSet> ubo;
        };
        auto SearchRenderPass(std::string_view name) const -> const std::unique_ptr<RenderPass>&;
        auto GetFrameContext() -> const FrameContext& { return m_frame_context; }

    private:
        void Initialize();
        void Destroy();
        void Tick();

    private:
        FrameContext m_frame_context;
        std::shared_ptr<Buffer>  m_global_ubo;
        
        std::vector<std::unique_ptr<RenderPass>> m_renderpasses;

        struct Frame
        {
            std::shared_ptr<DescriptorSet> ubo_descriptor_set{};
            Semaphore semaphore_image_available = Semaphore(SemaphoreType_Unsignaled);
            
            struct RenderPassResource
            {
                Semaphore  semaphore =Semaphore(SemaphoreType_Unsignaled);
                std::shared_ptr<CommandBuffer> commandbuffer;
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