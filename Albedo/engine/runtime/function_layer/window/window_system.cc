#include "window_system.h"

#include <runtime/asset_layer/asset_manager.h>

namespace Albedo {
namespace Runtime
{
	std::function<void()> WindowSystem::frame_buffer_resize_callback = nullptr;

	WindowSystem::WindowSystem()
	{
		glfwInit();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

		m_window = glfwCreateWindow(m_config.width, m_config.height, m_config.title, nullptr, nullptr);

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

		if (frame_buffer_resize_callback) _window->frame_buffer_resize_callback();
	}

}} // namespace Albedo::Runtime