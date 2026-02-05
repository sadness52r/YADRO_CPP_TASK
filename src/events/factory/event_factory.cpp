#include <sstream>
#include <stdexcept>

#include "event_factory.hpp"
#include "events/incoming/incoming_events.hpp"


std::unique_ptr<Event>
EventFactory::get_event(const EventTokens& tokens)
{
    return std::visit(Overload{
        [](const ClientArrivedToken& t) -> std::unique_ptr<Event> {
            return std::make_unique<ClientArrivedEvent>(t.time, t.name);
        },
        [](const ClientWaitingToken& t) -> std::unique_ptr<Event> {
            return std::make_unique<ClientWaitingEvent>(t.time, t.name);
        },
        [](const ClientLeftToken& t) -> std::unique_ptr<Event> {
            return std::make_unique<ClientLeftEvent>(t.time, t.name);
        },
        [](const ClientSatDownToken& t) -> std::unique_ptr<Event> {
            return std::make_unique<ClientSatDownEvent>(
                t.time, t.name, t.place
            );
        }
    }, tokens);
}
