#pragma once

#include <AlbedoLog.hpp>

#include <GLFW/glfw3.h>

#include <functional>

namespace Albedo {
namespace Runtime
{
	
	class WindowSystem
	{
		friend class RuntimeModule;
		void Update() { glfwPollEvents(); }
	public:
		struct Parameters
		{
			const char* title = "Albedo";
			int width		=	1280;		// If Zero: Auto Monitor Size
			int height	=	800;			// If Zero: Auto Monitor Size
			bool maximize = true;
		};
	public:
		bool ShouldClose() const { return glfwWindowShouldClose(m_window); }

		GLFWwindow* GetWindow() const { return m_window; }
		const Parameters& GetParameters() const { return m_config; }

		WindowSystem();
		~WindowSystem();

	private:
		GLFWwindow* m_window = NULL;
		Parameters m_config;

	private: // Callback Functions
		static void on_frame_buffer_resize(GLFWwindow* window, int width, int height);
		static void SetFramebufferResizeCallback(const std::function<void()>& callback) { frame_buffer_resize_callback = callback; }
		inline static std::function<void()> frame_buffer_resize_callback;
	};

}} // namespace Albedo::Runtime