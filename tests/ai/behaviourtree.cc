#include <gtest/gtest.h>
#include <cpputils/ai/behaviourtree.hpp>

using namespace cu::ai;

static auto successAction = [] { return BTStatus::success; };
static auto failureAction = [] { return BTStatus::failure; };
static auto runningAction = [] { return BTStatus::running; };

TEST(behaviour_tree_tests, sequence_returns_success_when_all_succeeded)
{
    BehaviourTree tree;
    auto          seq = tree.createRoot<BTSequence>();
    seq->createChild<BTAction>(successAction);
    seq->createChild<BTAction>(successAction);
    seq->createChild<BTAction>(successAction);
    seq->createChild<BTAction>(successAction);

    auto actualStatus           = BTStatus::running;
    auto expectedStatus         = BTStatus::success;
    int  actualIterationCount   = 0;
    int  expectedIterationCount = 4;

    while (BTStatus::running == actualStatus)
    {
        actualStatus = tree.tick();
        ++actualIterationCount;
    }

    EXPECT_EQ(expectedStatus, actualStatus);
    EXPECT_EQ(expectedIterationCount, actualIterationCount);
}

TEST(behaviour_tree_tests,
     sequence_returns_failure_immediately_at_first_failed_leaf)
{
    BehaviourTree tree;
    auto          seq = tree.createRoot<BTSequence>();
    seq->createChild<BTAction>(successAction);
    seq->createChild<BTAction>(successAction);
    seq->createChild<BTAction>(failureAction);
    seq->createChild<BTAction>(successAction);

    auto actualStatus           = BTStatus::running;
    auto expectedStatus         = BTStatus::failure;
    int  actualIterationCount   = 0;
    int  expectedIterationCount = 3;

    while (BTStatus::running == actualStatus)
    {
        actualStatus = tree.tick();
        ++actualIterationCount;
    }

    EXPECT_EQ(expectedStatus, actualStatus);
    EXPECT_EQ(expectedIterationCount, actualIterationCount);
}

TEST(behaviour_tree_tests,
     fallback_returns_success_immediately_at_first_succeeded_leaf)
{
    BehaviourTree tree;
    auto          fallback = tree.createRoot<BTFallback>();
    fallback->createChild<BTAction>(failureAction);
    fallback->createChild<BTAction>(failureAction);
    fallback->createChild<BTAction>(successAction);
    fallback->createChild<BTAction>(failureAction);

    auto actualStatus           = BTStatus::running;
    auto expectedStatus         = BTStatus::success;
    int  actualIterationCount   = 0;
    int  expectedIterationCount = 3;

    while (BTStatus::running == actualStatus)
    {
        actualStatus = tree.tick();
        ++actualIterationCount;
    }

    EXPECT_EQ(expectedStatus, actualStatus);
    EXPECT_EQ(expectedIterationCount, actualIterationCount);
}

TEST(behaviour_tree_tests, fallback_returns_failure_when_all_failed)
{
    BehaviourTree tree;
    auto          fallback = tree.createRoot<BTFallback>();
    fallback->createChild<BTAction>(failureAction);
    fallback->createChild<BTAction>(failureAction);
    fallback->createChild<BTAction>(failureAction);
    fallback->createChild<BTAction>(failureAction);

    auto actualStatus           = BTStatus::running;
    auto expectedStatus         = BTStatus::failure;
    int  actualIterationCount   = 0;
    int  expectedIterationCount = 4;

    while (BTStatus::running == actualStatus)
    {
        actualStatus = tree.tick();
        ++actualIterationCount;
    }

    EXPECT_EQ(expectedStatus, actualStatus);
    EXPECT_EQ(expectedIterationCount, actualIterationCount);
}

TEST(behaviour_tree_tests, can_chain_different_node_types)
{
    BehaviourTree tree;
    auto          root    = tree.createRoot<BTSequence>();
    auto          branch1 = root->createChild<BTFallback>();
    auto          branch2 = root->createChild<BTSequence>();

    auto b1Fallback = branch1->createChild<BTFallback>();
    auto b1Seq      = branch1->createChild<BTSequence>();

    auto b2Fallback = branch2->createChild<BTFallback>();
    auto b2Seq      = branch2->createChild<BTSequence>();

    b1Fallback->createChild<BTAction>(failureAction);
    b1Fallback->createChild<BTAction>(successAction);
    b1Fallback->createChild<BTAction>(failureAction);
    b1Seq->createChild<BTAction>(successAction);
    b1Seq->createChild<BTAction>(failureAction);

    b2Fallback->createChild<BTAction>(failureAction);
    b2Fallback->createChild<BTAction>(failureAction);
    b2Fallback->createChild<BTAction>(successAction);
    b2Seq->createChild<BTAction>(successAction);

    auto expectedStatus = BTStatus::success;
    auto actualStatus   = BTStatus::running;
    auto expectedIterationCount = 6;
    auto actualIterationCount   = 0;

    while (BTStatus::running==actualStatus)
    {
        actualStatus = tree.tick();
        ++actualIterationCount;
    }

    EXPECT_EQ(expectedStatus, actualStatus);
    EXPECT_EQ(expectedIterationCount, actualIterationCount);
}