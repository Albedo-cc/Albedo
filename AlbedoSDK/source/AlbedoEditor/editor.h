#pragma once

#include <imgui.h>

#include "../AlbedoGraphics/GRI.h"
#include "../AlbedoCore/Event/event_manager.h"

namespace Albedo
{
    using UIEvent = BasicEvent;
    class EditorPass;

    class Editor final
    {
        friend class GRI;
        struct FrameInfo;
    public: 
        static void Enable(const char* signature);
        static void Disable(const char* signature);

        static auto IsEnabled() -> bool { return m_enabled; }

        static void RegisterUIEvent(UIEvent* event); // new UIEvent()
        static void DeleteUIEvent(std::string_view name);

    public:
        struct CreateInfo
        {
            const char* font_path;
            float       font_size;
        };
        static void Initialize(const CreateInfo& createinfo);
        static void Terminate() noexcept;

    private:
        static auto Render() -> FrameInfo&;
        static void Recreate();

    private:
        static inline EventManager sm_ui_event_manager;
        static inline std::shared_ptr<EditorPass> sm_renderpass;
        struct FrameInfo
        {
            std::shared_ptr<GRI::Texture2D>     main_camera;
            std::shared_ptr<GRI::CommandBuffer> commandbuffer;
            std::shared_ptr<GRI::DescriptorSet> descriptor_set;
            GRI::Semaphore semaphore_editor{ SemaphoreType_Unsignaled };
        };
        static inline ImVec2 sm_main_camera_extent{ 600, 300 };
        static inline std::vector<FrameInfo> sm_frame_infos;
        static inline bool m_enabled = false;

    private:
        Editor()                         = delete;
        ~Editor()                        = delete;
        Editor(const Editor&)            = delete;
        Editor(Editor&&)                 = delete;
        Editor& operator=(const Editor&) = delete;
        Editor& operator=(Editor&&)      = delete;
    };

} // namespace Albedo