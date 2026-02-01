#pragma once

#include <string>
#include <memory>

#include "../../utils/time/time.hpp"

class Club;

class Event {
protected:
    Time time;

public:
    Event(Time time) : time(time) {}
    virtual ~Event() = default;

    virtual void execute(Club& club) = 0;
    virtual std::string to_string() const = 0;
    virtual int id() const = 0;

    const Time get_time() const;

    static std::unique_ptr<Event> create_error_event(const Time time, const std::string& error_msg);
    static std::unique_ptr<Event> create_client_seated_event(const Time time, 
        const std::string& client, const int place);
    static std::unique_ptr<Event> create_client_left_event(const Time time, const std::string& client);
};
