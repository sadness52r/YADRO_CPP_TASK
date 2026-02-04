#pragma once

#include <unordered_map>
#include <vector>
#include <string>
#include <memory>

#include "../client/client.hpp"
#include "../place/place.hpp"
#include "../waiting_queue/waiting_queue.hpp"
#include "../../events/event.hpp"
#include "../../utils/time/time.hpp"

class Club {
private:
    unsigned int places_count;
    Time open_time;
    Time close_time;
    unsigned int price_per_hour;

    std::unordered_map<std::string, Client> clients;
    std::vector<Place> places;
    WaitingQueue waiting_queue;
    std::vector<std::unique_ptr<Event>> event_log;

    void add_to_event_log(std::unique_ptr<Event> event);
public:
    Club(const unsigned int _places_count, const Time _open_time, const Time _close_time, 
        const unsigned int price_per_hour);

    void process_all_events(std::vector<std::unique_ptr<Event>>&& events);
    void process_event(std::unique_ptr<Event> event);
    void finalize_day();

    const bool is_open(const Time& time) const;
    const bool is_client_inside(const std::string& name) const;
    const bool is_client_seated(const std::string& name) const;
    const bool is_place_free(const unsigned int place_num) const;
    const unsigned int get_free_place() const;

    void seat_client_from_queue(const Time& time, std::optional<unsigned int> place_num);

    void handle_client_arrived(const std::string& name, const Time& time);
    void handle_client_sat_down(const std::string& name, const unsigned int place_num, 
        const Time& time);
    void handle_client_waiting(const std::string& name, const Time& time);
    void handle_client_left(const std::string& name, const Time& time);
    void handle_client_forced_left(const std::string& name, const Time& time);
    void handle_client_seated_from_queue(const std::string& name, const unsigned int place_num, const Time& time);

    std::vector<std::string> generate_output() const;
};