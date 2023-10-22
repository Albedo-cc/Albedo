#pragma once

#include <Albedo.Graphics>

namespace Albedo{
namespace APP
{
	
    class Renderer
    {
    public:
        static void Initialize();
        static void Destroy();
        static void Tick();

        static auto SearchRenderPass(std::string_view name) throw(std::runtime_error) -> const GRI::RenderPass*;

    private:
        enum RenderPasses { Background, Geometry };
        static inline std::vector<GRI::RenderPass*> sm_renderpasses;
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
        static inline std::vector<Frame> sm_frames;

    private:
        static void create_decriptor_set_layouts();
        static void create_renderpasses();
        static void destory_renderpasses();
        static void create_frames();
        static void destory_frames();

    private:
        Renderer() = delete;
    };

}} // namespace Albedo::APP