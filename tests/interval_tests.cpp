#include <gtest/gtest.h>

#include "probfd/value_type.h"
#include "probfd/value_utils.h"

TEST(IntervalTests, test_as_lower_bound)
{
    using namespace probfd;
    Interval interval(0.0_vt, 1.0_vt);
    ASSERT_EQ(probfd::as_lower_bound(interval), 0.0_vt);
}

TEST(IntervalTests, test_as_upper_bound)
{
    using namespace probfd;
    Interval interval(0.0_vt, 1.0_vt);
    ASSERT_EQ(probfd::as_upper_bound(interval), 1.0_vt);
}

TEST(IntervalTests, test_approx_less)
{
    using namespace probfd;
    ASSERT_TRUE(probfd::is_approx_less(0.0_vt, 0.6_vt, 0.5_vt));
}

TEST(IntervalTests, test_approx_equal)
{
    using namespace probfd;
    ASSERT_TRUE(probfd::is_approx_equal(0.0_vt, 0.0_vt, 0.5_vt));
}

TEST(IntervalTests, test_approx_greater)
{
    using namespace probfd;
    ASSERT_TRUE(probfd::is_approx_greater(0.6_vt, 0.0_vt, 0.5_vt));
}

TEST(IntervalTests, test_interval_default_ctor)
{
    using namespace probfd;
    Interval interval;
    ASSERT_EQ(interval.lower, 0.0_vt);
    ASSERT_EQ(interval.upper, 0.0_vt);
}

TEST(IntervalTests, test_interval_single_ctor)
{
    using namespace probfd;
    Interval interval(5.0_vt);
    ASSERT_EQ(interval.lower, 5.0_vt);
    ASSERT_EQ(interval.upper, 5.0_vt);
}

TEST(IntervalTests, test_interval_two_bounds_ctor)
{
    using namespace probfd;
    Interval interval(42.0_vt, 45.0_vt);
    ASSERT_EQ(interval.lower, 42.0_vt);
    ASSERT_EQ(interval.upper, 45.0_vt);
}

TEST(IntervalTests, test_interval_plus_fin)
{
    using namespace probfd;
    Interval interval(8.0_vt, 40.0_vt);
    Interval interval2(5.0_vt, 75.0_vt);

    auto interval3 = interval + interval2;

    ASSERT_EQ(interval3.lower, 8.0_vt + 5.0_vt);
    ASSERT_EQ(interval3.upper, 40.0_vt + 75.0_vt);
}

TEST(IntervalTests, test_interval_plus_inf)
{
    using namespace probfd;
    Interval interval(8.0_vt, INFINITE_VALUE);
    Interval interval2(INFINITE_VALUE, 75.0_vt);

    auto interval3 = interval + interval2;

    ASSERT_EQ(interval3.lower, INFINITE_VALUE);
    ASSERT_EQ(interval3.upper, INFINITE_VALUE);
}

TEST(IntervalTests, test_interval_times_fin)
{
    using namespace probfd;
    Interval interval(8.0_vt, 40.0_vt);

    auto interval2 = 20.0_vt * interval;

    ASSERT_EQ(interval2.lower, 20.0_vt * 8.0_vt);
    ASSERT_EQ(interval2.upper, 20.0_vt * 40.0_vt);
}

TEST(IntervalTests, test_interval_times2)
{
    using namespace probfd;
    Interval interval(-6.8, 4.3);

    auto interval2 = INFINITE_VALUE * interval;

    ASSERT_EQ(interval2.lower, -INFINITE_VALUE);
    ASSERT_EQ(interval2.upper, INFINITE_VALUE);
}

TEST(IntervalTests, test_interval_times3)
{
    using namespace probfd;
    Interval interval(-INFINITE_VALUE, 5.0);

    auto interval2 = -INFINITE_VALUE * interval;

    ASSERT_EQ(interval2.lower, INFINITE_VALUE);
    ASSERT_EQ(interval2.upper, -INFINITE_VALUE);
}

TEST(IntervalTests, test_interval_times4)
{
    using namespace probfd;
    Interval interval(8.0_vt, -INFINITE_VALUE);

    auto interval2 = -20.0_vt * interval;

    ASSERT_EQ(interval2.lower, -20.0_vt * 8.0_vt);
    ASSERT_EQ(interval2.upper, INFINITE_VALUE);
}

TEST(IntervalTests, test_interval_length)
{
    using namespace probfd;
    Interval interval(8.0_vt, 40.0_vt);

    ASSERT_EQ(interval.length(), 40.0_vt - 8.0_vt);
}

TEST(IntervalTests, test_interval_length2)
{
    using namespace probfd;
    Interval interval(8.0_vt, INFINITE_VALUE);

    ASSERT_EQ(interval.length(), INFINITE_VALUE);
}

TEST(IntervalTests, test_interval_length3)
{
    using namespace probfd;
    Interval interval(-INFINITE_VALUE, 8.0_vt);

    ASSERT_EQ(interval.length(), INFINITE_VALUE);
}

TEST(IntervalTests, test_interval_length4)
{
    using namespace probfd;
    Interval interval(-INFINITE_VALUE, INFINITE_VALUE);

    ASSERT_EQ(interval.length(), INFINITE_VALUE);
}

