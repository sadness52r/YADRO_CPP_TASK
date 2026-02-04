#pragma once

#include <string>
#include <memory>
#include <unordered_map>

#include "../utils/time/time.hpp"


enum class EventType : int {
    ClientArrived = 1,
    ClientSatDown = 2,
    ClientWaiting = 3,
    ClientLeft = 4,
    
    ClientForcedLeft = 11,
    ClientSeatedFromQueue = 12,
    Error = 13
};

inline int to_int(EventType type) {
    return static_cast<int>(type);
}

inline std::unordered_map<unsigned int, EventType> EVENT_ID_TO_TYPE = {
    {1, EventType::ClientArrived},
    {2, EventType::ClientSatDown},
    {3, EventType::ClientWaiting},
    {4, EventType::ClientLeft},
    {11, EventType::ClientForcedLeft},
    {12, EventType::ClientSeatedFromQueue},
    {13, EventType::Error}
};


class Club;

class Event {
protected:
    Time time;

public:
    Event(Time time) : time(time) {}
    virtual ~Event() = default;

    virtual void execute(Club& club) = 0;
    virtual std::string to_string() const = 0;
    virtual EventType id() const = 0;

    const Time get_time() const;

    static std::unique_ptr<Event> create_error_event(const Time time, const std::string& error_msg);
    static std::unique_ptr<Event> create_client_seated_event(const Time time, 
        const std::string& client, const int place);
    //static std::unique_ptr<Event> create_client_forced_left_event(Time time, const std::string& client);
    static std::unique_ptr<Event> create_client_left_event(const Time time, const std::string& client);
};
