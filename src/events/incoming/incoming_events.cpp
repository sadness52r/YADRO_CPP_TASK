#include <sstream>

#include "incoming_events.hpp"

// void ClientArrivedEvent::execute(Club& club) {
//     club.handle_client_arrived(client_name, time);
// }

std::string ClientArrivedEvent::to_string() const {
    std::ostringstream oss;
    oss << time.to_string() << " " << to_int(id()) << " " << client_name;
    return oss.str();
}

// void ClientSatDownEvent::execute(Club& club) {
//     club.handle_client_sat_down(client_name, place_number, time);
// }

std::string ClientSatDownEvent::to_string() const {
    std::ostringstream oss;
    oss << time.to_string() << " " << to_int(id()) << " " << client_name << " " << place_number;
    return oss.str();
}

// void ClientWaitingEvent::execute(Club& club) {
//     club.handle_client_waiting(client_name, time);
// }

std::string ClientWaitingEvent::to_string() const {
    std::ostringstream oss;
    oss << time.to_string() << " " << to_int(id()) << " " << client_name;
    return oss.str();
}

// void ClientLeftEvent::execute(Club& club) {
//     club.handle_client_left(client_name, time);
// }

std::string ClientLeftEvent::to_string() const {
    std::ostringstream oss;
    oss << time.to_string() << " " << to_int(id()) << " " << client_name;
    return oss.str();
}