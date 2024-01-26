#pragma once

#include <list>
#include <functional>

namespace cu
{

template<typename... TArgs>
class EventHandler;

template<typename... TArgs>
class EventListener
{
#pragma region ____________________________ Types ______________________________

public:
    friend EventHandler<TArgs...>;

#pragma endregion

#pragma region _________________________ Constructors __________________________

public:
    EventListener(std::function<void(TArgs...)> callback);
    ~EventListener();

#pragma endregion

#pragma region ________________________ Move Semantics _________________________

public:
    EventListener(EventListener<TArgs...>&& other);
    EventListener<TArgs...>& operator=(EventListener<TArgs...>&& other);

#pragma endregion

#pragma region ________________________ Copy Semantics _________________________

public:
    EventListener(const EventListener<TArgs...>& other);
    EventListener<TArgs...>& operator=(const EventListener<TArgs...>& other);

#pragma endregion

#pragma region ____________________________ Fields _____________________________

private:
    std::function<void(TArgs...)>      _callback;
    std::list<EventHandler<TArgs...>*> _handlers{};

#pragma endregion
};

template<typename... TArgs>
class EventHandler
{
#pragma region ____________________________ Types ______________________________

    friend EventListener<TArgs...>;

#pragma endregion

#pragma region _________________________ Constructors __________________________

public:
    EventHandler() = default;
    ~EventHandler();

#pragma endregion

#pragma region ________________________ Move Semantics _________________________

public:
    EventHandler(EventHandler<TArgs...>&& other);
    EventHandler<TArgs...>& operator=(EventHandler<TArgs...>&& other);

#pragma endregion

#pragma region ________________________ Copy Semantics _________________________

public:
    EventHandler(const EventHandler<TArgs...>& other);
    EventHandler<TArgs...>& operator=(const EventHandler<TArgs...>& other);

#pragma endregion

#pragma region ___________________________ Operators ___________________________

public:
    void operator+=(EventListener<TArgs...>& listener);
    void operator-=(EventListener<TArgs...>& listener);
    void operator()(TArgs... args);

#pragma endregion

#pragma region ____________________________ Fields _____________________________

private:
    bool                                _invoking{};
    std::list<EventListener<TArgs...>*> _listeners{};
    std::list<EventListener<TArgs...>*> _listenersToAdd{};
    std::list<EventListener<TArgs...>*> _listenersToRemove{};

#pragma endregion
};

#pragma region EventListenerImpl

template<typename... TArgs>
EventListener<TArgs...>::EventListener(std::function<void(TArgs...)> callback)
    : _callback{std::move(callback)}
{
}

template<typename... TArgs>
EventListener<TArgs...>::~EventListener()
{
    for (auto handler : _handlers)
        handler->_listeners.remove(this);
}

template<typename... TArgs>
EventListener<TArgs...>::EventListener(EventListener<TArgs...>&& other)
{
    _callback = std::move(other._callback);
    _handlers = std::move(other._handlers);

    for (auto handler : _handlers)
    {
        handler->_listeners.remove(&other);
        handler->_listeners.push_back(this);
    }
}

template<typename... TArgs>
EventListener<TArgs...>&
EventListener<TArgs...>::operator=(EventListener<TArgs...>&& other)
{
    _callback = std::move(other._callback);
    _handlers = std::move(other._handlers);

    for (auto handler : _handlers)
    {
        handler->_listeners.remove(&other);
        handler->_listeners.push_back(this);
    }

    return *this;
}

template<typename... TArgs>
EventListener<TArgs...>::EventListener(const EventListener<TArgs...>& other)
{
    _callback = other._callback;
    _handlers = other._handlers;

    for (auto handler : _handlers)
        handler->_listeners.push_back(this);
}

template<typename... TArgs>
EventListener<TArgs...>&
EventListener<TArgs...>::operator=(const EventListener<TArgs...>& other)
{
    _callback = other._callback;
    _handlers = other._handlers;

    for (auto handler : _handlers)
        handler->_listeners.push_back(this);

    return *this;
}

#pragma endregion

#pragma region EventHandlerImpl

template<typename... TArgs>
EventHandler<TArgs...>::~EventHandler()
{
    for (auto listener : _listeners)
        listener->_handlers.remove(this);
}

template<typename... TArgs>
EventHandler<TArgs...>::EventHandler(EventHandler<TArgs...>&& other)
{
    _listeners = std::move(other._listeners);

    for (auto listener : _listeners)
    {
        listener->_handlers.remove(&other);
        listener->_handlers.push_back(this);
    }
}

template<typename... TArgs>
EventHandler<TArgs...>&
EventHandler<TArgs...>::operator=(EventHandler<TArgs...>&& other)
{
    _listeners = std::move(other._listeners);

    for (auto listener : _listeners)
    {
        listener->_handlers.remove(&other);
        listener->_handlers.push_back(this);
    }

    return *this;
}

template<typename... TArgs>
EventHandler<TArgs...>::EventHandler(const EventHandler<TArgs...>& other)
{
    _listeners = other._listeners;

    for (auto listener : _listeners)
        listener->_handlers.push_back(this);
}

template<typename... TArgs>
EventHandler<TArgs...>&
EventHandler<TArgs...>::operator=(const EventHandler<TArgs...>& other)
{
    _listeners = other._listeners;

    for (auto listener : _listeners)
        listener->_handlers.push_back(this);

    return *this;
}

template<typename... TArgs>
void
EventHandler<TArgs...>::operator+=(EventListener<TArgs...>& listener)
{
    if (_invoking)
        _listenersToAdd.push_back(&listener);
    else
    {
        _listeners.push_back(&listener);
        listener._handlers.push_back(this);
    }
}

template<typename... TArgs>
void
EventHandler<TArgs...>::operator-=(EventListener<TArgs...>& listener)
{
    if (_invoking)
        _listenersToRemove.push_back(&listener);
    else
    {
        _listeners.remove(&listener);
        listener._handlers.remove(this);
    }
}

template<typename... TArgs>
void
EventHandler<TArgs...>::operator()(TArgs... args)
{
    _invoking = true;

    for (auto listener : _listeners)
        listener->_callback(args...);

    for (auto listener : _listenersToAdd)
    {
        _listeners.push_back(listener);
        listener->_handlers.push_back(this);
    }

    _listenersToAdd.clear();

    for (auto listener : _listenersToRemove)
    {
        _listeners.remove(listener);
        listener->_handlers.remove(this);
    }

    _listenersToRemove.clear();

    _invoking = false;
}

#pragma endregion

}
