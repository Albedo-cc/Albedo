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

	private:
		void Initialize(std::weak_ptr<WindowSystem> window_system);
		ControlSystem() = default;

	private:
		std::weak_ptr<WindowSystem> m_window_system;

		using KeyboardEventMap = std::unordered_map<Keyboard::Key, std::list<ControlEvent>>;
		inline static std::unordered_map<EventName, ControlEvent*> m_registry_keyborad_events;
		inline static std::array<KeyboardEventMap, Action::ACTION_TYPE_COUNT> m_keyboard_events;

	private:
		static void keyboard_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
	};

}} // namespace Albedo::Runtime