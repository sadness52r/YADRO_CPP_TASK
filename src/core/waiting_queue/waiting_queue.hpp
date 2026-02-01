#pragma once

#include <queue>
#include <string>
#include <vector>
#include <optional>

class WaitingQueue {
private:
    std::queue<std::string> queue;
    size_t max_size;

public:
    WaitingQueue(const size_t max_size) : max_size(max_size) {}

    const bool is_full() const;
    const bool is_empty() const;

    void add(const std::string& client_name);
    std::optional<std::string> pop();

    const std::vector<std::string> get_all_sorted() const;

    const size_t size() const;
};