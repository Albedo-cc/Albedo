#pragma once

#include <GLFW/glfw3.h>

#include <functional>

namespace Albedo {
namespace Runtime
{
	
	class WindowSystem
	{
	public:
		struct Configuration
		{
			const char* title = "Albedo";
			int width = 1080;
			int height = 720;
		};
	public:
		void Update() { glfwPollEvents(); }
		bool ShouldClose() const { return glfwWindowShouldClose(m_window); }

		static void SetFramebufferResizeCallback(const std::function<void()>& callback){ frame_buffer_resize_callback = callback; }
		GLFWwindow* GetWindow() const { return m_window; }
		const Configuration& GetConfiguration() const { return m_config; }

		WindowSystem();
		~WindowSystem();

	private:
		GLFWwindow* m_window;
		Configuration m_config;

	private: // Callback Functions
		static void on_frame_buffer_resize(GLFWwindow* window, int width, int height);
		static std::function<void()> frame_buffer_resize_callback;
	};

}} // namespace Albedo::Runtime