#pragma once

#include <string>
#include <optional>

#include "../event.hpp"

class ClientArrivedEvent : public Event {
private:
    std::string client_name;

public:
    ClientArrivedEvent(Time time, std::string client_name)
        : Event(time), client_name(std::move(client_name)) {}

    void execute(Club& club) override;
    std::string to_string() const override;
    EventType id() const override { return EventType::ClientArrived; }
    const std::string& get_client_name() const { return client_name; }
};

class ClientSatDownEvent : public Event {
private:
    std::string client_name;
    int place_number;

public:
    ClientSatDownEvent(Time time, std::string client_name, int place_number)
        : Event(time), client_name(std::move(client_name)), place_number(place_number) {}

    void execute(Club& club) override;
    std::string to_string() const override;
    EventType id() const override { return EventType::ClientSatDown; }
    const std::string& get_client_name() const { return client_name; }
    int get_place_number() const { return place_number; }
};

class ClientWaitingEvent : public Event {
private:
    std::string client_name;

public:
    ClientWaitingEvent(Time time, std::string client_name)
        : Event(time), client_name(std::move(client_name)) {}

    void execute(Club& club) override;
    std::string to_string() const override;
    EventType id() const override { return EventType::ClientWaiting; }
    const std::string& get_client_name() const { return client_name; }
};

class ClientLeftEvent : public Event {
private:
    std::string client_name;

public:
    ClientLeftEvent(Time time, std::string client_name)
        : Event(time), client_name(std::move(client_name)) {}

    void execute(Club& club) override;
    std::string to_string() const override;
    EventType id() const override { return EventType::ClientLeft; }
    const std::string& get_client_name() const { return client_name; }
};