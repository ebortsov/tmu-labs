#include <iostream>

int main(int argc, char* argv[]) {
    std::cout << "Hello";
    if (argc == 1) {
        std::cout << " World";
    } else {
        for (int i = 0; i < argc - 1; i++) {
            std::cout << " " << argv[i + 1];
        }
    }
    std::cout << "!\n";
}
