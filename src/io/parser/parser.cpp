#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <vector>
#include <iterator>

#include "parser.hpp"
#include "../../utils/time/time.hpp"
#include "../../events/factory/event_factory.hpp"

Club Parser::parse_file(const std::string& filename, std::vector<std::unique_ptr<Event>>& events) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open file: " + filename);
    }
    
    std::string line;
    if (!std::getline(file, line)) {
        throw std::invalid_argument(line);
    }
    line.erase(0, line.find_first_not_of(" \t"));
    if (line.empty()) 
        throw std::invalid_argument("Empty places_count " + line);

    std::istringstream iss(line);
    int places_count;
    if (!(iss >> places_count)) throw std::invalid_argument("Invalid places_count");
    std::string extra;
    if (iss >> extra) throw std::invalid_argument("Extra data after places_count");

    if (places_count <= 0) throw std::invalid_argument("Places count <= 0 " + line);
    
    if (!std::getline(file, line)) {
        throw std::invalid_argument("Line " + line + ": missing work hours");
    }
    line.erase(0, line.find_first_not_of(" \t"));
    if (line.empty()) 
        throw std::invalid_argument("Empty time " + line);
    
    std::istringstream time_stream(line);
    std::string topen_str;
    if (!(time_stream >> topen_str)) throw std::invalid_argument("Invalid topen_str");
    std::string tclose_str;
    if (!(time_stream >> tclose_str)) throw std::invalid_argument("Invalid tclose_str");
    if (time_stream >> extra) throw std::invalid_argument("Extra data after time");


    Time open_time, close_time;
    try {
        open_time = Time::from_string(topen_str);
        close_time = Time::from_string(tclose_str);
    } catch (...) {
        throw std::invalid_argument("Invalid time format " + line);
    }
    if (open_time >= close_time) 
        throw std::invalid_argument("Open time >= close time " + line);
    
    if (!std::getline(file, line)) {
        throw std::invalid_argument("Line " + line + ": missing price per hour");
    }
    line.erase(0, line.find_first_not_of(" \t"));
    if (line.empty()) throw std::invalid_argument(line);

    std::istringstream iss2(line);
    int price_per_hour;
    if (!(iss2 >> price_per_hour)) throw std::invalid_argument("Invalid price_per_hour");
    if (iss2 >> extra) throw std::invalid_argument("Extra data after price_per_hour");

    if (price_per_hour <= 0) throw std::invalid_argument("Price <= 0 " + line);
    
    events.clear();
    while (std::getline(file, line)) {
        try {
            auto event = EventFactory::parse_line(line, 0);
            events.push_back(std::move(event));
        } catch (const std::invalid_argument& e) {
            throw std::invalid_argument(line);
        }
    }
    
    return Club(static_cast<unsigned int>(places_count), open_time, close_time,
                 static_cast<unsigned int>(price_per_hour));
}