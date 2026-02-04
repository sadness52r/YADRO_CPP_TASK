#include <gtest/gtest.h>

#include "../../src/utils/time/time.hpp"

TEST(TimeTest, ParseValidTime) {
    Time t("09:30");
    EXPECT_EQ(t.get_minutes(), 570);
    EXPECT_EQ(t.to_string(), "09:30");
}

TEST(TimeTest, ParseInvalidTime) {
    EXPECT_THROW(Time("25:00"), std::invalid_argument);
    EXPECT_THROW(Time("09:60"), std::invalid_argument);
    EXPECT_THROW(Time("9:30"), std::invalid_argument);
    EXPECT_THROW(Time("09-30"), std::invalid_argument);
}

TEST(TimeTest, ComparisonOperators) {
    Time t1("10:00");
    Time t2("10:30");
    Time t3("10:00");
    
    EXPECT_TRUE(t1 < t2);
    EXPECT_TRUE(t1 <= t3);
    EXPECT_TRUE(t2 > t1);
    EXPECT_TRUE(t3 >= t1);
    EXPECT_TRUE(t1 == t3);
    EXPECT_TRUE(t1 != t2);
}

TEST(TimeTest, TimeArithmetic) {
    Time t("09:15");
    Time t_plus = t + 45;
    EXPECT_EQ(t_plus.to_string(), "10:00");
    
    Time t_minus = t - 15;
    EXPECT_EQ(t_minus.to_string(), "09:00");
    
    EXPECT_EQ(t.diff_minutes(t_minus), 15);
}