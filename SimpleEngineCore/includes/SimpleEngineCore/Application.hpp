#pragma once
#include <memory>
#include "Event.hpp"
#include "SimpleEngineCore/Camera.hpp"

namespace SimpleEngine {

	class Application {
	public:
		Application();
		virtual ~Application();

		Application(const Application&) = delete;
		Application(Application&&) = delete;
		Application& operator=(const Application&) = delete;
		Application& operator=(Application&&) = delete;

		virtual int start(unsigned __int32 window_width, unsigned __int32 window_height, const char* title);
		glm::vec2 get_current_cursor_position();

		virtual void on_update() {}
		virtual void on_ui_draw() {}
		virtual void on_mouse_button_event(const MouseButtonCode button_code,
										   const double x_pos,
										   const double y_pos,
										   const bool pressed) {};

		float camera_position[3] = { 0.0f, 0.0f, 1.0f };
		float camera_rotation[3] = { 0.0f, 0.0f, 0.0f };
		bool is_perspective_mode = true;
		Camera camera{ {-2, 0, 0} };

	private:
		std::unique_ptr<class Window> m_pWindow;
		EventDispatcher m_event_dispatcher;
		bool m_bCloseWindow = false;
	};

}