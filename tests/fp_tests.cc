#include <gtest/gtest.h>

#include "probfd/value_type.h"

using namespace probfd;

TEST(FPTests, test_approx_less)
{
    ASSERT_TRUE(probfd::is_approx_less(0.0_vt, 0.6_vt, 0.5_vt));
}

TEST(FPTests, test_approx_equal)
{
    ASSERT_TRUE(probfd::is_approx_equal(0.0_vt, 0.0_vt, 0.5_vt));
}

TEST(FPTests, test_approx_greater)
{
    ASSERT_TRUE(probfd::is_approx_greater(0.6_vt, 0.0_vt, 0.5_vt));
}

TEST(FPTests, test_fraction_to_value)
{
    ASSERT_EQ(probfd::fraction_to_value(853, 1003526), 853_vt / 1003526_vt);
}

TEST(FPTests, test_string_to_value)
{
    ASSERT_EQ(probfd::string_to_value("45/923"), 45_vt / 923_vt);
}

TEST(FPTests, test_abs)
{
    ASSERT_EQ(probfd::abs(-42_vt), 42_vt);
}

TEST(FPTests, test_user_defined_literal_double)
{
    ASSERT_EQ(probfd::double_to_value(42.9), 42.9_vt);
}

TEST(FPTests, test_user_defined_literal_long)
{
    ASSERT_EQ(probfd::double_to_value(static_cast<double>(42)), 42_vt);
}