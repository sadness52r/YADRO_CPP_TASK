#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <vector>
#include <iterator>
#include <regex>

#include "parser.hpp"
#include "utils/time/time.hpp"
#include "events/factory/event_factory.hpp"


bool Parser::validate_client_name(const std::string& name) {
    return std::regex_match(name, std::regex("^[a-z0-9_-]+$"));
}

Club Parser::parse_file(const std::string& filename, 
    std::vector<std::unique_ptr<Event>>& out_events) {

    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open file: " + filename);
    }

    Parser::ConfigTokens config_tokens = parse_header(file);

    unsigned int places_count = std::get<0>(config_tokens);
    Time topen = std::get<1>(config_tokens);
    Time tclose = std::get<2>(config_tokens);
    unsigned int price_per_hour = std::get<3>(config_tokens);

    std::vector<EventTokens> all_events_tokens = 
                parse_events(file, places_count, out_events);

    for (const auto& event_tokens : all_events_tokens)
    {
        auto event = EventFactory::get_event(event_tokens);
        out_events.push_back(std::move(event));
    }

    return Club(places_count, topen, tclose, price_per_hour);
}

Parser::ConfigTokens Parser::parse_header(std::ifstream& file) {
    std::string line;
    if (!std::getline(file, line)) {
        throw std::invalid_argument(line);
    }

    line.erase(0, line.find_first_not_of(" \t"));
    if (line.empty()) {
        throw std::invalid_argument(line);
    }   

    std::istringstream iss(line);
    int places_count;
    iss >> places_count;
    if (!iss) {
        throw std::invalid_argument(line);
    }
    std::string extra;
    iss >> extra;
    if (iss) {
        throw std::invalid_argument(line);
    }
        
    if (places_count <= 0)  {
        throw std::invalid_argument(line);
    }
    
    if (!std::getline(file, line)) {
        throw std::invalid_argument(line);
    }
    
    line.erase(0, line.find_first_not_of(" \t"));
    if (line.empty()) {
        throw std::invalid_argument(line);
    }
    
    std::istringstream time_stream(line);
    std::string topen_str;
    time_stream >> topen_str;
    if (!time_stream) {
        throw std::invalid_argument(line);
    }
    std::string tclose_str;
    time_stream >> tclose_str;
    if (!time_stream) {
        throw std::invalid_argument(line);
    }
    time_stream >> extra;
    if (time_stream) {
        throw std::invalid_argument(line);
    }

    Time open_time, close_time;
    try {
        open_time = Time::from_string(topen_str);
        close_time = Time::from_string(tclose_str);
    } catch (...) {
        throw std::invalid_argument(line);
    }

    if (open_time >= close_time) {
        throw std::invalid_argument(line);
    }
    
    if (!std::getline(file, line)) {
        throw std::invalid_argument(line);
    }

    line.erase(0, line.find_first_not_of(" \t"));
    if (line.empty()){
        throw std::invalid_argument(line);
    } 

    std::istringstream iss2(line);
    int price_per_hour;
    iss2 >> price_per_hour;
    if (!iss2) {
        throw std::invalid_argument(line);
    }
    iss2 >> extra;
    if (iss2) {
        throw std::invalid_argument(line);
    }

    if (price_per_hour <= 0) {
        throw std::invalid_argument(line);
    }
    
    return std::make_tuple(static_cast<unsigned int>(places_count), open_time, close_time,
                static_cast<unsigned int>(price_per_hour));
}

std::vector<EventTokens> Parser::parse_events(std::ifstream& file, 
    unsigned int places_count, std::vector<std::unique_ptr<Event>>& out_events) {

    std::string line;
    out_events.clear();

    std::vector<EventTokens> event_tokens;
    
    while (std::getline(file, line)) {
        if (line.empty()) {
            throw std::invalid_argument(line);
        }
        
        std::istringstream iss(line);
        std::vector<std::string> tokens;
        std::string token;
        while (iss >> token) {
            tokens.push_back(token);
        }
        
        if (tokens.size() < 3 || tokens.size() > 4) {
            throw std::invalid_argument(line);
        }
        
        Time time;
        try {
            time = Time::from_string(tokens[0]);
        } catch (std::invalid_argument) {
            throw std::invalid_argument(line);
        }

        unsigned int event_id;
        try {
            event_id = std::stoi(tokens[1]);
        } catch (std::invalid_argument) {
            throw std::invalid_argument(line);
        }

        EventType event_type;
        if (EVENT_ID_TO_TYPE.find(event_id) == EVENT_ID_TO_TYPE.end()) {
            throw std::invalid_argument(line);
        }
        event_type = EVENT_ID_TO_TYPE[event_id];

        if (!validate_client_name(tokens[2])) {
            throw std::invalid_argument(line);
        }
        std::string name = tokens[2];

        switch (event_type) {
            case EventType::ClientArrived:
                if (tokens.size() != 3) {
                    throw std::invalid_argument(line);
                }
                event_tokens.push_back(ClientArrivedToken{time, name});
                break;

            case EventType::ClientWaiting:
                if (tokens.size() != 3) {
                    throw std::invalid_argument(line);
                }
                event_tokens.push_back(ClientWaitingToken{time, name});
                break;

            case EventType::ClientLeft:
                if (tokens.size() != 3) {
                    throw std::invalid_argument(line);
                }
                event_tokens.push_back(ClientLeftToken{time, name});
                break;

            case EventType::ClientSatDown:
                if (tokens.size() != 4) {
                    throw std::invalid_argument(line);
                }

                int place_num;
                try {
                    place_num = std::stoi(tokens[3]);
                } catch (std::invalid_argument) {
                    throw std::invalid_argument(line);
                }

                if (place_num < 1 || place_num > places_count) {
                    throw std::invalid_argument(line);
                }

                event_tokens.push_back(
                    ClientSatDownToken{time, name, static_cast<unsigned>(place_num)}
                );
                break;
        }
    }
    
    return event_tokens;
}