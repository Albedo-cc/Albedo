#include "control_system.h"

#include <runtime/function_layer/window/window_system.h>

namespace Albedo {
namespace Runtime
{
	
	void ControlSystem::Initialize(std::weak_ptr<WindowSystem> window_system)
	{
		m_window_system = std::move(window_system);
		GLFWwindow* window = m_window_system.lock()->GetWindow();
		// Register Callbacks
		glfwSetKeyCallback(window, keyboard_callback);
	}

	void ControlSystem::Update()
	{
		if (!m_window_system.expired()) // glfwPollEvents has been called in WindowSystem
		{
			auto window_system = m_window_system.lock(); // Keep
			GLFWwindow* window = window_system->GetWindow();
			// Handle Keyborad Press and Release Events
			for (auto& [key, press_events] : m_keyboard_events[Action::Press])
			{
				if (Action::Press == glfwGetKey(window, key))
				{
					for (auto press_event = press_events.begin(); 
						press_event != press_events.end(); ++press_event)
					{
						if (*press_event != nullptr) (*press_event)();
						else press_events.erase(press_event);
					}
				}
			}
			for (auto& [key, release_events] : m_keyboard_events[Action::Release])
			{
				if (Action::Release == glfwGetKey(window, key))
				{
					for (auto release_event = release_events.begin(); 
						release_event != release_events.end(); ++release_event)
					{
						if (*release_event != nullptr) (*release_event)();
						else release_events.erase(release_event);
					}
				}
			}

		} // End Update()
	}

	void ControlSystem::RegisterKeyboardEvent(KeyboardEventCreateInfo createinfo)
	{
		assert(!createinfo.name.empty() && "You cannot register an event without name!");
		
		auto newbee = m_registry_keyborad_events.find(createinfo.name);
		if (newbee == m_registry_keyborad_events.end())
		{
			log::info("Albedo Control System: Registering a new keyboard event {}", createinfo.name);
			m_registry_keyborad_events[createinfo.name] =
				&(m_keyboard_events[createinfo.action][createinfo.key].
					emplace_back(std::move(createinfo.event)));
		}
		else log::error("Albedo Control System: Failed to register keyboard event {}", createinfo.name);
	}

	void ControlSystem::DeleteKeyboardEvent(EventName name)
	{
		auto exiler = m_registry_keyborad_events.find(name);
		if (exiler != m_registry_keyborad_events.end())
		{
			log::info("Albedo Control System: Will delete a keyboard event {}", name);
			(*exiler->second) = nullptr;
			m_registry_keyborad_events.erase(exiler);
		}
		else log::error("Albedo Control System: Failed to delete keyboard event {}", name);
	}

	void ControlSystem::keyboard_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
	{
		// Handle Hold and Detach Events (because glfwGetXXX() can only get last state, so Repeat cannot be caught)

		switch (action)
		{
			case Action::Hold:
			{
				auto target = m_keyboard_events[Action::Hold].find(static_cast<Keyboard::Key>(key));
				if (target != m_keyboard_events[Action::Hold].end())
				{
					auto& hold_events = target->second;
					for (auto hold_event = hold_events.begin();
						hold_event != hold_events.end(); ++hold_event)
					{
						if (*hold_event != nullptr) (*hold_event)();
						else hold_events.erase(hold_event);
					}
				}
				break;
			}
			case Action::Release: // Detach is a special Release action
			{
				auto target = m_keyboard_events[Action::Detach].find(static_cast<Keyboard::Key>(key));
				if (target != m_keyboard_events[Action::Detach].end())
				{
					auto& detach_events = target->second;
					for (auto detach_event = detach_events.begin();
						detach_event != detach_events.end(); ++detach_event)
					{
						if (*detach_event != nullptr) (*detach_event)();
						else detach_events.erase(detach_event);
					}
				}
				break;
			}
		}
	}

}} // namespace Albedo::Runtime