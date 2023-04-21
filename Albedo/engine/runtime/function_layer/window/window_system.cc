#include "window_system.h"

#include <runtime/asset_layer/asset_manager.h>

namespace Albedo {
namespace Runtime
{

	WindowSystem::WindowSystem()
	{
		glfwInit();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

		if (!m_config.width || !m_config.height)
		{
			GLFWmonitor* monitor = glfwGetPrimaryMonitor();
			const GLFWvidmode* mode = glfwGetVideoMode(monitor);
			m_config.width = mode->width;
			m_config.height = mode->height;
		}

		m_window = glfwCreateWindow(m_config.width, m_config.height, m_config.title, NULL, NULL);
		if (!m_window) throw std::runtime_error("Failed to create GLFWwindow!");

		// Set Window Position
		GLFWmonitor* monitor = glfwGetPrimaryMonitor();
		const GLFWvidmode* mode = glfwGetVideoMode(monitor);
		glfwSetWindowPos(m_window,
			(mode->width - m_config.width) >> 1,		// Mid
			(mode->height - m_config.height) >> 1);	// Mid
		if (m_config.maximize) glfwMaximizeWindow(m_window);


		//GLFW does not know how to properly call a member function with the right pointer
		glfwSetWindowUserPointer(m_window, this);
		glfwSetFramebufferSizeCallback(m_window, on_frame_buffer_resize);
	
		// Set Icon
		auto icon_image = AssetManager::instance().LoadIcon("Albedo_32x32.png");
		GLFWimage icon
		{
			.width = icon_image->width,
			.height = icon_image->height,
			.pixels = icon_image->data
		};
		glfwSetWindowIcon(m_window, 1, &icon);
	}

	WindowSystem::~WindowSystem()
	{
		glfwDestroyWindow(m_window);
		glfwTerminate();
	}

	// Callback Functions
	
	void WindowSystem::on_frame_buffer_resize(GLFWwindow* window, int width, int height)
	{
		auto* _window = reinterpret_cast<WindowSystem*>(glfwGetWindowUserPointer(window));

		_window->m_config.height = height;
		_window->m_config.width = width;
		log::info("Current Window Size {}, {}", width, height);

		if (frame_buffer_resize_callback) _window->frame_buffer_resize_callback();
	}

}} // namespace Albedo::Runtime