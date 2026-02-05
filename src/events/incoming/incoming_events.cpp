#include <sstream>

#include "incoming_events.hpp"
#include "core/club/club.hpp"

void ClientArrivedEvent::execute(Club& club) {
    club.handle_client_arrived(client_name, time);
}

std::string ClientArrivedEvent::to_string() const {
    std::ostringstream oss;
    oss << time.to_string() << " " << to_int(id()) << " " << client_name;
    return oss.str();
}

EventType ClientArrivedEvent::id() const {
    return EventType::ClientArrived;
}

const std::string& ClientArrivedEvent::get_client_name() const { 
    return client_name; 
}

void ClientSatDownEvent::execute(Club& club) {
    club.handle_client_sat_down(client_name, place_number, time);
}

std::string ClientSatDownEvent::to_string() const {
    std::ostringstream oss;
    oss << time.to_string() << " " << to_int(id()) << " " << client_name << " " << place_number;
    return oss.str();
}

EventType ClientSatDownEvent::id() const {
    return EventType::ClientSatDown; 
}

const std::string& ClientSatDownEvent::get_client_name() const {
    return client_name; 
}

int ClientSatDownEvent::get_place_number() const { 
    return place_number; 
}

void ClientWaitingEvent::execute(Club& club) {
    club.handle_client_waiting(client_name, time);
}

std::string ClientWaitingEvent::to_string() const {
    std::ostringstream oss;
    oss << time.to_string() << " " << to_int(id()) << " " << client_name;
    return oss.str();
}

EventType ClientWaitingEvent::id() const { 
    return EventType::ClientWaiting; 
}

const std::string& ClientWaitingEvent::get_client_name() const {
    return client_name; 
}

void ClientLeftEvent::execute(Club& club) {
    club.handle_client_left(client_name, time);
}

std::string ClientLeftEvent::to_string() const {
    std::ostringstream oss;
    oss << time.to_string() << " " << to_int(id()) << " " << client_name;
    return oss.str();
}

EventType ClientLeftEvent::id() const { 
    return EventType::ClientLeft; 
}

const std::string& ClientLeftEvent::get_client_name() const { 
    return client_name;
}