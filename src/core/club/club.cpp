#include <iostream>
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
    opened = false;
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
    opened = is_open(event_time);

    try {
        //event->execute(*this);
        add_to_event_log(std::move(event));
    } catch (const std::runtime_error& e) {
        //auto error_event = Event::create_error_event(event_time, e.what());
        add_to_event_log(std::move(event)); // оригинальное событие всё равно логируем
        //add_to_event_log(std::move(error_event));
    }
}

// void Club::process_all_events(std::vector<std::unique_ptr<Event>>&& events) {
//     event_log_.clear();
//     clients_.clear();

//     // Добавляем время открытия как первое событие в вывод
//     // (но не как обрабатываемое событие)
//     for (auto& event : events) {
//         process_event(std::move(event));
//     }

//     // Финализация дня
//     finalize_day();
// }

void Club::finalize_day() {
    std::vector<std::string> remaining_clients;
    
    for (const auto& place : places) {
        if (place.is_occupied()) {
            remaining_clients.push_back(*place.get_сlient());
        }
    }
    
    auto waiting = waiting_queue.get_all_sorted();
    remaining_clients.insert(remaining_clients.end(), waiting.begin(), waiting.end());
    
    std::sort(remaining_clients.begin(), remaining_clients.end());
    
    for (const auto& client_name : remaining_clients) {
        //auto event = Event::create_client_forced_left_event(close_time_, client_name);
        //event->execute(*this);
        //add_to_event_log(std::move(event));
        std::cout << "Сгенерировано событие 11, клуб закрывается\n";
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
        throw std::runtime_error("PlaceIsBusy"); // несуществующий стол = занят
    }

    if (!is_place_free(place_num)) {
        throw std::runtime_error("PlaceIsBusy");
    }

    // Разрешаем пересадку: если клиент уже сидит за другим столом - освободим его
    if (is_client_seated(name)) {
        int current_place = *clients[name].get_place_num();
        places[current_place - 1].release(time, price_per_hour);
    }
    
    // Садим клиента
    places[place_num - 1].occupy(name, time);
    clients[name].set_place(place_num, time);
}

void Club::handle_client_waiting(const std::string& name, const Time& time) {
    if (!is_client_inside(name)) {
        throw std::runtime_error("ClientUnknown");
    }
    // Проверяем наличие свободных столов
    if (get_free_place() != -1) {
        throw std::runtime_error("ICanWaitNoLonger!");
    }
    // Проверяем переполнение очереди
    if (waiting_queue.is_full()) {
        // Клиент уходит немедленно - генерируем событие 11
        //auto forced_left = Event::create_client_forced_left_event(time, name);
        //forced_left->execute(*this);
        //add_to_event_log(std::move(forced_left));
        std::cout << "Сгенерировано событие 11, очередь забита\n";
        return;
    }
    // Добавляем в очередь
    waiting_queue.add(name);
}

void Club::handle_client_left(const std::string& name, const Time& time) {
    if (!is_client_inside(name)) {
        throw std::runtime_error("ClientUnknown");
    }
    
    // Если клиент сидел за столом - освобождаем стол
    if (is_client_seated(name)) {
        int table_num = *clients_[name].table_number;
        tables_[table_num - 1].release(time, hourly_rate_);
    }
    
    // Удаляем клиента
    clients_.erase(name);
    
    // Пытаемся посадить следующего из очереди
    seat_client_from_queue(time);
}

void ComputerClub::handle_client_forced_left(const std::string& name, const Time& time) {
    // Удаляем из очереди (если там был)
    // Просто удаляем клиента - он покидает клуб
    clients_.erase(name);
}

void ComputerClub::handle_client_seated_from_queue(const std::string& name, int table_num, const Time& time) {
    // Этот метод вызывается только из внутренней логики при освобождении стола
    if (!is_client_inside(name)) {
        return; // клиент уже ушёл
    }
    
    // Садим клиента
    tables_[table_num - 1].occupy(name, time);
    clients_[name].status = ClientStatus::Seated;
    clients_[name].table_number = table_num;
    clients_[name].seated_time = time;
}

void ComputerClub::seat_client_from_queue(const Time& time) {
    if (waiting_queue_.is_empty()) return;
    
    // Ищем свободный стол
    int free_table = get_free_table();
    if (free_table == -1) return;
    
    // Берём первого из очереди
    auto next_client = waiting_queue_.pop();
    if (!next_client) return;
    
    // Генерируем исходящее событие 12
    auto seated_event = Event::create_client_seated_event(time, *next_client, free_table);
    seated_event->execute(*this);
    add_to_event_log(std::move(seated_event));
}

void ComputerClub::add_to_event_log(std::unique_ptr<Event> event) {
    event_log_.push_back(std::move(event));
}

std::vector<std::string> ComputerClub::generate_output() const {
    std::vector<std::string> output;
    
    // Первая строка - время открытия
    output.push_back(open_time_.to_string());
    
    // Все события в хронологическом порядке
    for (const auto& event : event_log_) {
        output.push_back(event->to_string());
    }
    
    // Время закрытия
    output.push_back(close_time_.to_string());
    
    // Статистика по столам
    for (const auto& table : tables_) {
        std::ostringstream oss;
        oss << table.number << " " << table.revenue << " ";
        // Формат времени: HH:MM
        int hours = table.total_minutes / 60;
        int mins = table.total_minutes % 60;
        oss << std::setw(2) << std::setfill('0') << hours << ":"
            << std::setw(2) << std::setfill('0') << mins;
        output.push_back(oss.str());
    }
    
    return output;
}