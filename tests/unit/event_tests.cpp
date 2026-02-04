#include <gtest/gtest.h>

#include "../../src/events/incoming/incoming_events.hpp"
#include "../../src/events/outgoing/outgoing_events.hpp"
#include "../../src/utils/time/time.hpp"

class EventTest : public ::testing::Test {
protected:
    Time t{"10:30"};
};

TEST_F(EventTest, ClientArrivedEvent) {
    ClientArrivedEvent event(t, "alice");
    EXPECT_EQ(event.id(), EventType::ClientArrived);
    EXPECT_EQ(event.to_string(), "10:30 1 alice");
}

TEST_F(EventTest, ClientSatDownEvent) {
    ClientSatDownEvent event(t, "bob", 3);
    EXPECT_EQ(event.id(), EventType::ClientSatDown);
    EXPECT_EQ(event.to_string(), "10:30 2 bob 3");
    EXPECT_EQ(event.get_place_number(), 3);
}

TEST_F(EventTest, ErrorEvent) {
    ErrorEvent event(t, "NotOpenYet");
    EXPECT_EQ(event.id(), EventType::Error);
    EXPECT_EQ(event.to_string(), "10:30 13 NotOpenYet");
    EXPECT_EQ(event.get_error_msg(), "NotOpenYet");
}

TEST_F(EventTest, ClientForcedLeftEvent) {
    ClientForcedLeftEvent event(t, "eve");
    EXPECT_EQ(event.id(), EventType::ClientForcedLeft);
    EXPECT_EQ(event.to_string(), "10:30 11 eve");
}