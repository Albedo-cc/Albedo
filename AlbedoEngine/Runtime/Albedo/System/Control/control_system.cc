#include "control_system.h"
#include "keyboard.h"
#include "mouse.h"

#include <GLFW/glfw3.h>

#include <Albedo/Core/Log/log.h>
#include <Albedo/Core/Norm/assert.h>
#include <Albedo/System/Window/window_system.h>

namespace Albedo
{

	void
	ControlSystem::
	Process()
	{
		GLFWwindow* window = WindowSystem::GetWindow();

		// Handle Keyborad Press and Release Events
		auto& keyboard_events = Keyboard::sm_keyboard_events;
		for (auto& [key, press_events] : keyboard_events[ActionType::Press])
		{
			if (ActionType::Press == glfwGetKey(window, key))
			{
				for (auto press_event_iter = press_events.begin(); 
					 press_event_iter != press_events.end();)
				{
					if (*press_event_iter != nullptr)
					{
						(*press_event_iter)();
						++press_event_iter;
					}
					else press_events.erase(press_event_iter++);
				}
			}
		}
		for (auto& [key, release_events] : keyboard_events[ActionType::Release])
		{
			if (ActionType::Release == glfwGetKey(window, key))
			{
				for (auto release_event_iter = release_events.begin(); 
					 release_event_iter != release_events.end();)
				{
					if (*release_event_iter != nullptr)
					{
						(*release_event_iter)();
						++release_event_iter;
					}
					else release_events.erase(release_event_iter++);
				}
			}
		}
	}

	void
	ControlSystem::
	RegisterKeyboardEvent(const KeyboardEventCreateInfo& createinfo)
	{
		auto& registery = Keyboard::sm_keyborad_event_registry;
		if (registery.find(createinfo.name) == registery.end())
		{
			Log::Info("Creating a keyboard event({}).", createinfo.name);

			registery[createinfo.name] = &Keyboard::sm_keyboard_events
										 [createinfo.action]
										 [createinfo.key]
										 .emplace_back(createinfo.callback);
		}
		else Log::Error("Failed to register the event({}) - duplicated event.", createinfo.name);
	}

	void
	ControlSystem::
	DeleteKeyboardEvent(const std::string& event_name)
	{
		auto& registery = Keyboard::sm_keyborad_event_registry;
		auto exiler = registery.find(event_name);
		if (exiler != registery.end())
		{
			Log::Info("Deleting a keyboard event({}).", event_name);

			(*exiler->second) = nullptr;
			registery.erase(exiler);
		}
		else Log::Error("Failed to delete event({}) - not found.", event_name);
	}

	void
	ControlSystem::
	RegisterMouseButtonEvent(const MouseButtonEventCreateInfo& createinfo)
	{

	}

	void
	ControlSystem::
	DeleteMouseButtonEvent(const std::string& event_name)
	{

	}

	void
	ControlSystem::
	RegisterMouseScrollEvent(const MouseScrollEventCreateInfo& createinfo)
	{

	}

	void
	ControlSystem::
	DeleteMouseScrollEvent(const std::string& event_name)
	{

	}

	void
	ControlSystem::
	Initialize(CreateInfo createinfo)
	{
		sm_settings = std::move(createinfo);

		ALBEDO_ASSERT(ActionType::Release	== 0);
		ALBEDO_ASSERT(ActionType::Press	== 1);
		ALBEDO_ASSERT(ActionType::Hold		== 2);
		ALBEDO_ASSERT(ActionType::Detach	== 3);

		GLFWwindow* window = WindowSystem::GetWindow();
		// Register Callbacks (Before Init Editor)
		if (sm_settings.enable_keyboard)
		glfwSetKeyCallback			(window, callback_keyboard);
		if (sm_settings.enable_mouse_cursor)
		glfwSetCursorPosCallback	(window, callback_mouse_cursor);
		if (sm_settings.enable_mouse_scroll)
		glfwSetScrollCallback		(window, callback_mouse_scroll);
	}

	void
	ControlSystem::
	Terminate()
	{
		Keyboard::sm_keyborad_event_registry.clear();
		for (uint32_t i = 0; i < ActionType::ACTION_TYPE_COUNT; ++i)
		{
			Keyboard::sm_keyboard_events[i].clear();
		}
	}

	void
	ControlSystem::
	callback_keyboard(GLFWwindow* window, int key, int scancode, int action, int mods)
	{
		// Handle Hold and Detach Events (because glfwGetXXX() can only get last state, so Repeat cannot be caught)
		auto& keyboard_events = Keyboard::sm_keyboard_events;

		if (ActionType::Hold == action)
		{
			auto target = keyboard_events[ActionType::Hold].find(static_cast<Keyboard::Key>(key));
			if (target != keyboard_events[ActionType::Hold].end())
			{
				auto& hold_events = target->second;
				for (auto hold_event_iter = hold_events.begin();
					hold_event_iter != hold_events.end();)
				{
					if (*hold_event_iter != nullptr)
					{
						(*hold_event_iter)();
						++hold_event_iter;
					}
					else hold_events.erase(hold_event_iter++);
				}
			}
		}
		else if (ActionType::Release == action)
		{
			auto target = keyboard_events[ActionType::Detach].find(static_cast<Keyboard::Key>(key));
			if (target != keyboard_events[ActionType::Detach].end())
			{
				auto& detach_events = target->second;
				for (auto detach_event_iter = detach_events.begin();
					 detach_event_iter != detach_events.end();)
				{
					if (*detach_event_iter != nullptr)
					{
						(*detach_event_iter)();
						++detach_event_iter;
					}
					else detach_events.erase(detach_event_iter++);
				}
			}
		}
	}

	void
	ControlSystem::
	callback_mouse_scroll(GLFWwindow* window, double offset_x, double offset_y)
	{
		sm_mouse.scroll.dx = offset_x;
		sm_mouse.scroll.dy = offset_y;
	}

	void
	ControlSystem::
	callback_mouse_cursor(GLFWwindow* window, double position_x, double position_y)
	{
		sm_mouse.cursor.dx = position_x - sm_mouse.cursor.x;
		sm_mouse.cursor.dy = position_y - sm_mouse.cursor.y;
		sm_mouse.cursor.x = position_x;
		sm_mouse.cursor.y = position_y;
	}

} // namespace Albedo