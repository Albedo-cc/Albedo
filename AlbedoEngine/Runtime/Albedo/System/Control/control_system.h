#pragma once

#include "action_types.h"

// Predeclaration
typedef struct GLFWwindow GLFWwindow;

namespace Albedo
{
	struct KeyboardEventCreateInfo;
	struct MouseButtonEventCreateInfo;
	struct MouseScrollEventCreateInfo;

	class ControlSystem final
	{
	public: // Info
		struct Mouse
		{
			struct
			{
				double x{ 0 }, dx{ 0 };
				double y{ 0 }, dy{ 0 };
			}cursor;

			struct
			{
				double dx{ 0 }, dy{ 0 };
			}scroll;
		};

	public: // Events
		static void RegisterKeyboardEvent		(const KeyboardEventCreateInfo&	   createinfo);
        static void DeleteKeyboardEvent			(const std::string& event_name);

		static void RegisterMouseButtonEvent	(const MouseButtonEventCreateInfo& createinfo);
        static void DeleteMouseButtonEvent		(const std::string& event_name);

		static void RegisterMouseScrollEvent	(const MouseScrollEventCreateInfo& createinfo);
        static void DeleteMouseScrollEvent		(const std::string& event_name);

	public:
		struct CreateInfo
		{
			bool enable_keyboard;
			bool enable_mouse_cursor;
			bool enable_mouse_scroll;
		};
		static void Initialize(CreateInfo createinfo);
		static void Terminate();
		static void Process();
		 
	private:
		static void callback_keyboard(GLFWwindow* window, int key, int scancode, int action, int mods);
		static void callback_mouse_scroll(GLFWwindow* window, double offset_x, double offset_y);
		static void callback_mouse_cursor(GLFWwindow* window, double position_x, double position_y);

	private:
		static inline CreateInfo sm_settings;
		static inline Mouse sm_mouse{};
	};

} // namespace Albedo