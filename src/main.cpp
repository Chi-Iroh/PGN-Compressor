#include <iostream>
#include "../include/io.hpp"

int main(int argc, char* argv[]) {
    if (argc == 1) {
        std::cout << "Write your PGN :" << std::endl;
        std::cout << inputPGN(nullptr).value_or("Error !") << std::endl;
    }
    for (int i{ 1 }; i < argc; i++) {
        std::cout << "Reading PGN from file " << argv[i] << std::endl;
        std::cout << inputPGN(argv[i]).value_or("Error !") << std::endl;
    }
}