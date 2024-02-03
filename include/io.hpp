#pragma once
#include <string>
#include <optional>

/**
 * @brief Inputs PGN, either from a given filepath or the standard input.
 * @param filepath if nullptr, reads from standard input, otherwise reads from the file
 * @note All spaces are removed on the returned string
 * @returns std::nullopt if input is empty (or only spaces, as they're removed)
*/
std::optional<std::string> inputPGN(char* filepath);