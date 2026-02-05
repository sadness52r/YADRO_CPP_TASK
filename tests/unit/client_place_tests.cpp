#include <gtest/gtest.h>

#include "core/client/client.hpp"
#include "core/place/place.hpp"
#include "utils/time/time.hpp"
#include "core/waiting_queue/waiting_queue.hpp"

TEST(ClientTest, InitialState) {
    Client c("alice");
    EXPECT_EQ(c.get_name(), "alice");
    EXPECT_FALSE(c.get_place_num().has_value());
}

TEST(PlaceTest, OccupyAndRelease) {
    Place place(1);
    Time start("10:00");
    Time end("11:30");
    
    EXPECT_FALSE(place.is_occupied());
    place.occupy("alice", start);
    EXPECT_TRUE(place.is_occupied());
    EXPECT_TRUE(place.get_сlient());
    EXPECT_EQ(place.get_сlient().value(), "alice");
    
    place.release(end, 100);
    EXPECT_FALSE(place.is_occupied());
    EXPECT_EQ(place.get_total_minutes(), 90);
    EXPECT_EQ(place.get_income(), 200);
}

TEST(PlaceTest, RoundUpBilling) {
    Place place(2);
    place.occupy("bob", Time("10:00"));
    place.release(Time("10:01"), 50);
    EXPECT_EQ(place.get_income(), 50);
    EXPECT_EQ(place.get_total_minutes(), 1);
}

TEST(WaitingQueueTest, BasicOperations) {
    WaitingQueue q(3);
    
    EXPECT_FALSE(q.is_full());
    q.add("alice");
    q.add("bob");
    EXPECT_EQ(q.size(), 2);
    
    q.add("charlie");
    EXPECT_TRUE(q.is_full());
    
    auto next = q.pop();
    EXPECT_TRUE(next.has_value());
    EXPECT_EQ(next.value(), "alice");
    EXPECT_EQ(q.size(), 2);
}