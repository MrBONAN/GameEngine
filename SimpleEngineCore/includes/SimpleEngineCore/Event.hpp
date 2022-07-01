#pragma once
#include <array>
#include <functional>

namespace SimpleEngine {
	
	enum class EventType {
		WindowResize,
		WindowClose,

		KeyPressed,
		KeyReleased,

		MouseButtonPressed,
		MouseButtonReleased,
		MouseMoved,


		EventsCount
	};

	struct BaseEvent {
		virtual ~BaseEvent() = default;
		virtual EventType get_type() const = 0;
	};

	class EventDispatcher {
	public:
		template<typename EventType>
		void add_event_listener(std::function<void(EventType&)> callback) {
			auto baseCallback = [func = std::move(callback)](BaseEvent& event) {
				func(static_cast<EventType&>(event));
			};
			m_eventCallbacks[static_cast<size_t>(EventType::type)] = std::move(baseCallback);
		}

		void dispatch(BaseEvent& event) {
			auto& callback = m_eventCallbacks[static_cast<size_t>(event.get_type())];
			if (callback) // if callback != nullptr <=> we have callback with this type of event
				callback(event);
		}

	private:
		std::array<std::function<void(BaseEvent&)>, static_cast<int>(EventType::EventsCount)> m_eventCallbacks;
	};

	struct EventWindowResize : public BaseEvent
	{
		EventWindowResize(const unsigned int new_width, const unsigned int new_height)
		:	width(new_width),
			height(new_height)
		{}

		virtual EventType get_type() const override {
			return type;
		}

		unsigned int width;
		unsigned int height;

		static const EventType type = EventType::WindowResize;
	};

	struct EventWindowClose : public BaseEvent
	{
		virtual EventType get_type() const override {
			return type;
		}

		static const EventType type = EventType::WindowClose;
	};

	struct EventMouseMoved : public BaseEvent
	{
		EventMouseMoved(const double new_x, const double new_y)
			: x(new_x),
			y(new_y)
		{}

		virtual EventType get_type() const override {
			return type;
		}

		double x;
		double y;

		static const EventType type = EventType::MouseMoved;
	};
}