#include <cpputils/nullable.hpp>
#include <gtest/gtest.h>

using namespace cu;

TEST(nullable_tests, default_constructor_creates_null)
{
    Nullable<int&> n;

    EXPECT_TRUE(n.isNull());
}

TEST(nullable_tests, constructing_with_value_creates_non_null)
{
    int            val;
    Nullable<int&> n{val};

    EXPECT_FALSE(n.isNull());
}

TEST(nullable_tests, moving_nullable_sets_null)
{
    Nullable<int> n1{32};
    Nullable<int> n2{std::move(n1)};

    EXPECT_TRUE(n1.isNull());
    EXPECT_FALSE(n2.isNull());
    EXPECT_EQ(n2.get(), 32);
}

TEST(nullable_tests, move_assigning_nullable_sets_rhs_null)
{
    Nullable<int> n1{32};
    Nullable<int> n2;

    EXPECT_TRUE(n2.isNull());

    n2 = std::move(n1);

    EXPECT_FALSE(n2.isNull());
    EXPECT_TRUE(n1.isNull());
    EXPECT_EQ(n2.get(), 32);
}

TEST(nullable_tests, copying_nullable_will_not_change_rhs)
{
    Nullable<int> n1{32};
    Nullable<int> n2{n1};

    EXPECT_FALSE(n1.isNull());
    EXPECT_FALSE(n2.isNull());
    EXPECT_EQ(n1.get(), 32);
    EXPECT_EQ(n2.get(), 32);
}

TEST(nullable_tests, copy_assigning_nullable_will_not_change_rhs)
{
    Nullable<int> n1{32};
    Nullable<int> n2;

    n2 = n1;

    EXPECT_FALSE(n1.isNull());
    EXPECT_FALSE(n2.isNull());
    EXPECT_EQ(n1.get(), 32);
    EXPECT_EQ(n2.get(), 32);
}

TEST(nullable_tests, can_use_assignment_operator_with_underlying_type)
{
    Nullable<int> n;
    n = 32;

    EXPECT_FALSE(n.isNull());
    EXPECT_EQ(32, n.get());
}

TEST(nullable_tests, can_compare_against_underlying_type)
{
    Nullable<int> n{32};

    EXPECT_TRUE(n == 32);
    EXPECT_TRUE(n != 31);
}

TEST(nullable_tests, can_compare_against_nullable)
{
    Nullable<int> n1{32};
    Nullable<int> n2{32};
    Nullable<int> n3{31};

    EXPECT_TRUE(n1 == n2);
    EXPECT_TRUE(n1 != n3);
}

TEST(nullable_tests, can_set_new_value)
{
    Nullable<int> n{32};
    n.set(16);

    EXPECT_EQ(n.get(), 16);
}

TEST(nullable_tests, can_set_null)
{
    Nullable<int> n{32};
    n.setNull();

    EXPECT_TRUE(n.isNull());
}

TEST(nullable_tests, calling_get_when_null_throws)
{
    Nullable<int> n;

    EXPECT_THROW({ n.get(); }, std::runtime_error);
}

TEST(nullable_tests, works_with_references)
{
    int            val{32};
    Nullable<int&> n;

    EXPECT_TRUE(n.isNull());

    n = val;

    EXPECT_FALSE(n.isNull());
    EXPECT_EQ(&val, &n.get());
    EXPECT_TRUE(n == 32);

    int anotherVal{16};
    n = anotherVal;

    EXPECT_EQ(&anotherVal, &n.get());
}