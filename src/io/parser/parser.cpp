#include <fstream>
#include <sstream>
#include <stdexcept>
#include <vector>

#include "parser.hpp"
#include "../../utils/time/time.hpp"
#include "../../events/factory/event_factory.hpp"

Club Parser::parse_file(const std::string& filename, std::vector<std::unique_ptr<Event>>& events) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open file: " + filename);
    }
    
    std::string line;
    int line_number = 0;
    
    if (!std::getline(file, line)) {
        throw std::runtime_error("File too short: missing places count");
    }
    line_number++;
    line.erase(0, line.find_first_not_of(" \t"));
    if (line.empty()) throw std::runtime_error(line);

    unsigned int places_count;
    try {
        places_count = std::stoi(line);
    }
    catch (...) {
        throw std::invalid_argument(line);
    }
    if (places_count <= 0) throw std::invalid_argument(line);
    
    if (!std::getline(file, line)) {
        throw std::runtime_error("Line " + std::to_string(line_number + 1) + ": missing work hours");
    }
    line_number++;
    line.erase(0, line.find_first_not_of(" \t"));
    if (line.empty()) throw std::runtime_error(line);
    
    std::istringstream time_stream(line);
    std::string topen_str, tclose_str;
    if (!(time_stream >> topen_str >> tclose_str)) { // проверить
        throw std::runtime_error(line);
    }
    Time open_time, close_time;
    try {
        open_time = Time::from_string(topen_str);
        close_time = Time::from_string(tclose_str);
    } catch (...) {
        throw std::invalid_argument(line);
    }
    if (open_time >= close_time) throw std::invalid_argument(line);
    
    if (!std::getline(file, line)) {
        throw std::runtime_error("Line " + std::to_string(line_number + 1) + ": missing price per hour");
    }
    line_number++;
    line.erase(0, line.find_first_not_of(" \t"));
    if (line.empty()) throw std::runtime_error(line);
    unsigned int price_per_hour;
    try {
        price_per_hour = std::stoi(line);
    }
    catch (...) {
        throw std::invalid_argument(line);
    }
    if (price_per_hour <= 0) throw std::invalid_argument(line);
    
    events.clear();
    while (std::getline(file, line)) {
        line_number++;
        
        try {
            auto event = EventFactory::parse_line(line, line_number);
            events.push_back(std::move(event));
        } catch (const std::invalid_argument& e) {
            throw std::runtime_error(line);
        }
    }
    
    return Club(places_count, open_time, close_time, price_per_hour);
}