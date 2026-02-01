#include "place.hpp"

void Place::occupy(const std::string& client, const Time& time) {
    occupied = true;
    client_name = client;
    start_time = time;
}

void Place::release(const Time& time, unsigned int price_per_hour) {
    if (occupied) {
        int used_minutes = time.diff_minutes(start_time);
        total_minutes += used_minutes;
        int hours = (used_minutes + 59) / 60;
        income += hours * price_per_hour;
        occupied = false;
        client_name = std::nullopt;
    }
}

const unsigned int Place::get_num() const {
    return num;
}

const bool Place::is_occupied() const {
    return occupied;
}

const std::optional<std::string> Place::get_сlient() const {
    return client_name;
}

const unsigned int Place::get_total_minutes() const {
    return total_minutes;
}

const unsigned int Place::get_income() const {
    return income;
}