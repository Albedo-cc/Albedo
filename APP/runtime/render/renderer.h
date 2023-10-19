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
        enum RenderPasses { Geometry };
        static inline std::vector<GRI::RenderPass*> sm_renderpasses;
        struct Frame
        {
            std::shared_ptr<GRI::CommandBuffer> commandbuffer_geometry;
            GRI::Semaphore  semaphore_image_available = GRI::Semaphore(SemaphoreType_Unsignaled);
            GRI::Semaphore  semaphore_geometry_pass   = GRI::Semaphore(SemaphoreType_Unsignaled);
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