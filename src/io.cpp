#include <algorithm>
#include <iostream>
#include <fstream>
#include "../include/io.hpp"

static std::optional<std::string> inputPGNfromStream(std::istream& stream) {
    std::string PGN{};
    std::string elem{};

    while (stream >> elem) {
        PGN += std::move(elem);
    }
    return PGN.empty() ? std::nullopt : std::optional{ PGN };
}

static std::optional<std::string> inputPGNfromFile(char* filepath) {
    std::ifstream file{ filepath };

    if (!file) {
        return std::nullopt;
    }
    return inputPGNfromStream(file);
}

std::optional<std::string> inputPGN(char* filepath) {
    return filepath ? inputPGNfromFile(filepath) : inputPGNfromStream(std::cin);
}