#pragma once

#include <optional>
#include <string>

#include "../../utils/time/time.hpp"


struct Place {
public:
    Place(int num) : num(num), occupied(false), total_minutes(0), income(0) {}

    void occupy(const std::string& client, const Time& time);
    void release(const Time& time, unsigned int price_per_hour);

    const unsigned int get_num() const;
    const bool is_occupied() const;
    const std::optional<std::string> get_сlient() const;
    const unsigned int get_total_minutes() const;
    const unsigned int get_income() const;
private:
    unsigned int num;
    bool occupied;
    std::optional<std::string> client_name;
    Time start_time;
    unsigned int total_minutes;
    unsigned int income;
};