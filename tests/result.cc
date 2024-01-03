#include <cpputils/result.hpp>
#include <gtest/gtest.h>

struct DummyDefaultNonConstructable
{
    DummyDefaultNonConstructable(int val)
        : value{val}
    {
    }

    static const DummyDefaultNonConstructable* defaultValue();

    int value;
};

const DummyDefaultNonConstructable*
DummyDefaultNonConstructable::defaultValue()
{
    static DummyDefaultNonConstructable val(0);

    return &val;
}

TEST(Result, success_returns_succeeded_result)
{
    auto res = cu::Result::success();
    EXPECT_TRUE(res.succeeded());
}

TEST(Result, failure_returns_failed_result)
{
    auto res = cu::Result::failure("fail");
    EXPECT_TRUE(res.failed());
}

TEST(Result, failure_returns_correct_error)
{
    auto err = "fail";
    auto res = cu::Result::failure(err);
    EXPECT_STREQ(err, res.error().c_str());
}

TEST(ErrorOr, success_returns_succeeded_result)
{
    auto res = cu::ErrorOr<int>::success(34);
    EXPECT_TRUE(res.succeeded());
}

TEST(ErrorOr, failure_returns_failed_result)
{
    auto res = cu::ErrorOr<int>::failure("fail");
    EXPECT_TRUE(res.failed());
}

TEST(ErrorOr, failure_returns_correct_error)
{
    auto err = "fail";
    auto res = cu::ErrorOr<int>::failure(err);
    EXPECT_STREQ(err, res.error().c_str());
}

TEST(ErrorOr, success_returns_correct_object)
{
    int  val = 34;
    auto res = cu::ErrorOr<int>::success(val);
    EXPECT_EQ(val, res.value());
}

TEST(ErrorOr, can_provide_default_error_to_default_non_constructable)
{
    auto res = cu::ErrorOr<const DummyDefaultNonConstructable*>::failure(
        "fail",
        DummyDefaultNonConstructable::defaultValue());

    EXPECT_EQ(DummyDefaultNonConstructable::defaultValue(), res.value());
}

TEST(ErrorOr, can_provide_ref_value)
{
    int a = 5;
    auto res = cu::ErrorOr<int&>::success(a);
    EXPECT_EQ(&a, &res.value());
}

TEST(ErrorOr, can_provide_cref_value)
{
    int a = 5;
    auto res = cu::ErrorOr<const int&>::success(a);
    EXPECT_EQ(&a, &res.value());
}

TEST(ErrorOr, can_provide_pointer)
{
    int a = 5;
    auto res = cu::ErrorOr<int*>::success(&a);
    EXPECT_EQ(&a, res.value());
}

TEST(ErrorOr, can_provide_const_pointer)
{
    int a = 5;
    auto res = cu::ErrorOr<const int*>::success(&a);
    EXPECT_EQ(&a, res.value());
}
