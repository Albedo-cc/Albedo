#include "window_system.h"

namespace Albedo {
namespace Runtime
{

	WindowSystem::WindowSystem()
	{
		glfwInit();

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

		m_window = glfwCreateWindow(m_config.width, m_config.height, m_config.title, nullptr, nullptr);

		//GLFW does not know how to properly call a member function with the right pointer
		glfwSetWindowUserPointer(m_window, this);
		glfwSetFramebufferSizeCallback(m_window, on_frame_buffer_resize);
	}

	WindowSystem::~WindowSystem()
	{
		glfwDestroyWindow(m_window);
		glfwTerminate();
	}

	// Callback Functions
	
	void WindowSystem::on_frame_buffer_resize(GLFWwindow* window, int width, int height)
	{
		auto* window_config = reinterpret_cast<WindowSystem*>(glfwGetWindowUserPointer(window));
		window_config->m_is_resized = true;
	}

}} // namespace Albedo::Runtime