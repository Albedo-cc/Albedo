#pragma once

#include <Albedo.Graphics>
#include <Albedo.Pattern>

namespace Albedo{
namespace APP
{
	
    class Renderer final
        : public Pattern::Singleton<Renderer>
    {
        friend class Runtime;
        friend class Pattern::Singleton<Renderer>;
    public:
        auto SearchRenderPass(std::string_view name) throw(std::runtime_error) -> const GRI::RenderPass*;

    private:
        void Initialize();
        void Destroy();
        void Tick();

    private:
        enum RenderPasses { Background, Geometry };
        std::vector<GRI::RenderPass*> m_renderpasses;
        struct Frame
        {
            GRI::Semaphore semaphore_image_available = GRI::Semaphore(SemaphoreType_Unsignaled);
            struct RenderPassResource
            {
                GRI::Semaphore semaphore = GRI::Semaphore(SemaphoreType_Unsignaled);
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
        void create_decriptor_set_layouts();
        void create_renderpasses();
        void destory_renderpasses();
        void create_frames();
        void destory_frames();

    private:
        Renderer() = default;
    };

}} // namespace Albedo::APP