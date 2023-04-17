#pragma once

#include <AlbedoPattern.hpp>
#include <AlbedoLog.hpp>

#include <GLFW/glfw3.h>

#include "control_events.h"

namespace Albedo {
namespace Runtime
{

	class ControlSystem : public pattern::Singleton<ControlSystem>
	{
		friend class pattern::Singleton<ControlSystem>;
		friend class RuntimeModule;
		void Update();

	public:
		void RegisterKeyboardEvent(std::string name, KeyboardEvent event);
		void RegisterMouseButtonEvent(std::string name, MouseButtonEvent event);

		void DeleteKeyboardEvent(std::string name);
		void DeleteMouseEvent(std::string name);

	private:
		void Initialize(GLFWwindow* window);
		ControlSystem() = default;

	private:
		inline static std::unordered_map<std::string, KeyboardEvent> m_keyborad_events;
		inline static std::unordered_map<std::string, MouseButtonEvent> m_mouse_button_events;

	private:
		static void keyboard_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
		static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
	};

}} // namespace Albedo::Runtime