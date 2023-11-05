#pragma once

// Predeclaration
struct GLFWwindow;

namespace Albedo
{
    
    class WindowSystem final
    {
    public:
        struct CreateInfo
        {
            const char* title = "Albedo";
            int width     = 1280;
            int height    = 800;
            bool maximize = true;
        };
        static void Initialize(WindowSystem::CreateInfo createinfo);
        static void Terminate() noexcept;

    public:
        static auto IsDisplaying()  -> bool;
        static void Process();

        static auto GetWindow()     -> GLFWwindow* const { return m_window; }
        static auto GetParameters() -> const CreateInfo& { return m_parameters; }
        static auto GetWidth()      -> int               { return m_parameters.width; }
        static auto GetHeight()     -> int               { return m_parameters.height; }

        //auto SetParameters() -> void

    private:
        static inline GLFWwindow* m_window = nullptr;
        static inline CreateInfo  m_parameters;

    private:
        WindowSystem()                              = delete;
        ~WindowSystem()                             = delete;
        WindowSystem(const WindowSystem&)           = delete;
        WindowSystem(WindowSystem&&)                = delete;
        WindowSystem& operator=(const WindowSystem&)= delete;
        WindowSystem& operator=(WindowSystem&&)     = delete;
    };

} // namespace Albedo