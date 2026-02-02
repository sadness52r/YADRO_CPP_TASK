#pragma once

#include <string>
#include <memory>

#include "../event.hpp"


class EventFactory {
public:
    static std::unique_ptr<Event> parse_line(const std::string& line, int line_number);
    static bool validate_client_name(const std::string& name);
};