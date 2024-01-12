#pragma once

#include <vector>
#include <type_traits>
#include <concepts>
#include <memory>
#include <functional>
#include <mutex>

namespace cu::ai
{

class FSMEvent;

class FSMState
{
public:
    virtual ~FSMState() noexcept = default;

    virtual void onActivated(const FSMEvent& activatorEvent) {}

    virtual FSMEvent& tick() = 0;
};

class FiniteStateMachine;

class FSMEvent
{
    friend FiniteStateMachine;

    FSMState* _linkedState{};

public:
    void switchTo(FSMState& state) noexcept
    {
        _linkedState = &state;
    }

    void clearTransition() noexcept
    {
        _linkedState = nullptr;
    }

    static FSMEvent& notChanged() noexcept;
    static FSMEvent& exit() noexcept;
};

FSMEvent&
FSMEvent::notChanged() noexcept
{
    static FSMEvent event;

    return event;
}

FSMEvent&
FSMEvent::exit() noexcept
{
    static FSMEvent event;

    return event;
}

class FiniteStateMachine
{
    FSMState* _currentState{};
    bool      _running{};

public:

    void setCurrentState(FSMState& state) noexcept
    {
        if (nullptr == _currentState)
            _running = true;

        _currentState = &state;
    }

    bool running() const noexcept
    {
        return _running;
    }

    void exit() noexcept
    {
        _running = false;
    }

    void tick()
    {
        if (!_running)
            return;

        auto& event = _currentState->tick();

        if (&event == &FSMEvent::notChanged())
            return;

        if (&event == &FSMEvent::exit())
        {
            _running = false;

            return;
        }

        _currentState = event._linkedState;

        if (nullptr == _currentState)
        {
            _running = false;

            return;
        }

        _currentState->onActivated(event);
    }
};

class FSMActionState : public FSMState
{
    std::function<FSMEvent&(void)>       _tickFunc;
    std::function<void(const FSMEvent&)> _onActivatedCallback;

public:
    FSMEvent done;

    FSMActionState(std::function<FSMEvent&(void)> tickFunc) noexcept
        : _tickFunc{std::move(tickFunc)}
        , _onActivatedCallback{[](auto) {}}
    {
    }

    FSMActionState(
        std::function<FSMEvent&(void)>       tickFunc,
        std::function<void(const FSMEvent&)> onActivatedCallback) noexcept
        : _tickFunc{std::move(tickFunc)}
        , _onActivatedCallback{std::move(onActivatedCallback)}
    {
    }

    FSMEvent& tick() override
    {
        return _tickFunc();
    }

    void onActivated(const FSMEvent& activatorEvent)
    {
        _onActivatedCallback(activatorEvent);
    }
};

}