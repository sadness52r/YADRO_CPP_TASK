#pragma once

#include <string>
#include <memory>

#include "events/event.hpp"
#include "io/parser/parser.hpp"


template<class... Ts> 
struct Overload : Ts... { 
    using Ts::operator()...; 
};

template<class... Ts> 
Overload(Ts...) -> Overload<Ts...>;

class EventFactory {
public:
    static std::unique_ptr<Event> get_event(const EventTokens& tokens);
};