#include "client.hpp"

const std::string Client::get_name() const {
    return name;
}

const std::optional<unsigned int> Client::get_place_num() const {
    return place_num;
}

const Time Client::get_seated_time() const {
    return seated_time;
}

void Client::set_place(const unsigned int _place_num, const Time& _seatead_time) {
    place_num = _place_num;
    seated_time = _seatead_time;
}