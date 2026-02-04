#pragma once

#include <string>
#include <vector>
#include <memory>

#include "../../core/club/club.hpp"

class Parser {
public:
    static Club parse_file(const std::string& filename, std::vector<std::unique_ptr<Event>>& events);
};