TEST(IntervalTests, test_interval_length5)
{
    using namespace probfd;
    Interval interval(INFINITE_VALUE, INFINITE_VALUE);

    ASSERT_EQ(interval.length(), 0.0_vt);
}

TEST(IntervalTests, test_interval_length6)
{
    using namespace probfd;
    Interval interval(-INFINITE_VALUE, -INFINITE_VALUE);

    ASSERT_EQ(interval.length(), 0.0_vt);
}

TEST(IntervalTests, test_approx_compare1)
{
    using namespace probfd;
    Interval interval(3.6_vt, 7.3_vt);
    Interval interval2(3.7_vt, 7.4_vt);

    ASSERT_EQ(probfd::approx_compare(interval, interval2, 0.2_vt), 0);
}

TEST(IntervalTests, test_approx_compare2)
{
    using namespace probfd;
    Interval interval(3.0_vt, 7.3_vt);
    Interval interval2(3.0_vt, 7.7_vt);

    ASSERT_EQ(probfd::approx_compare(interval, interval2, 0.2_vt), -1);
}

TEST(IntervalTests, test_approx_compare3)
{
    using namespace probfd;
    Interval interval(4.0_vt, 7.3_vt);
    Interval interval2(4.7_vt, 7.0_vt);

    ASSERT_EQ(probfd::approx_compare(interval, interval2, 0.2_vt), 1);
}

TEST(IntervalTests, test_approx_compare4)
{
    using namespace probfd;
    Interval interval(0.0_vt, INFINITE_VALUE);
    Interval interval2(0.0_vt, INFINITE_VALUE);

    ASSERT_EQ(probfd::approx_compare(interval, interval2, 0.2_vt), 0);
}

TEST(IntervalTests, test_approx_compare5)
{
    using namespace probfd;
    Interval interval(0.0_vt, 7.3_vt);
    Interval interval2(0.0_vt, INFINITE_VALUE);

    ASSERT_EQ(probfd::approx_compare(interval, interval2, 0.2_vt), -1);
}

TEST(IntervalTests, test_approx_compare6)
{
    using namespace probfd;
    Interval interval(0.0_vt, INFINITE_VALUE);
    Interval interval2(0.0_vt, 7.4_vt);

    ASSERT_EQ(probfd::approx_compare(interval, interval2, 0.2), 1);
}

TEST(IntervalTests, test_interval_set_min)
{
    using namespace probfd;
    Interval interval(8.0_vt, 40.0_vt);
    Interval interval2(-45.0_vt, 30.0_vt);

    probfd::set_min(interval, interval2);

    ASSERT_EQ(interval.lower, -45.0_vt);
    ASSERT_EQ(interval.upper, 30.0_vt);
}

TEST(IntervalTests, test_interval_set_min2)
{
    using namespace probfd;
    Interval interval(8.0_vt, INFINITE_VALUE);
    Interval interval2(INFINITE_VALUE, 30.0_vt);

    probfd::set_min(interval, interval2);

    ASSERT_EQ(interval.lower, 8.0_vt);
    ASSERT_EQ(interval.upper, 30.0_vt);
}

TEST(IntervalTests, test_interval_set_min3)
{
    using namespace probfd;
    Interval interval(INFINITE_VALUE, INFINITE_VALUE);
    Interval interval2(-INFINITE_VALUE, INFINITE_VALUE);

    probfd::set_min(interval, interval2);

    ASSERT_EQ(interval.lower, -INFINITE_VALUE);
    ASSERT_EQ(interval.upper, INFINITE_VALUE);
}

TEST(IntervalTests, test_interval_update1)
{
    using namespace probfd;
    Interval interval(8.0_vt, 40.0_vt);
    Interval interval2(-45.0_vt, 30.0_vt);

    bool result = probfd::update(interval, interval2);

    ASSERT_TRUE(result);
    ASSERT_EQ(interval.lower, std::max(8.0_vt, -45.0_vt));
    ASSERT_EQ(interval.upper, std::min(40.0_vt, 30.0_vt));
}

TEST(IntervalTests, test_interval_update2)
{
    using namespace probfd;
    Interval interval(8.0_vt, 40.0_vt);
    Interval interval2(7.0_vt, 41.0_vt);

    bool result = probfd::update(interval, interval2);

    ASSERT_FALSE(result);
    ASSERT_EQ(interval.lower, std::max(8.0_vt, 7.0_vt));
    ASSERT_EQ(interval.upper, std::min(40.0_vt, 41.0_vt));
}

TEST(IntervalTests, test_interval_update3)
{
    using namespace probfd;
    Interval interval(8.0_vt, 40.0_vt);
    Interval interval2(25.0_vt, 30.0_vt);

    bool result = probfd::update(interval, interval2, false);

    ASSERT_TRUE(result);
    ASSERT_EQ(interval.lower, std::max(8.0_vt, 25.0_vt));
    ASSERT_EQ(interval.upper, std::min(40.0_vt, 30.0_vt));
}

TEST(IntervalTests, test_interval_update4)
{
    using namespace probfd;
    Interval interval(8.0_vt, 40.0_vt);
    Interval interval2(7.0_vt, 39.0_vt);

    bool result = probfd::update(interval, interval2, false);

    ASSERT_FALSE(result);
    ASSERT_EQ(interval.lower, std::max(8.0_vt, 7.0_vt));
    ASSERT_EQ(interval.upper, std::min(40.0_vt, 39.0_vt));
}