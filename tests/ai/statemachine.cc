#include <gtest/gtest.h>
#include <cpputils/ai/statemachine.hpp>

using namespace cu::ai;

TEST(state_machine_tests, fsm_exits_when_state_returns_exit)
{
    bool called{};

    FSMActionState state1([&state1]() -> FSMEvent& { return state1.done; });
    FSMActionState state2([&state2]() -> FSMEvent& { return state2.done; });
    FSMActionState state3([&state3]() -> FSMEvent& { return state3.done; });

    FSMActionState state4([]() -> FSMEvent& { return FSMEvent::exit(); },
                          [&called](auto) { called = true; });

    FSMActionState state5([&state5]() -> FSMEvent& { return state5.done; });

    FiniteStateMachine fsm;
    fsm.setCurrentState(state1);
    state1.done.switchTo(state2);
    state2.done.switchTo(state3);
    state3.done.switchTo(state4);
    state4.done.switchTo(state5);
    state5.done.switchTo(state1);

    int actualIterationCount   = 0;
    int expectedIterationCount = 4;

    while (fsm.running())
    {
        ++actualIterationCount;
        fsm.tick();
    }

    EXPECT_EQ(expectedIterationCount, actualIterationCount);
}

TEST(state_machine_tests, onActivated_gets_called_when_becomes_current_state)
{
    bool called{};

    FSMActionState state1([&state1]() -> FSMEvent& { return state1.done; });

    FSMActionState state2([]() -> FSMEvent& { return FSMEvent::exit(); },
                          [&called](auto) { called = true; });

    FiniteStateMachine fsm;
    fsm.setCurrentState(state1);
    state1.done.switchTo(state2);

    while (fsm.running())
        fsm.tick();

    EXPECT_TRUE(called);
}

TEST(state_machine_tests, state_does_not_change_when_returns_notChanged_event)
{
    int actualIterationCount   = 0;
    int expectedIterationCount = 5;

    FSMActionState state1(
        [&actualIterationCount, &expectedIterationCount]() -> FSMEvent&
        {
            if (actualIterationCount == expectedIterationCount)
                return FSMEvent::exit();

            ++actualIterationCount;
            return FSMEvent::notChanged();
        });

    FSMActionState state2([&state1]() -> FSMEvent& { return state1.done; });

    FiniteStateMachine fsm;
    fsm.setCurrentState(state1);
    state1.done.switchTo(state2);

    while (fsm.running())
        fsm.tick();

    EXPECT_EQ(expectedIterationCount, actualIterationCount);
}

namespace cu::ai::test
{
class DummyState : public FSMState
{
public:
    FSMEvent event1;
    FSMEvent event2;

    FSMEvent& tick() override
    {
        return event2;
    }
};
}

TEST(state_machine_tests, state_changes_based_on_returned_event)
{
    bool state1NotExecuted = true;
    bool state2Executed    = false;

    test::DummyState state0;

    FSMActionState state1(
        [&state1NotExecuted]() -> FSMEvent&
        {
            state1NotExecuted = false;

            return FSMEvent::exit();
        });

    FSMActionState state2(
        [&state2Executed]() -> FSMEvent&
        {
            state2Executed = true;

            return FSMEvent::exit();
        });

    FiniteStateMachine fsm;
    fsm.setCurrentState(state0);
    state0.event1.switchTo(state1);
    state0.event2.switchTo(state2);

    while (fsm.running())
        fsm.tick();

    EXPECT_TRUE(state1NotExecuted);
    EXPECT_TRUE(state2Executed);
}