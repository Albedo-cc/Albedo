#include "window_system.h"

#include <AlbedoCore/Log/log.h>

#include <GLFW/glfw3.h>
#include <exception>

namespace Albedo
{

    void WindowSystem::Initialize(WindowSystem::CreateInfo createinfo)
    {
		Log::Debug("Albedo Window System is being initialized...");

        // Init GLFW
        glfwInit();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE,	GLFW_TRUE);

        // Init System Parameters
        m_parameters = std::move(createinfo);
        if (!m_parameters.width || !m_parameters.height)
		{
			GLFWmonitor* monitor = glfwGetPrimaryMonitor();
			const GLFWvidmode* mode = glfwGetVideoMode(monitor);
			m_parameters.width = mode->width;
			m_parameters.height = mode->height;
		}

		// Create Window
		m_window = glfwCreateWindow(
			m_parameters.width, 
			m_parameters.height, 
			m_parameters.title, NULL, NULL);
		if (!m_window) Log::Fatal("Failed to create GLFWwindow!");

        // Set Window Position
        GLFWmonitor* monitor = glfwGetPrimaryMonitor();
		const GLFWvidmode* mode = glfwGetVideoMode(monitor);
		glfwSetWindowPos(m_window,
			(mode->width    -   m_parameters.width ) >> 1,	// Mid
			(mode->height   -   m_parameters.height) >> 1);	// Mid
		if (m_parameters.maximize) glfwMaximizeWindow(m_window);

        //GLFW does not know how to properly call a member function with the right pointer
		//glfwSetWindowUserPointer(m_window, this);
		//glfwSetFramebufferSizeCallback(m_window, on_frame_buffer_resize);

		//// Set Icon
		//auto icon_image = AssetManager::instance().LoadIcon("Albedo_32x32.png");
		//GLFWimage icon
		//{
		//	.width = icon_image->width,
		//	.height = icon_image->height,
		//	.pixels = icon_image->data
		//};
		//glfwSetWindowIcon(m_window, 1, &icon);
    }

	void WindowSystem::Terminate() noexcept
	{
		Log::Debug("Albedo Window System is being terminated...");
		glfwDestroyWindow(m_window);
		glfwTerminate();
	}

    bool WindowSystem::IsDisplaying()
    {
        return !glfwWindowShouldClose(m_window);
    }

	void WindowSystem::PollEvents()
	{
		glfwPollEvents();
	}

} // namespace Albedo