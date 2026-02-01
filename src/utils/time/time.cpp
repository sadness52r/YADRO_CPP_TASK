#include "time.hpp"
#include <sstream>
#include <iomanip>
#include <regex>

Time::Time(const std::string& time_str) {
    if (!std::regex_match(time_str, std::regex("^\\d{2}:\\d{2}$"))) {
        throw std::invalid_argument("Invalid time format");
    }

    int hours = std::stoi(time_str.substr(0, 2));
    int mins = std::stoi(time_str.substr(3, 2));
    if (hours < 0 || hours > 23 || mins < 0 || mins > 59) {
        throw std::invalid_argument("Invalid time value");
    }

    minutes = hours * 60 + mins;
}

Time Time::from_string(const std::string& time_str) {
    return Time(time_str);
}

std::string Time::to_string() const {
    int hours = minutes / 60;
    int mins = minutes % 60;

    std::ostringstream oss;
    oss << std::setw(2) << std::setfill('0') << hours << ":"
        << std::setw(2) << std::setfill('0') << mins;

    return oss.str();
}

bool Time::operator<(const Time& other) const {
    return minutes < other.minutes;
}

bool Time::operator<=(const Time& other) const {
    return minutes <= other.minutes;
}

bool Time::operator>(const Time& other) const {
    return minutes > other.minutes;
}

bool Time::operator>=(const Time& other) const {
    return minutes >= other.minutes;
}

bool Time::operator==(const Time& other) const {
    return minutes == other.minutes;
}

Time Time::operator+(const int _minutes) const {
    return Time(minutes + _minutes);
}

Time Time::operator-(const int _minutes) const {
    return Time(minutes - _minutes);
}

int Time::diff_minutes(const Time& other) const {
    return minutes - other.minutes;
}

int Time::get_minutes() const {
    return minutes;
}