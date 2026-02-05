#pragma once

#include <string>
#include <optional>

#include "utils/time/time.hpp"


class Client {
public:
    Client() = default;
    Client(const std::string& _name) 
        : name(_name), place_num(std::nullopt) {}

    const std::string get_name() const;
    const std::optional<unsigned int> get_place_num() const;
    const Time get_seated_time() const;

    void set_place(const unsigned int _place_num, const Time& _seatead_time);
private:
    std::string name;
    std::optional<unsigned int> place_num;
    Time seated_time;
};