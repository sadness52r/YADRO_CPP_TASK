#pragma once

#include <string>

struct Time {
private:
    int minutes;

public:
    Time() : minutes(0) {}
    Time(int minutes) : minutes(minutes) {}
    Time(const std::string& time_str);

    static Time from_string(const std::string& time_str);
    std::string to_string() const;

    bool operator<(const Time& other) const;
    bool operator<=(const Time& other) const;
    bool operator>(const Time& other) const;
    bool operator>=(const Time& other) const;
    bool operator==(const Time& other) const;
    bool operator!=(const Time& other) const;

    Time operator+(const int _minutes) const;
    Time operator-(const int _minutes) const;
    
    int diff_minutes(const Time& other) const;

    int get_minutes() const;
};