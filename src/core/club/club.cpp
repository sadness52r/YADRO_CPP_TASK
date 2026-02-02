#include <algorithm>
#include <sstream>
#include <stdexcept>

#include "club.hpp"
#include "../../events/incoming/incoming_events.hpp"
#include "events/OutgoingEvents.h"


Club::Club(const unsigned int _places_count, const Time _open_time, const Time _close_time, 
                const unsigned int price_per_hour)
    : places_count(_places_count), open_time(_open_time), close_time(_close_time),
      price_per_hour(price_per_hour), waiting_queue(_places_count) {
    places.reserve(places_count);
    for (int i = 1; i <= places_count; ++i) {
        places.emplace_back(i);
    }
    is_open = false;
}

bool Club::is_open(const Time& time) const {
    return time >= open_time && time <= close_time;
}

bool Club::is_client_seated(const std::string& name) const {
    auto it = clients.find(name);
    return it != clients.end() && it->second.get_status() == ClientStatus::Seated;
}

bool ComputerClub::is_table_free(int table_num) const {
    if (table_num < 1 || table_num > table_count_) return false;
    return !tables_[table_num - 1].is_occupied;
}

bool ComputerClub::can_wait() const {
    return waiting_queue_.can_add();
}

int ComputerClub::get_free_table() const {
    for (int i = 0; i < table_count_; ++i) {
        if (!tables_[i].is_occupied) {
            return tables_[i].number;
        }
    }
    return -1;
}

void ComputerClub::process_event(std::unique_ptr<Event> event) {
    Time event_time = event->get_time();
    is_open_ = is_open(event_time); // обновляем статус открытия

    try {
        event->execute(*this);
        add_to_event_log(std::move(event));
    } catch (const std::runtime_error& e) {
        // Генерация события ошибки 13
        auto error_event = Event::create_error_event(event_time, e.what());
        add_to_event_log(std::move(event)); // оригинальное событие всё равно логируем
        add_to_event_log(std::move(error_event));
    }
}

void ComputerClub::process_all_events(std::vector<std::unique_ptr<Event>>&& events) {
    event_log_.clear();
    clients_.clear();

    // Добавляем время открытия как первое событие в вывод
    // (но не как обрабатываемое событие)
    for (auto& event : events) {
        process_event(std::move(event));
    }

    // Финализация дня
    finalize_day();
}

void ComputerClub::finalize_day() {
    // Собираем всех клиентов, которые остались в клубе
    std::vector<std::string> remaining_clients;
    
    // Клиенты за столами
    for (const auto& table : tables_) {
        if (table.is_occupied && table.client_name) {
            remaining_clients.push_back(*table.client_name);
        }
    }
    
    // Клиенты в очереди
    auto waiting = waiting_queue_.get_all_sorted();
    remaining_clients.insert(remaining_clients.end(), waiting.begin(), waiting.end());
    
    // Сортируем по алфавиту
    std::sort(remaining_clients.begin(), remaining_clients.end());
    
    // Генерируем события 11 для каждого клиента
    for (const auto& client_name : remaining_clients) {
        auto event = Event::create_client_forced_left_event(close_time_, client_name);
        event->execute(*this);
        add_to_event_log(std::move(event));
    }
}

void ComputerClub::handle_client_arrived(const std::string& name, const Time& time) {
    if (!is_open(time)) {
        throw std::runtime_error("NotOpenYet");
    }
    if (is_client_inside(name)) {
        throw std::runtime_error("YouShallNotPass");
    }
    
    clients_[name] = Client(name);
    clients_[name].status = ClientStatus::Inside;
    clients_[name].arrival_time = time;
}

void ComputerClub::handle_client_sat_down(const std::string& name, int table_num, const Time& time) {
    if (!is_client_inside(name)) {
        throw std::runtime_error("ClientUnknown");
    }
    if (table_num < 1 || table_num > table_count_) {
        throw std::runtime_error("PlaceIsBusy"); // несуществующий стол = занят
    }
    // Разрешаем пересадку: если клиент уже сидит за другим столом - освободим его
    if (is_client_seated(name)) {
        int current_table = *clients_[name].table_number;
        tables_[current_table - 1].release(time, hourly_rate_);
    }
    
    if (!is_table_free(table_num)) {
        throw std::runtime_error("PlaceIsBusy");
    }
    
    // Садим клиента
    tables_[table_num - 1].occupy(name, time);
    clients_[name].status = ClientStatus::Seated;
    clients_[name].table_number = table_num;
    clients_[name].seated_time = time;
}

void ComputerClub::handle_client_waiting(const std::string& name, const Time& time) {
    if (!is_client_inside(name)) {
        throw std::runtime_error("ClientUnknown");
    }
    // Проверяем наличие свободных столов
    if (get_free_table() != -1) {
        throw std::runtime_error("ICanWaitNoLonger!");
    }
    // Проверяем переполнение очереди
    if (!can_wait()) {
        // Клиент уходит немедленно - генерируем событие 11
        auto forced_left = Event::create_client_forced_left_event(time, name);
        forced_left->execute(*this);
        add_to_event_log(std::move(forced_left));
        return;
    }
    // Добавляем в очередь
    waiting_queue_.add(name);
    clients_[name].status = ClientStatus::Inside; // остаётся внутри, но без стола
    clients_[name].table_number = std::nullopt;
}

void ComputerClub::handle_client_left(const std::string& name, const Time& time) {
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