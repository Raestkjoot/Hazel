#pragma once

#include "Hazel/Core.h"

namespace Hazel {

	// Events in Hazel are currently blocking, meaning when an event occurs it
	// immediately gets dispatched and must be dealt with right then an there.
	// For the future, a better strategy might be to buffer events in an event
	// bus and process them during the "event" part of the update stage.

	enum class EventType
	{
		None = 0,
		WindowClose, WindowResize, WindowFocus, WindowLostFocus, WindowMoved,
		AppTick, AppUpdate, AppRender,
		KeyPressed, KeyReleased,
		MouseButtonPressed, MouseButtonReleased, MouseMoved, MouseScrolled
	};

	// Used for filtering. To log all mouse events (all mouse event types)
	enum EventCategory
	{
		None = 0,
		EventCategoryApplication	= BIT(0),
		EventCategoryInput			= BIT(1),
		EventCategoryKeyboard		= BIT(2),
		EventCategoryMouse			= BIT(3),
		EventCategoryMouseButton	= BIT(4)
	};

	// Macros for simplicity's sake when implementing an event.
#define EVENT_CLASS_TYPE(type) static EventType GetStaticType() { return EventType::##type; }\
								virtual EventType GetEventType() const override { return GetStaticType(); }\
								virtual const char* GetName() const override { return #type; }

#define EVENT_CLASS_CATEGORY(category) virtual int GetCategoryFlags() const override { return category; }

	// A base class for events
	class HAZEL_API Event
	{
		friend class EventDispatcher;
	public:
		// Virtual functions to be overriden. 
		virtual EventType GetEventType() const = 0;
		virtual const char* GetName() const = 0;
		virtual int GetCategoryFlags() const = 0;
		virtual std::string ToString() const { return GetName(); }

		// utility function: is this event in this category? 0=false, else=true
		inline bool IsInCategory(EventCategory category)
		{
			return GetCategoryFlags() & category;
		}
	protected:
		bool m_Handled = false; // Has the event been handled or not?
	};

	// dispatch events based on type
	class EventDispatcher
	{
		template<typename T>
		using EventFn = std::function<bool(T&)>;

	public:
		EventDispatcher(Event& event) // event ref, could be any event
			: m_Event(event) {}

		template<typename T>
		bool Dispatch(EventFn<T> func)
		{
			// if the event we try to dispatch matches function type, call function with that event
			if (m_Event.GetEventType() == T::GetStaticType())
			{
				m_Event.m_Handled = func(*(T*)&m_Event);
				return true;
			}
			return false;
		}

	private:
		Event& m_Event;
	};

	// for logging events
	inline std::ostream& operator<<(std::ostream& os, const Event& e)
	{
		return os << e.ToString();
	}
}