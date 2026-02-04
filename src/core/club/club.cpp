#include <iostream>
#include <iomanip>
#include <algorithm>
#include <sstream>
#include <stdexcept>

#include "club.hpp"
#include "../../events/incoming/incoming_events.hpp"


Club::Club(const unsigned int _places_count, const Time _open_time, const Time _close_time, 
                const unsigned int price_per_hour)
    : places_count(_places_count), open_time(_open_time), close_time(_close_time),
      price_per_hour(price_per_hour), waiting_queue(_places_count) {
    places.reserve(places_count);
    for (int i = 1; i <= places_count; ++i) {
        places.emplace_back(i);
    }
}

const bool Club::is_open(const Time& time) const {
    return time >= open_time && time <= close_time;
}

const bool Club::is_client_inside(const std::string& name) const {
    return clients.find(name) != clients.end();
}

const bool Club::is_client_seated(const std::string& name) const {
    auto it = clients.find(name);
    return it != clients.end() && it->second.get_place_num().has_value();
}

const bool Club::is_place_free(const unsigned int place_num) const {
    if (place_num < 1 || place_num > places_count) return false;
    return !places[place_num - 1].is_occupied();
}

const unsigned int Club::get_free_place() const {
    for (int i = 0; i < places_count; ++i) {
        if (!places[i].is_occupied()) {
            return places[i].get_num();
        }
    }
    return -1;
}

void Club::process_event(std::unique_ptr<Event> event) {
    Time event_time = event->get_time();

    Event* evt_ptr = event.get();
    add_to_event_log(std::move(event));

    try {
        evt_ptr->execute(*this);
    } catch (const std::runtime_error& e) {
        auto error_event = Event::create_error_event(event_time, e.what());
        add_to_event_log(std::move(error_event));
    }
}

void Club::process_all_events(std::vector<std::unique_ptr<Event>>&& events) {
    event_log.clear();
    clients.clear();

    for (auto& event : events) {
        process_event(std::move(event));
    }

    finalize_day();
}

void Club::finalize_day() {
    std::vector<std::string> remaining_clients;
    
    for (const auto& place : places) {
        if (place.is_occupied()) {
            remaining_clients.push_back(*place.get_сlient());
        }
    }
    
    auto waiting = waiting_queue.get_all();
    remaining_clients.insert(remaining_clients.end(), waiting.begin(), waiting.end());
    
    std::sort(remaining_clients.begin(), remaining_clients.end());
    
    for (const auto& client_name : remaining_clients) {
        auto event = Event::create_client_forced_left_event(close_time, client_name);
        event->execute(*this);
        add_to_event_log(std::move(event));
    }
}

void Club::handle_client_arrived(const std::string& name, const Time& time) {
    if (!is_open(time)) {
        throw std::runtime_error("NotOpenYet");
    }
    if (is_client_inside(name)) {
        throw std::runtime_error("YouShallNotPass");
    }
    
    clients[name] = Client(name);
}

void Club::handle_client_sat_down(const std::string& name, 
        const unsigned int place_num, const Time& time) {
    if (!is_client_inside(name)) {
        throw std::runtime_error("ClientUnknown");
    }

    if (place_num < 1 || place_num > places_count) {
        throw std::runtime_error("PlaceIsBusy");
    }

    if (!is_place_free(place_num)) {
        throw std::runtime_error("PlaceIsBusy");
    }

    if (is_client_seated(name)) {
        int current_place = *clients[name].get_place_num();
        places[current_place - 1].release(time, price_per_hour);
    }
    
    places[place_num - 1].occupy(name, time);
    clients[name].set_place(place_num, time);
}

void Club::handle_client_waiting(const std::string& name, const Time& time) {
    if (!is_client_inside(name)) {
        throw std::runtime_error("ClientUnknown");
    }

    if (get_free_place() != -1) {
        throw std::runtime_error("ICanWaitNoLonger!");
    }

    if (waiting_queue.is_full()) {
        auto forced_left = Event::create_client_forced_left_event(time, name);
        forced_left->execute(*this);
        add_to_event_log(std::move(forced_left));
        return;
    }

    waiting_queue.add(name);
}

void Club::handle_client_left(const std::string& name, const Time& time) {
    if (!is_client_inside(name)) {
        throw std::runtime_error("ClientUnknown");
    }
    
    std::optional<unsigned int> place_num = std::nullopt;
    if (is_client_seated(name)) {
        place_num = *clients[name].get_place_num();
        places[place_num.value() - 1].release(time, price_per_hour);
        
    }

    clients.erase(name);
    
    seat_client_from_queue(time, place_num);
}

void Club::handle_client_forced_left(const std::string& name, const Time& time) {
    places[clients[name].get_place_num().value() - 1].release(time, price_per_hour);
    clients.erase(name);
}

void Club::handle_client_seated_from_queue(const std::string& name, 
    const unsigned int place_number, const Time& time) {
    if (!is_client_inside(name)) {
        return;
    }
    
    places[place_number - 1].occupy(name, time);
    clients[name].set_place(place_number, time);
}

void Club::seat_client_from_queue(const Time& time, 
    const std::optional<unsigned int> place_num) {
        
    if (waiting_queue.is_empty() || !place_num.has_value() ||
        !is_place_free(place_num.value()))
            return;
    
    auto next_client = waiting_queue.pop();
    if (!next_client) return;
    
    auto seated_event = Event::create_client_seated_event(time, *next_client, place_num.value());
    seated_event->execute(*this);
    add_to_event_log(std::move(seated_event));
}

void Club::add_to_event_log(std::unique_ptr<Event> event) {
    event_log.push_back(std::move(event));
}

std::vector<std::string> Club::generate_output() const {
    std::vector<std::string> output;
    
    output.push_back(open_time.to_string());
    
    for (const auto& event : event_log) {
        output.push_back(event->to_string());
    }
    
    output.push_back(close_time.to_string());
    
    for (const auto& place : places) {
        std::ostringstream oss;
        oss << place.get_num() << " " << place.get_income() << " ";
        int hours = place.get_total_minutes() / 60;
        int mins = place.get_total_minutes() % 60;
        oss << std::setw(2) << std::setfill('0') << hours << ":"
            << std::setw(2) << std::setfill('0') << mins;
        output.push_back(oss.str());
    }
    
    return output;
}