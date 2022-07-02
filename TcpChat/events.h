#pragma once

#include <list>
#include <functional>

template<typename...>
class EventHandler;

template<typename... Args>
class Event
{
	typedef EventHandler<Args...> EventHandlerType;
	std::list<EventHandlerType*> subs;
public:
	void Add(EventHandlerType& sub);
	void Remove(EventHandlerType& sub);
	void Raise(Args... args) const;
};

template<typename... Args>
class EventHandler
{
	typedef Event<Args...> EventType;
	typedef std::function<void(Args...)> CallbackType;
	EventType* event_ = nullptr;
	CallbackType callback_;
	
public:
	void BindEvent(EventType& e);
	void BindCallback(const CallbackType& callback);
	~EventHandler();
	void Call(Args... args) const;
};

template<typename... Args>
inline void Event<Args...>::Add(EventHandlerType& sub)
{
	sub.BindEvent(*this);
	subs.push_back(&sub);
}

template<typename... Args>
inline void Event<Args...>::Remove(EventHandlerType& sub)
{
	subs.remove(&sub);
}

template<typename... Args>
inline void Event<Args...>::Raise(Args... args) const
{
	// TODO: self unsub in notify cause invalid iterator error
	for (auto sub : subs)
		sub->Call(args...);
}

template<typename... Args>
inline EventHandler<Args...>::~EventHandler()
{
	if (event_)
		event_->Remove(*this);
}

template<typename... Args>
inline void EventHandler<Args...>::BindEvent(EventType& e)
{
	event_ = &e;
}

template<typename... Args>
inline void EventHandler<Args...>::BindCallback(const CallbackType& callback)
{
	callback_ = callback;
}

template<typename... Args>
inline void EventHandler<Args...>::Call(Args... args) const
{
	callback_(args...);
}
