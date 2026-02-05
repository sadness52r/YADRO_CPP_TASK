#include <gtest/gtest.h>
#include <memory>

#include "core/club/club.hpp"
#include "events/incoming/incoming_events.hpp"
#include "events/outgoing/outgoing_events.hpp"
#include "utils/time/time.hpp"


class ClubTest : public ::testing::Test {
protected:
    Club club{3, Time("09:00"), Time("20:00"), 100};
    
    void add_client(const std::string& name) {
        auto event = std::make_unique<ClientArrivedEvent>(Time("09:10"), name);
        event->execute(club);
    }
    
    void seat_client(const std::string& name, unsigned int place) {
        auto event = std::make_unique<ClientSatDownEvent>(Time("09:15"), name, place);
        event->execute(club);
    }
    
    void leave_client(const std::string& name) {
        auto event = std::make_unique<ClientLeftEvent>(Time("10:00"), name);
        event->execute(club);
    }
};

TEST_F(ClubTest, OpenHoursCheck) {
    EXPECT_TRUE(club.is_open(Time("09:00")));
    EXPECT_TRUE(club.is_open(Time("15:00")));
    EXPECT_TRUE(club.is_open(Time("20:00")));
    EXPECT_FALSE(club.is_open(Time("08:59")));
    EXPECT_FALSE(club.is_open(Time("20:01")));
}

TEST_F(ClubTest, ClientArrivalDuringBusinessHours) {
    EXPECT_FALSE(club.is_client_inside("alice"));
    
    auto event = std::make_unique<ClientArrivedEvent>(Time("10:00"), "alice");
    event->execute(club);
    
    EXPECT_TRUE(club.is_client_inside("alice"));
}

TEST_F(ClubTest, ClientArrivalBeforeOpening) {
    auto event = std::make_unique<ClientArrivedEvent>(Time("08:59"), "bob");
    EXPECT_THROW(event->execute(club), std::runtime_error);
}

TEST_F(ClubTest, DoubleArrival) {
    add_client("alice");
    auto event = std::make_unique<ClientArrivedEvent>(Time("09:20"), "alice");
    EXPECT_THROW(event->execute(club), std::runtime_error);
}

TEST_F(ClubTest, SeatAtOccupiedTable) {
    add_client("alice");
    seat_client("alice", 1);
    
    add_client("bob");
    auto event = std::make_unique<ClientSatDownEvent>(Time("09:20"), "bob", 1);
    EXPECT_THROW(event->execute(club), std::runtime_error);
}

TEST_F(ClubTest, PlaceOccupancyAndRevenue) {
    add_client("alice");
    seat_client("alice", 1);
    
    Time leave_time("10:45");
    auto leave_event = std::make_unique<ClientLeftEvent>(leave_time, "alice");
    leave_event->execute(club);
    
    const auto& place = club.get_places()[0];
    EXPECT_FALSE(place.is_occupied());
    EXPECT_EQ(place.get_total_minutes(), 90);
    EXPECT_EQ(place.get_income(), 200);
}

TEST_F(ClubTest, WaitingQueueOverflow) {
    for (int i = 1; i <= 3; ++i) {
        add_client("client" + std::to_string(i));
        seat_client("client" + std::to_string(i), i);
    }
    
    for (int i = 4; i <= 6; ++i) {
        add_client("client" + std::to_string(i));
        auto wait_event = std::make_unique<ClientWaitingEvent>(Time("10:00"), "client" + std::to_string(i));
        wait_event->execute(club);
    }
    
    add_client("client7");
    auto wait_event = std::make_unique<ClientWaitingEvent>(Time("10:05"), "client7");
    wait_event->execute(club);
    
    EXPECT_FALSE(club.is_client_inside("client7"));
}

TEST_F(ClubTest, AutoSeatFromQueue) {
    add_client("alice"); seat_client("alice", 1);
    add_client("bob");   seat_client("bob", 2);
    add_client("dave");  seat_client("dave", 3);
    
    add_client("charlie");
    auto wait_event = std::make_unique<ClientWaitingEvent>(Time("10:05"), "charlie");
    wait_event->execute(club);
    
    leave_client("alice");
    
    EXPECT_TRUE(club.is_client_seated("charlie"));
}