#include <gtest/gtest.h>
#include <cpputils/event.hpp>

namespace cu::event::tests
{
struct DummyEventOwner
{
    cu::EventHandler<int> event1;
    cu::EventHandler<>    event2;
};

struct DummyEventListenerOwner
{
    cu::EventListener<int> listener1;
    cu::EventListener<>    listener2;
    int                    value1{};
    int                    value2{};

    DummyEventListenerOwner()
        : listener1{[this](int val) { this->handle1(val); }}
        , listener2{[this]() { this->handle2(); }}
    {
    }

    void handle1(int val)
    {
        value1 = val;
    }

    void handle2()
    {
        ++value2;
    }
};

}

TEST(event_tests, can_subscribe_to_event)
{
    cu::event::tests::DummyEventOwner         eventOwner;
    cu::event::tests::DummyEventListenerOwner listener1;
    cu::event::tests::DummyEventListenerOwner listener2;

    eventOwner.event1 += listener1.listener1;
    eventOwner.event1 += listener2.listener1;

    int val = 43;
    eventOwner.event1(val);

    EXPECT_EQ(val, listener1.value1);
    EXPECT_EQ(val, listener2.value1);
}

TEST(event_tests, can_unsubscribe_from_event)
{
    cu::event::tests::DummyEventOwner         eventOwner;
    cu::event::tests::DummyEventListenerOwner listener1;
    cu::event::tests::DummyEventListenerOwner listener2;

    eventOwner.event1 += listener1.listener1;
    eventOwner.event1 += listener2.listener1;

    int val1 = 43;
    eventOwner.event1(val1);

    eventOwner.event1 -= listener1.listener1;
    eventOwner.event1 -= listener2.listener1;

    int val2 = 10;
    eventOwner.event1(val2);

    EXPECT_EQ(val1, listener1.value1);
    EXPECT_EQ(val1, listener2.value1);
}

TEST(event_tests, safely_unsubscribe_nonattached_listener)
{
    cu::event::tests::DummyEventOwner         eventOwner;
    cu::event::tests::DummyEventListenerOwner listener;

    eventOwner.event2 -= listener.listener2;
    eventOwner.event2 -= listener.listener2;
    eventOwner.event2 -= listener.listener2;
    eventOwner.event2 -= listener.listener2;
    eventOwner.event2 -= listener.listener2;

    eventOwner.event2();

    EXPECT_EQ(0, listener.value2);
}

TEST(event_tests, moving_listener_moves_events)
{
    int              val = 0;
    cu::EventHandler event;

    cu::EventListener<> listener1([&val]() { ++val; });

    event += listener1;

    cu::EventListener<> listener2 = std::move(listener1);

    event();
    EXPECT_EQ(1, val);

    event -= listener1;
    event -= listener1;
    event -= listener1;
    event -= listener1;

    event();
    EXPECT_EQ(2, val);

    event -= listener2;

    event();
    EXPECT_EQ(2, val);
}

TEST(event_tests, moving_event_moves_event_listeners)
{
    int                val{};
    cu::EventHandler<> event1;

    cu::EventListener<> listener1([&val]() { ++val; });

    cu::EventListener<> listener2([&val]() { ++val; });

    cu::EventListener<> listener3([&val]() { ++val; });

    event1 += listener1;
    event1 += listener2;
    event1 += listener3;

    cu::EventHandler<> event2 = std::move(event1);

    event1();
    EXPECT_EQ(0, val);

    event2();
    EXPECT_EQ(3, val);
}

TEST(event_tests, copying_event_listener_copies_events)
{
    int                val{};
    cu::EventHandler<> event1;
    cu::EventHandler<> event2;
    cu::EventHandler<> event3;

    cu::EventListener<> listener1([&val]() { ++val; });

    event1 += listener1;
    event2 += listener1;
    event3 += listener1;

    cu::EventListener<> listener2 = listener1;

    event1();
    event2();
    event3();

    EXPECT_EQ(6, val);
}

TEST(event_tests, copying_event_copies_event_listeners)
{
    int              val{};
    cu::EventHandler event1;

    cu::EventListener<> listener1([&val]() { ++val; });

    cu::EventListener<> listener2([&val]() { ++val; });

    cu::EventListener<> listener3([&val]() { ++val; });

    event1 += listener1;
    event1 += listener2;
    event1 += listener3;

    cu::EventHandler<> event2 = event1;

    event1();
    event2();

    EXPECT_EQ(6, val);
}

TEST(event_tests, unsubscribes_before_die)
{
    int                val{};
    cu::EventHandler<> event;

    {
        cu::EventListener<> listener([&val]() { ++val; });

        event += listener;
    }

    event();
    EXPECT_EQ(0, val);
}

TEST(event_tests, event_is_safe_to_die_when_has_listeners)
{
    cu::EventListener<> listener1([]() {});
    cu::EventListener<> listener2([]() {});

    {
        cu::EventHandler<> event;
        event += listener1;
        event += listener2;
    }

    cu::EventListener<> listener3 = std::move(listener1);
    cu::EventListener<> listener4 = listener3;

    SUCCEED();
}

TEST(event_tests, can_pass_by_lref)
{
    int                    val{};
    cu::EventHandler<int&> event;

    cu::EventListener<int&> listener([](int& v) { ++v; });

    event += listener;

    event(val);

    EXPECT_EQ(1, val);
}

TEST(event_tests, can_pass_by_cref)
{
    int                          val{};
    cu::EventHandler<const int&> event;

    cu::EventListener<const int&> listener([&val](const int& v) { val += v; });

    event += listener;

    event(7);

    EXPECT_EQ(7, val);
}

TEST(event_tests, can_pass_pointer)
{
    int                    val{};
    cu::EventHandler<int*> event;

    cu::EventListener<int*> listener([](int* v) { ++(*v); });

    event += listener;
    event(&val);

    EXPECT_EQ(1, val);
}

TEST(event_tests, can_pass_cpointer)
{
    int                          val{};
    cu::EventHandler<const int*> event;

    cu::EventListener<const int*> listener([&val](const int* v) { val += *v; });

    event += listener;

    int val2 = 12;
    event(&val2);

    EXPECT_EQ(val2, val);
}

TEST(event_tests, subscribing_event_during_execution_will_not_affect_handler)
{
    int                  val{};
    cu::EventHandler<>   event;
    cu::EventListener<>* l2Ptr;

    cu::EventListener<> l1{[&event, &l2Ptr, &val]
                           {
                               ++val;
                               event += *l2Ptr;
                           }};

    cu::EventListener<> l2{[&val] { ++val; }};
    l2Ptr = &l2;

    event += l1;
    event();

    EXPECT_EQ(1, val);
}

TEST(event_tests, unsubscribing_event_during_execution_will_not_affect_handler)
{
    int                  val{};
    cu::EventHandler<>   event;
    cu::EventListener<>* l2Ptr;

    cu::EventListener<> l1{[&event, &l2Ptr, &val]
                           {
                               ++val;
                               event -= *l2Ptr;
                           }};

    cu::EventListener<> l2{[&val] { ++val; }};
    l2Ptr = &l2;

    event += l1;
    event += l2;
    event();

    EXPECT_EQ(2, val);
}
