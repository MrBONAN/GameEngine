#pragma once
#include "SimpleEngineCore/Keys.hpp"

namespace SimpleEngine {

	class Input {
	public:
		static bool IsKeyPressed(const KeyCode key_code);
		static void PressKey(const KeyCode key_code);
		static void ReleaseKey(const KeyCode key_code);

		static bool IsMouseButtonPressed(const MouseButtonCode mouse_button_code);
		static void PressMouseButton(const MouseButtonCode mouse_button_code);
		static void ReleaseMouseButton(const MouseButtonCode mouse_button_code);
	private:
		static bool m_keys_pressed[];
		static bool m_mouse_buttons_pressed[];
	};

}