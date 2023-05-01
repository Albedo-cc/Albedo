#pragma once

#include <AlbedoPattern.hpp>
#include <AlbedoLog.hpp>

#include <GLFW/glfw3.h>

#include "control_events.h"

#include <array>

namespace Albedo {
namespace Runtime
{
	class WindowSystem;

	class ControlSystem : public pattern::Singleton<ControlSystem>
	{
		friend class pattern::Singleton<ControlSystem>;
		friend class RuntimeModule;
		void Update();

	public:
		void RegisterKeyboardEvent(KeyboardEventCreateInfo createinfo);
		void DeleteKeyboardEvent(EventName name);

		void RegisterMouseScrollEvent(MouseScrollEventCreateInfo createinfo);
		void DeleteMouseScrollEvent(EventName name);

	public:		
		struct Cursor { double x, y; double delta_x, delta_y; };
		static const Cursor& GetCursor() { return sm_cursor; }
		Action::Type GetKeyboardAction(Keyboard::Key key);
		Action::Type GetMouseButtonAction(Mouse::Button button);

	private:
		void Initialize(std::shared_ptr<WindowSystem> window_system);
		ControlSystem() = default;

	private:
		std::shared_ptr<WindowSystem> m_window_system;
		// Keyboard Events
		using KeyboardEventMap = std::unordered_map<Keyboard::Key, std::list<KeyboardEvent>>;
		inline static std::unordered_map<EventName, KeyboardEvent*> m_registry_keyborad_events;
		inline static std::array<KeyboardEventMap, Action::ACTION_TYPE_COUNT> m_keyboard_events;
		
		// Mouse Scroll Events
		inline static std::unordered_map<EventName, MouseScrollEvent*> m_registry_mouse_scroll_events;
		inline static std::list<MouseScrollEvent> m_mouse_scroll_events;

		// Cursor Events ( GetCursor() may help you to fulfill most of problems )
		inline static Cursor sm_cursor;

	private:
		static void keyboard_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
		static void cursor_position_callback(GLFWwindow* window, double position_x, double position_y);
		static void mouse_scroll_callback(GLFWwindow* window, double offset_x, double offset_y);
	};

}} // namespace Albedo::Runtime