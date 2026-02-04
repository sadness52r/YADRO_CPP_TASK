#include <iostream>
#include <vector>
#include <memory>

#include "io/parser/parser.hpp"

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <input_file>" << std::endl;
        return 1;
    }
    
    try {
        std::vector<std::unique_ptr<Event>> events;
        Club club = Parser::parse_file(argv[1], events);

        club.process_all_events(std::move(events));
        
        auto output = club.generate_output();
        for (const auto& line : output) {
            std::cout << line << std::endl;
        }
        
        return 0;
    } catch (const std::invalid_argument& e) {
        std::cout << e.what() << std::endl;
        return 1;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}
