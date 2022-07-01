#include "SimpleEngineCore/Application.hpp"
#include "SimpleEngineCore/Log.hpp" 
#include "SimpleEngineCore/Window.hpp"

#include <iostream>

namespace SimpleEngine {
	Application::Application() {
        LOG_INFO("Starting Application");
	}
	Application::~Application() {
		LOG_INFO("Closing Application");
	}

	int Application::start(unsigned __int32 window_width, unsigned __int32 window_height, const char* title) {
		m_pWindow = std::make_unique<Window>(title, window_width, window_height);

		m_pWindow->set_event_callback(
			[&](BaseEvent& event) {
				m_event_dispatcher.dispatch(event);
			});


		m_event_dispatcher.add_event_listener<EventWindowResize>(
			[](EventWindowResize& event) {
				LOG_INFO("[WindowResized] Change size to {0}x{1}", event.width, event.height);
			});

		m_event_dispatcher.add_event_listener<EventWindowClose>(
			[&](EventWindowClose& event) {
				LOG_INFO("[WindowClose] Window closed");
				m_bCloseWindow = true;
			});

		m_event_dispatcher.add_event_listener<EventMouseMoved>(
			[](EventMouseMoved& event) {
				LOG_INFO("[MouseMoved] Mouse moved to {0}-X {1}-Y", event.x, event.y);
			});

		while (!m_bCloseWindow) {
			m_pWindow->on_update();
			on_update();
		}

		m_pWindow = nullptr;

		return 0;
	}
}