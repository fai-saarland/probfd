#include <gtest/gtest.h>

#include "probfd/interval.h"
#include "probfd/value_type.h"

#include "probfd/engines/utils.h"

using namespace probfd;

TEST(IntervalTests, test_as_lower_bound)
{
    Interval interval(0.0_vt, 1.0_vt);
    ASSERT_EQ(interval.lower, 0.0_vt);
}

TEST(IntervalTests, test_as_upper_bound)
{
    Interval interval(0.0_vt, 1.0_vt);
    ASSERT_EQ(interval.upper, 1.0_vt);
}

TEST(IntervalTests, test_approx_less)
{
    ASSERT_TRUE(probfd::is_approx_less(0.0_vt, 0.6_vt, 0.5_vt));
}

TEST(IntervalTests, test_approx_equal)
{
    ASSERT_TRUE(probfd::is_approx_equal(0.0_vt, 0.0_vt, 0.5_vt));
}

TEST(IntervalTests, test_approx_greater)
{
    ASSERT_TRUE(probfd::is_approx_greater(0.6_vt, 0.0_vt, 0.5_vt));
}

TEST(IntervalTests, test_interval_default_ctor)
{
    Interval interval;
    ASSERT_EQ(interval.lower, 0.0_vt);
    ASSERT_EQ(interval.upper, 0.0_vt);
}

TEST(IntervalTests, test_interval_single_ctor)
{
    Interval interval(5.0_vt);
    ASSERT_EQ(interval.lower, 5.0_vt);
    ASSERT_EQ(interval.upper, 5.0_vt);
}

TEST(IntervalTests, test_interval_two_bounds_ctor)
{
    Interval interval(42.0_vt, 45.0_vt);
    ASSERT_EQ(interval.lower, 42.0_vt);
    ASSERT_EQ(interval.upper, 45.0_vt);
}

TEST(IntervalTests, test_interval_plus_fin)
{
    Interval interval(8.0_vt, 40.0_vt);
    Interval interval2(5.0_vt, 75.0_vt);

    auto interval3 = interval + interval2;

    ASSERT_EQ(interval3.lower, 8.0_vt + 5.0_vt);
    ASSERT_EQ(interval3.upper, 40.0_vt + 75.0_vt);
}

TEST(IntervalTests, test_interval_plus_inf)
{
    Interval interval(8.0_vt, INFINITE_VALUE);
    Interval interval2(INFINITE_VALUE, 75.0_vt);

    auto interval3 = interval + interval2;

    ASSERT_EQ(interval3.lower, INFINITE_VALUE);
    ASSERT_EQ(interval3.upper, INFINITE_VALUE);
}

TEST(IntervalTests, test_interval_times_fin)
{
    Interval interval(8.0_vt, 40.0_vt);

    auto interval2 = 20.0_vt * interval;

    ASSERT_EQ(interval2.lower, 20.0_vt * 8.0_vt);
    ASSERT_EQ(interval2.upper, 20.0_vt * 40.0_vt);
}

TEST(IntervalTests, test_interval_times2)
{
    Interval interval(-6.8, 4.3);

    auto interval2 = INFINITE_VALUE * interval;

    ASSERT_EQ(interval2.lower, -INFINITE_VALUE);
    ASSERT_EQ(interval2.upper, INFINITE_VALUE);
}

TEST(IntervalTests, test_interval_times3)
{
    Interval interval(-INFINITE_VALUE, 5.0);

    auto interval2 = -INFINITE_VALUE * interval;

    ASSERT_EQ(interval2.lower, INFINITE_VALUE);
    ASSERT_EQ(interval2.upper, -INFINITE_VALUE);
}

TEST(IntervalTests, test_interval_times4)
{
    Interval interval(8.0_vt, -INFINITE_VALUE);

    auto interval2 = -20.0_vt * interval;

    ASSERT_EQ(interval2.lower, -20.0_vt * 8.0_vt);
    ASSERT_EQ(interval2.upper, INFINITE_VALUE);
}

TEST(IntervalTests, test_interval_length)
{
    Interval interval(8.0_vt, 40.0_vt);
    ASSERT_EQ(interval.length(), 40.0_vt - 8.0_vt);
}

