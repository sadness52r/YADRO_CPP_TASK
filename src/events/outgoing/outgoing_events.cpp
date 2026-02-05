#include <sstream>

#include "core/club/club.hpp"
#include "outgoing_events.hpp"

void ClientForcedLeftEvent::execute(Club& club) {
    club.handle_client_forced_left(client_name, time);
}

std::string ClientForcedLeftEvent::to_string() const {
    std::ostringstream oss;
    oss << time.to_string() << " " << to_int(id()) << " " << client_name;
    return oss.str();
}

EventType ClientForcedLeftEvent::id() const { 
    return EventType::ClientForcedLeft; 
}

void ClientSeatedFromQueueEvent::execute(Club& club) {
    club.handle_client_seated_from_queue(client_name, place_number, time);
}

std::string ClientSeatedFromQueueEvent::to_string() const {
    std::ostringstream oss;
    oss << time.to_string() << " " << to_int(id()) << " " << client_name << " " << place_number;
    return oss.str();
}

EventType ClientSeatedFromQueueEvent::id() const { 
    return EventType::ClientSeatedFromQueue;
}

void ErrorEvent::execute(Club& club) {}

std::string ErrorEvent::to_string() const {
    std::ostringstream oss;
    oss << time.to_string() << " " << to_int(id()) << " " << error_msg;
    return oss.str();
}

EventType ErrorEvent::id() const { 
    return EventType::Error; 
}

const std::string& ErrorEvent::get_error_msg() const { 
    return error_msg; 
}