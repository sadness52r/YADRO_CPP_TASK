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
        
        return 0;
    } catch (const std::runtime_error& e) {
        // Ошибка формата: выводим первую некорректную строку
        std::cout << e.what() << std::endl;
        return 1;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}
