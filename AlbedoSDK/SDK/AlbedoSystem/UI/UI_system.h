#pragma once

#include <imgui.h>

#include <list>
#include <string>
#include <string_view>
#include <functional>
#include <unordered_map>

#include <AlbedoGraphics/GRI.h>
#include <AlbedoCore/Event/event_manager.h>

namespace Albedo
{
    using UIEvent = BasicEvent;

    class UISystem final
    {
    public:
        static void RegisterUIEvent(UIEvent* event);
        static void DeleteUIEvent(std::string_view name);

    public:
        struct CreateInfo
        {
            VkRenderPass     renderpass;
            uint32_t         subpass;
            VkDescriptorPool descriptor_pool;
        };
        static void Initialize(const CreateInfo& createinfo);
        static void Terminate() noexcept;
        static void Process(std::shared_ptr<GRI::CommandBuffer> commandbuffer);

    private:
        static inline EventManager m_ui_event_manager;

    private:
        UISystem()                          = delete;
        ~UISystem()                         = delete;
        UISystem(const UISystem&)           = delete;
        UISystem(UISystem&&)                = delete;
        UISystem& operator=(const UISystem&)= delete;
        UISystem& operator=(UISystem&&)     = delete;
    };

} // namespace Albedo