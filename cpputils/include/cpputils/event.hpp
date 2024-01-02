#pragma once

#include "export.hpp"
#include <vector>
#include <algorithm>
#include <functional>
#include "concepts.hpp"

namespace cu
{
template<NotRRef... TArgs>
class EventListener;

template<NotRRef... TArgs>
class CU_EXPORT Event
{
public: // Types ______________________________________________________________
    friend EventListener<TArgs...>;

public: // Constructors & Destructor __________________________________________
    Event() noexcept = default;
    virtual ~Event() noexcept;

public: // Move Semantics _____________________________________________________
    Event(Event&&) noexcept;
    virtual Event& operator=(Event&&) noexcept;

public: // Copy Semantics _____________________________________________________
    Event(const Event&);
    virtual Event& operator=(const Event&);

public: // Operators __________________________________________________________
    void operator+=(EventListener<TArgs...>& listener);
    void operator-=(EventListener<TArgs...>& listener);
    void operator()(TArgs... args);

public: // Static _____________________________________________________________

public: // Events _____________________________________________________________

public: // Methods ____________________________________________________________
private:
    void addListener(EventListener<TArgs...>* listener)
    {
        if (std::find(_listeners.cbegin(), _listeners.cend(), listener) ==
            _listeners.cend())
            _listeners.push_back(listener);
    }

    void removeListener(EventListener<TArgs...>* listener)
    {
        auto it = std::find(_listeners.begin(), _listeners.end(), listener);

        if (it != _listeners.end())
            _listeners.erase(it);
    }

private: // Fields ____________________________________________________________
    std::vector<EventListener<TArgs...>*> _listeners{};
};

template<NotRRef... TArgs>
class CU_EXPORT EventListener
{
public: // Types ______________________________________________________________
    friend Event<TArgs...>;

public: // Constructors & Destructor __________________________________________
    EventListener(std::function<void(TArgs...)> callback) noexcept
        : _callback{std::move(callback)}
    {
    }

    virtual ~EventListener() noexcept;

public: // Move Semantics _____________________________________________________
    EventListener(EventListener<TArgs...>&&) noexcept;
    virtual EventListener<TArgs...>& operator=(
        EventListener<TArgs...>&&) noexcept;

public: // Copy Semantics _____________________________________________________
    EventListener(const EventListener<TArgs...>&);
    virtual EventListener<TArgs...>& operator=(const EventListener<TArgs...>&);

public: // Operators __________________________________________________________
    void operator()(TArgs... args)
    {
        _callback(std::forward<TArgs>(args)...);
    }

public: // Events _____________________________________________________________

public: // Methods ____________________________________________________________
    void setCallback(std::function<void(TArgs...)> callback)
    {
        _callback = std::move(callback);
    }

private:
    void addEvent(Event<TArgs...>* event)
    {
        if (std::find(_events.cbegin(), _events.cend(), event) ==
            _events.cend())
            _events.push_back(event);
    }

    void removeEvent(Event<TArgs...>* event)
    {
        auto it = std::find(_events.begin(), _events.end(), event);

        if (it != _events.end())
            _events.erase(it);
    }

private: // Fields ____________________________________________________________
    std::function<void(TArgs...)> _callback;
    std::vector<Event<TArgs...>*> _events{};
};

template<NotRRef... TArgs>
Event<TArgs...>::~Event() noexcept
{
    for (EventListener<TArgs...>* listener : _listeners)
        listener->removeEvent(this);
}

template<NotRRef... TArgs>
Event<TArgs...>::Event(Event<TArgs...>&& other) noexcept
{
    for (auto it = other._listeners.begin(); it != other._listeners.end();
         it      = other._listeners.begin())
    {
        auto& listener = *(*it);
        (*this) += listener;
        other -= listener;
    }
}

template<NotRRef... TArgs>
Event<TArgs...>&
Event<TArgs...>::operator=(Event<TArgs...>&& other) noexcept
{
    for (auto it = other._listeners.begin(); it != other._listeners.end();
         it      = other._listeners.begin())
    {
        auto& listener = *(*it);
        (*this) += listener;
        other -= listener;
    }

    return *this;
}

template<NotRRef... TArgs>
Event<TArgs...>::Event(const Event<TArgs...>& other)
{
    for (auto listener : other._listeners)
        (*this) += *listener;
}

template<NotRRef... TArgs>
Event<TArgs...>&
Event<TArgs...>::operator=(const Event<TArgs...>& other)
{
    for (auto listener : other._listeners)
        (*this) += *listener;

    return *this;
}

template<NotRRef... TArgs>
void
Event<TArgs...>::operator+=(EventListener<TArgs...>& listener)
{
    addListener(&listener);
    listener.addEvent(this);
}

template<NotRRef... TArgs>
void
Event<TArgs...>::operator-=(EventListener<TArgs...>& listener)
{
    removeListener(&listener);
    listener.removeEvent(this);
}

template<NotRRef... TArgs>
void
Event<TArgs...>::operator()(TArgs... args)
{
    for (EventListener<TArgs...>* listener : _listeners)
        (*listener)(args...);
}

template<NotRRef... TArgs>
EventListener<TArgs...>::~EventListener() noexcept
{
    for (Event<TArgs...>* event : _events)
        event->removeListener(this);
}

template<NotRRef... TArgs>
EventListener<TArgs...>::EventListener(EventListener<TArgs...>&& other) noexcept
{
    _callback = std::move(other._callback);

    for (auto it = other._events.begin(); it != other._events.end();
         it      = other._events.begin())
    {
        auto& event = *(*it);
        event += *this;
        event -= other;
    }
}

template<NotRRef... TArgs>
EventListener<TArgs...>&
EventListener<TArgs...>::operator=(EventListener<TArgs...>&& other) noexcept
{
    _callback = std::move(other._callback);

    for (auto it = other._events.begin(); it != other._events.end();
         it      = other._events.begin())
    {
        auto& event = *(*it);
        event += *this;
        event -= other;
    }

    return *this;
}

template<NotRRef... TArgs>
EventListener<TArgs...>::EventListener(const EventListener<TArgs...>& other)
{
    _callback = other._callback;

    for (auto event : other._events)
        *event += *this;
}

template<NotRRef... TArgs>
EventListener<TArgs...>&
EventListener<TArgs...>::operator=(const EventListener<TArgs...>& other)
{
    _callback = other._callback;

    for (auto event : other._events)
        *event += *this;

    return *this;
}

}
