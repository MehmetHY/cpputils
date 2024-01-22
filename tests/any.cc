#include <cpputils/any.hpp>
#include <gtest/gtest.h>

TEST(any_tests, default_constructor_creates_invalid_data)
{
    cu::Any any;

    EXPECT_FALSE(any.isValid());
}

TEST(any_tests, create_function_creates_valid_data)
{
    int  v;
    auto any{cu::Any::create<int&>(v)};

    EXPECT_TRUE(any.isValid());
}

TEST(any_tests, isSameData_returns_correct_result)
{
    auto any{cu::Any::create<float>(2.5f)};

    EXPECT_TRUE(any.isSameType<float>());
    EXPECT_FALSE(any.isSameType<int>());
}

TEST(any_tests, reset_creates_invalid_data)
{
    auto any{cu::Any::create<float>(2.5f)};

    EXPECT_TRUE(any.isValid());

    any.reset();

    EXPECT_FALSE(any.isValid());
}

TEST(any_tests, can_get_valid_data)
{
    int  v;
    auto any{cu::Any::create<int&>(v)};

    EXPECT_EQ(&v, &any.get<int&>());
}

TEST(any_tests, getting_invalid_data_throws)
{
    cu::Any any;

    EXPECT_THROW({ any.get<int>(); }, std::runtime_error);
}

TEST(any_tests, emplace_assigns_new_value)
{
    cu::Any any;

    EXPECT_FALSE(any.isValid());

    any.emplace(32);

    EXPECT_TRUE(any.isValid());
    EXPECT_EQ(32, any.get<int>());

    int v;
    any.emplace<int&>(v);

    EXPECT_TRUE(any.isValid());
    EXPECT_EQ(&v, &any.get<int&>());
}

TEST(any_tests, works_with_non_copyable_types)
{
    auto uptr{std::make_unique<int>(32)};
    auto ptr{uptr.get()};
    auto any{cu::Any::create<std::unique_ptr<int>>(std::move(uptr))};
    uptr = std::move(any.get<std::unique_ptr<int>>());

    EXPECT_EQ(uptr.get(), ptr);
}

TEST(any_tests, can_set_new_data_with_assignment_operator)
{
    cu::Any any;
    any = 32;

    EXPECT_EQ(32, any.get<int>());

    any = 12.6f;

    EXPECT_EQ(12.6f, any.get<float>());
}

TEST(any_tests, can_implicitly_casts_to_type)
{
    auto any{cu::Any::create<std::unique_ptr<int>>(std::make_unique<int>(32))};
    std::unique_ptr<int> ptr = std::move(any);

    EXPECT_EQ(*ptr, 32);

    any.emplace(32);
    int val = any;

    EXPECT_EQ(32, any.get<int>());

    any.emplace<int&>(val);
    int& ref = any;

    EXPECT_EQ(&val, &any.get<int&>());
}

TEST(any_tests, can_explicitly_cast_to_type)
{
    auto any{cu::Any::create<std::unique_ptr<int>>(std::make_unique<int>(32))};

    std::unique_ptr<int> ptr =
        static_cast<std::unique_ptr<int>>(std::move(any));

    EXPECT_EQ(*ptr, 32);

    any.emplace(32);
    int val = static_cast<int>(any);

    EXPECT_EQ(32, any.get<int>());

    any.emplace<int&>(val);
    int& ref = static_cast<int&>(any);

    EXPECT_EQ(&val, &any.get<int&>());
}

TEST(any_test, moving_any_will_reset)
{
    auto any1{cu::Any::create<std::unique_ptr<int>>(std::make_unique<int>(32))};

    std::unique_ptr<int> ptr =
        static_cast<std::unique_ptr<int>>(std::move(any1));

    EXPECT_EQ(*ptr, 32);
    EXPECT_FALSE(any1.isValid());

    auto any2{cu::Any::create(32)};
    auto any3{std::move(any2)};
    auto any4 = std::move(any3);

    EXPECT_FALSE(any2.isValid());
    EXPECT_FALSE(any3.isValid());
    EXPECT_TRUE(any4.isValid());
    EXPECT_EQ(32, any4.get<int>());
}
