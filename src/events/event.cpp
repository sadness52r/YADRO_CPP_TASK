#include "event.hpp"
#include "events/outgoing/outgoing_events.hpp"

const Time Event::get_time() const { 
    return time; 
}

std::unique_ptr<Event> Event::create_error_event(const Time time, const std::string& error_msg) {
    return std::make_unique<ErrorEvent>(time, error_msg);
}

std::unique_ptr<Event> Event::create_client_seated_event(Time time, const std::string& client, unsigned int place) {
    return std::make_unique<ClientSeatedFromQueueEvent>(time, client, place);
}

std::unique_ptr<Event> Event::create_client_forced_left_event(Time time, const std::string& client) {
    return std::make_unique<ClientForcedLeftEvent>(time, client);
}