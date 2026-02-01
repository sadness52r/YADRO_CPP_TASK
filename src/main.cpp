#include <iostream>

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Usage: task <input_file>\n";
        return 1;
    }

    std::cout << __cplusplus << std::endl;
    return 0;
}
