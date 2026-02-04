#include <algorithm>

#include "waiting_queue.hpp"

const bool WaitingQueue::is_full() const { 
    return queue.size() == max_size; 
}

const bool WaitingQueue::is_empty() const {
     return queue.empty(); 
}

void WaitingQueue::add(const std::string& client_name) {
    if (!is_full()) {
        queue.push(client_name);
    }
}

std::optional<std::string> WaitingQueue::pop() {
    if (queue.empty()) return std::nullopt;
    std::string front = queue.front();
    queue.pop();
    return front;
}

const std::vector<std::string> WaitingQueue::get_all() const {
    std::vector<std::string> result;
    auto temp = queue;
    while (!temp.empty()) {
        result.push_back(temp.front());
        temp.pop();
    }
    return result;
}

const size_t WaitingQueue::size() const { 
    return queue.size();
}