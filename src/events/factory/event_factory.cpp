#include <sstream>
#include <vector>
#include <stdexcept>
#include <regex>

#include "event_factory.hpp"
#include "events/IncomingEvents.h"


bool EventFactory::validate_client_name(const std::string& name) {
    return std::regex_match(name, std::regex("^[a-z0-9_-]+$"));
}

std::unique_ptr<Event> EventFactory::parse_line(const std::string& line, int line_number) {
    if (line.empty()) {
        throw std::invalid_argument("Empty line");
    }
    
    std::istringstream iss(line);
    std::vector<std::string> tokens;
    std::string token;
    while (iss >> token) {
        tokens.push_back(token);
    }
    
    if (tokens.size() < 3) {
        throw std::invalid_argument("Invalid event format");
    }
    
    Time time;
    try {
        time = Time::from_string(tokens[0]);
    } catch (...) {
        throw std::invalid_argument("Invalid time format");
    }

    unsigned int event_id;
    try {
        event_id = std::stoi(tokens[1]);
    } catch (...) {
        throw std::invalid_argument("Invalid event ID");
    }

    EventType event_type;
    if (EVENT_ID_TO_TYPE.find(event_id) == EVENT_ID_TO_TYPE.end()) {
        throw std::invalid_argument("Invalid event ID");
    }
    event_type = EVENT_ID_TO_TYPE[event_id];
    
    switch (event_type) {
        case EventType::ClientArrived: {
            if (tokens.size() != 3) throw std::invalid_argument("Invalid event format");
            if (!validate_client_name(tokens[2])) throw std::invalid_argument("Invalid client name");
            return std::make_unique<ClientArrivedEvent>(time, tokens[2]);
        }
        case EventType::ClientSatDown: {
            if (tokens.size() != 4) throw std::invalid_argument("Invalid event format");
            if (!validate_client_name(tokens[2])) throw std::invalid_argument("Invalid client name");
            try {
                unsigned int place_num = std::stoi(tokens[3]);
            } catch (...) {
                throw std::invalid_argument("Invalid place number");
            }
            if (place_num < 1) throw std::invalid_argument("Invalid place number");
            return std::make_unique<ClientSatDownEvent>(time, tokens[2], std::stoi(tokens[3]));
        }
        case EventType::ClientWaiting: {
            if (tokens.size() != 3) throw std::invalid_argument("Invalid event format");
            if (!validate_client_name(tokens[2])) throw std::invalid_argument("Invalid client name");
            return std::make_unique<ClientWaitingEvent>(time, tokens[2]);
        }
        case EventType::ClientLeft: {
            if (tokens.size() != 3) throw std::invalid_argument("Invalid event format");
            if (!validate_client_name(tokens[2])) throw std::invalid_argument("Invalid client name");
            return std::make_unique<ClientLeftEvent>(time, tokens[2]);
        }
        default:
            throw std::invalid_argument("Unknown event type");
    }
}