#pragma once

#include <GLFW/glfw3.h>

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

		GLFWwindow* GetWindow() const { return m_window; }
		const Configuration& GetConfiguration() const { return m_config; }
		bool ShouldClose() const { return glfwWindowShouldClose(m_window); }
		bool IsResized(bool reset = false) { bool res = m_is_resized;  if (reset) m_is_resized = false; return res; }

		WindowSystem();
		~WindowSystem();

	private:
		GLFWwindow* m_window;
		Configuration m_config;
		bool m_is_resized = false;

	private: // Callback Functions
		static void on_frame_buffer_resize(GLFWwindow* window, int width, int height);
	};

}} // namespace Albedo::Runtime