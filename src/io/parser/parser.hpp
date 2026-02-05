#pragma once

#include <string>
#include <vector>
#include <memory>
#include <variant>

#include "core/club/club.hpp"


struct ClientArrivedToken {
    Time time;
    std::string name;
};

struct ClientWaitingToken {
    Time time;
    std::string name;
};

struct ClientLeftToken {
    Time time;
    std::string name;
};

struct ClientSatDownToken {
    Time time;
    std::string name;
    unsigned int place;
};

using EventTokens = std::variant<
    ClientArrivedToken,
    ClientWaitingToken,
    ClientLeftToken,
    ClientSatDownToken
>;

class Parser {
public:
    static Club parse_file(const std::string& filename, std::vector<std::unique_ptr<Event>>& out_events);
private:
    using ConfigTokens = std::tuple<unsigned int, Time, Time, unsigned int>;

    static bool validate_client_name(const std::string& name);

    static ConfigTokens parse_header(std::ifstream& file);
    static std::vector<EventTokens> parse_events(std::ifstream& file, 
        unsigned int places_count, std::vector<std::unique_ptr<Event>>& out_events);
};