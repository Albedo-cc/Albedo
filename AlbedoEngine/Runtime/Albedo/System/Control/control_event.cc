#include "control_event.h"
#include <GLFW/glfw3.h>

namespace Albedo
{
	//---------------------------------------------------------------------------------------------------//
	Keyboard::Key
	//Keyboard::Alphabat
	Keyboard::A = GLFW_KEY_A, Keyboard::B = GLFW_KEY_B, Keyboard::C = GLFW_KEY_C, Keyboard::D = GLFW_KEY_D,
	Keyboard::E = GLFW_KEY_E, Keyboard::F = GLFW_KEY_F, Keyboard::G = GLFW_KEY_G, Keyboard::H = GLFW_KEY_H,
	Keyboard::I = GLFW_KEY_I, Keyboard::J = GLFW_KEY_J, Keyboard::K = GLFW_KEY_K, Keyboard::L = GLFW_KEY_L,
	Keyboard::M = GLFW_KEY_M, Keyboard::N = GLFW_KEY_N, Keyboard::O = GLFW_KEY_O, Keyboard::P = GLFW_KEY_P,
	Keyboard::Q = GLFW_KEY_Q, Keyboard::R = GLFW_KEY_R, Keyboard::S = GLFW_KEY_S, Keyboard::T = GLFW_KEY_T,
	Keyboard::U = GLFW_KEY_U, Keyboard::V = GLFW_KEY_V, Keyboard::W = GLFW_KEY_W, Keyboard::X = GLFW_KEY_X,
	Keyboard::Y = GLFW_KEY_Y, Keyboard::Z = GLFW_KEY_Z,

	//Keyboard::Numbers
	Keyboard::Num0 = GLFW_KEY_0,  Keyboard::Num1 = GLFW_KEY_1, Keyboard::Num2 = GLFW_KEY_2,  
	Keyboard::Num3 = GLFW_KEY_3,  Keyboard::Num4 = GLFW_KEY_4, Keyboard::Num5 = GLFW_KEY_5, 
	Keyboard::Num6 = GLFW_KEY_6,  Keyboard::Num7 = GLFW_KEY_7, Keyboard::Num8 = GLFW_KEY_8,
	Keyboard::Num9 = GLFW_KEY_9, 

	//Keyboard::FX
	Keyboard::F1  =  GLFW_KEY_F1,   Keyboard::F2 =  GLFW_KEY_F2,  Keyboard::F3 =  GLFW_KEY_F3,
	Keyboard::F4  =  GLFW_KEY_F4,   Keyboard::F5 =  GLFW_KEY_F5,  Keyboard::F6 =  GLFW_KEY_F6,
	Keyboard::F7  =  GLFW_KEY_F7,   Keyboard::F8 =  GLFW_KEY_F8,  Keyboard::F9 =  GLFW_KEY_F9,
	Keyboard::F10 =  GLFW_KEY_F10,  Keyboard::F11=  GLFW_KEY_F11, Keyboard::F12=  GLFW_KEY_F12,

	//Keyboard::Others
	Keyboard::Space  = GLFW_KEY_SPACE,
	Keyboard::LShift = GLFW_KEY_LEFT_SHIFT,
	Keyboard::RShift = GLFW_KEY_RIGHT_SHIFT,
	Keyboard::ESC    = GLFW_KEY_ESCAPE,
	Keyboard::Enter  = GLFW_KEY_ENTER,
	Keyboard::Tab	 = GLFW_KEY_TAB;
	//---------------------------------------------------------------------------------------------------//

	//---------------------------------------------------------------------------------------------------//
	Mouse::Button
	Mouse::Left	  = GLFW_MOUSE_BUTTON_LEFT,
	Mouse::Middle = GLFW_MOUSE_BUTTON_MIDDLE,
	Mouse::Right  = GLFW_MOUSE_BUTTON_RIGHT;
	//---------------------------------------------------------------------------------------------------//

} //Keyboard:: namespace Albedo