TEST(IntervalTests, test_interval_length2)
{
    Interval interval(8.0_vt, INFINITE_VALUE);
    ASSERT_EQ(interval.length(), INFINITE_VALUE);
}

TEST(IntervalTests, test_interval_length3)
{
    Interval interval(-INFINITE_VALUE, 8.0_vt);
    ASSERT_EQ(interval.length(), INFINITE_VALUE);
}

TEST(IntervalTests, test_interval_length4)
{
    Interval interval(-INFINITE_VALUE, INFINITE_VALUE);
    ASSERT_EQ(interval.length(), INFINITE_VALUE);
}

TEST(IntervalTests, test_interval_length5)
{
    Interval interval(INFINITE_VALUE, INFINITE_VALUE);
    ASSERT_EQ(interval.length(), 0.0_vt);
}

TEST(IntervalTests, test_interval_length6)
{
    Interval interval(-INFINITE_VALUE, -INFINITE_VALUE);
    ASSERT_EQ(interval.length(), 0.0_vt);
}

TEST(IntervalTests, test_interval_set_min)
{
    Interval interval(8.0_vt, 40.0_vt);
    Interval interval2(-45.0_vt, 30.0_vt);

    probfd::engines::set_min(interval, interval2);

    ASSERT_EQ(interval.lower, -45.0_vt);
    ASSERT_EQ(interval.upper, 30.0_vt);
}

TEST(IntervalTests, test_interval_set_min2)
{
    Interval interval(8.0_vt, INFINITE_VALUE);
    Interval interval2(INFINITE_VALUE, 30.0_vt);

    probfd::engines::set_min(interval, interval2);

    ASSERT_EQ(interval.lower, 8.0_vt);
    ASSERT_EQ(interval.upper, 30.0_vt);
}

TEST(IntervalTests, test_interval_set_min3)
{
    Interval interval(INFINITE_VALUE, INFINITE_VALUE);
    Interval interval2(-INFINITE_VALUE, INFINITE_VALUE);

    probfd::engines::set_min(interval, interval2);

    ASSERT_EQ(interval.lower, -INFINITE_VALUE);
    ASSERT_EQ(interval.upper, INFINITE_VALUE);
}

TEST(IntervalTests, test_interval_update1)
{
    Interval interval(8.0_vt, 40.0_vt);
    Interval interval2(-45.0_vt, 30.0_vt);

    bool result = probfd::engines::update(interval, interval2);

    ASSERT_TRUE(result);
    ASSERT_EQ(interval.lower, std::max(8.0_vt, -45.0_vt));
    ASSERT_EQ(interval.upper, std::min(40.0_vt, 30.0_vt));
}

TEST(IntervalTests, test_interval_update2)
{
    Interval interval(8.0_vt, 40.0_vt);
    Interval interval2(7.0_vt, 41.0_vt);

    bool result = probfd::engines::update(interval, interval2);

    ASSERT_FALSE(result);
    ASSERT_EQ(interval.lower, std::max(8.0_vt, 7.0_vt));
    ASSERT_EQ(interval.upper, std::min(40.0_vt, 41.0_vt));
}

TEST(IntervalTests, test_interval_update3)
{
    Interval interval(8.0_vt, 40.0_vt);
    Interval interval2(25.0_vt, 30.0_vt);

    bool result = probfd::engines::update(interval, interval2, false);

    ASSERT_TRUE(result);
    ASSERT_EQ(interval.lower, std::max(8.0_vt, 25.0_vt));
    ASSERT_EQ(interval.upper, std::min(40.0_vt, 30.0_vt));
}

TEST(IntervalTests, test_interval_update4)
{
    Interval interval(8.0_vt, 40.0_vt);
    Interval interval2(7.0_vt, 39.0_vt);

    bool result = probfd::engines::update(interval, interval2, false);

    ASSERT_FALSE(result);
    ASSERT_EQ(interval.lower, std::max(8.0_vt, 7.0_vt));
    ASSERT_EQ(interval.upper, std::min(40.0_vt, 39.0_vt));
}