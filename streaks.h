#pragma once

#include <iostream>
#include <string>
#include "colors.h"
#include "utils.h"

// Tracks consecutive rejections (including ghosts) and fires commentary
// at escalating thresholds. Resets when an interview pipeline is entered.

struct RejectionStreak {
    int current  = 0;   // current consecutive rejection/ghost count
    int allTime  = 0;   // highest streak ever reached this run

    void onRejection() {
        current++;
        if (current > allTime) allTime = current;
    }

    // Call when entering an interview pipeline -- streak breaks on any forward progress
    void onProgress() { current = 0; }

    // Call on a full offer -- full reset, print something
    void onOffer() { current = 0; }
};

// Prints streak commentary based on current count.
// Called after every rejection/ghost in apply.h.
// Nothing prints below 5 -- the early ones don't need commentary.
void printStreakCommentary(int streak) {
    if      (streak == 5)  {
        std::cout << DIM << "\n  (5 in a row. Bad luck.)\n" << RESET;
    }
    else if (streak == 10) {
        std::cout << YELLOW << DIM << "\n  (10 straight. The pipeline is telling you something.)\n" << RESET;
    }
    else if (streak == 15) {
        std::cout << YELLOW << "\n  (15 consecutive. Statistically unlikely. Are you okay?)\n" << RESET;
    }
    else if (streak == 20) {
        std::cout << RED << "\n  (20 in a row. This is the game being honest with you.)\n" << RESET;
    }
    else if (streak == 25) {
        std::cout << RED << BOLD << "\n  (25. The market is not broken. This is just what it is.)\n" << RESET;
    }
    else if (streak == 30) {
        std::cout << "\n" << RED << BOLD << "  .\n" << RESET;
    }
    else if (streak > 30 && streak % 10 == 0) {
        // Every 10 after 30, just a period. Nothing else.
        std::cout << "\n" << DIM << "  .\n" << RESET;
    }
    else if (streak == 50) {
        std::cout << "\n" << BRED << BOLD
                  << "  (50. You are the main character. This is the part before the turnaround.)\n"
                  << RESET;
    }
}
