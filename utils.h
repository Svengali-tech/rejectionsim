#pragma once

#include <string>
#include <vector>
#include <cstdlib>
#include <thread>
#include <chrono>
#include <iostream>
#include "colors.h"

// Returns a random element from any string vector.
// Used everywhere we need weighted or random text responses.
inline std::string pickRandom(const std::vector<std::string>& v) {
    return v[rand() % v.size()];
}

// Blocks execution for N milliseconds.
// Used to add dramatic pauses between interview stages.
inline void sleepMs(int ms) {
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

// Prints a full-width horizontal rule in dim style.
inline void printSeparator() {
    std::cout << DIM << std::string(60, '-') << RESET << "\n";
}

// Prints a section header with a label, e.g. "[ PHONE SCREEN ]".
// Used to visually break up the interview pipeline stages.
inline void printHeader(const std::string& label, const char* color = CYAN) {
    std::cout << color << BOLD << "[ " << label << " ]" << RESET << "\n";
}

// Clamps an integer between lo and hi (inclusive).
inline int clamp(int val, int lo, int hi) {
    if (val < lo) return lo;
    if (val > hi) return hi;
    return val;
}

// Returns a random int in [lo, hi] inclusive.
inline int randRange(int lo, int hi) {
    return lo + rand() % (hi - lo + 1);
}

// Returns a random double in [0, 1).
inline double randDouble() {
    return (double)rand() / RAND_MAX;
}

// Prompts the user for a yes/no answer and returns true for 'y'.
inline bool askYesNo(const std::string& prompt) {
    std::cout << "  " << prompt << " [y/n] > ";
    std::string input;
    std::cin >> input;
    return (input == "y" || input == "Y");
}
