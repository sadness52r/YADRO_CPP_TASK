#pragma once

#include <string>

#include "events/event.hpp"

class ClientForcedLeftEvent : public Event {
private:
    std::string client_name;

public:
    ClientForcedLeftEvent(Time time, std::string client_name)
        : Event(time), client_name(std::move(client_name)) {}

    void execute(Club& club) override;
    std::string to_string() const override;
    EventType id() const override;
};

class ClientSeatedFromQueueEvent : public Event {
private:
    std::string client_name;
    unsigned int place_number;

public:
    ClientSeatedFromQueueEvent(Time time, std::string client_name, unsigned int place_number)
        : Event(time), client_name(std::move(client_name)), place_number(place_number) {}

    void execute(Club& club) override;
    std::string to_string() const override;
    EventType id() const override;
};

class ErrorEvent : public Event {
private:
    std::string error_msg;

public:
    ErrorEvent(Time time, std::string error_msg)
        : Event(time), error_msg(std::move(error_msg)) {}

    void execute(Club& club) override;
    std::string to_string() const override;
    EventType id() const override;
    const std::string& get_error_msg() const;
};