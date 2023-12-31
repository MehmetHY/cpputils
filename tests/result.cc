#include <cpputils/result.hpp>
#include <gtest/gtest.h>

TEST(Result, success_returns_succeeded_result)
{
    auto res = cu::Result::success();
    EXPECT_TRUE(res.succeeded());
}

TEST(Result, failure_returns_failed_result)
{
    auto res = cu::Result::failure("failed");
    EXPECT_FALSE(res.succeeded());
}

TEST(Result, failure_returns_correct_error)
{
    auto err = "fail";
    auto res = cu::Result::failure(err);
    EXPECT_STREQ(err, res.error().c_str());
}

TEST(ErrorOr, success_returns_succeeded_result)
{
    auto res = cu::ErrorOr<int>::success(std::make_unique<int>(32));
    EXPECT_TRUE(res.succeeded());
}

TEST(ErrorOr, failure_returns_failed_result)
{
    auto res = cu::ErrorOr<int>::failure("fail");
    EXPECT_FALSE(res.succeeded());
}

TEST(ErrorOr, failure_returns_correct_error)
{
    auto err = "fail";
    auto res = cu::ErrorOr<int>::failure(err);
    EXPECT_STREQ(err, res.error().c_str());
}

TEST(ErrorOr, success_returns_correct_object)
{
    auto obj = new int(32);
    auto ptr = std::unique_ptr<int>(obj);
    auto res = cu::ErrorOr<int>::success(std::move(ptr));
    ptr = res.object();
    EXPECT_EQ(obj, ptr.get());
}