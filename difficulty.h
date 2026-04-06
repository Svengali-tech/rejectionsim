#pragma once

#include <iostream>
#include <string>
#include "colors.h"
#include "utils.h"

// Difficulty presets. Selected at game start, saved to file.
enum class Difficulty {
    Easy      = 0,
    Normal    = 1,
    Hard      = 2,
    Nightmare = 3
};

inline std::string difficultyName(Difficulty d) {
    switch (d) {
        case Difficulty::Easy:      return "Easy";
        case Difficulty::Normal:    return "Normal";
        case Difficulty::Hard:      return "Hard";
        case Difficulty::Nightmare: return "Nightmare";
    }
    return "Normal";
}

inline const char* difficultyColor(Difficulty d) {
    switch (d) {
        case Difficulty::Easy:      return GREEN;
        case Difficulty::Normal:    return CYAN;
        case Difficulty::Hard:      return YELLOW;
        case Difficulty::Nightmare: return BRED;
    }
    return RESET;
}

// Per-difficulty config. All modifiers are additive on top of base pass rates.
struct DifficultyConfig {
    int  passRateMod;          // additive % to all pass rates
    int  atsRate;              // base ATS pass rate (replaces default 80)
    int  ghostChance;          // % of apps that ghost (replaces default 35%)
    int  referralThreshold;    // connections needed for referral
    int  faangPrestigeReq;     // prestige required to unlock FAANG
    bool bullMarketBias;       // if true, season roll favors bull/normal
    bool bearMarketBias;       // if true, season roll favors bear/freeze
    std::string description;
};

inline DifficultyConfig getDifficultyConfig(Difficulty d) {
    switch (d) {
        case Difficulty::Easy:
            return {
                +15, 90, 20, 3, 1, true, false,
                "Lenient ATS, bull market bias, referrals easier. For the vibes."
            };
        case Difficulty::Normal:
            return {
                0, 80, 35, 5, 2, false, false,
                "Standard rates. The default experience."
            };
        case Difficulty::Hard:
            return {
                -15, 65, 50, 8, 3, false, true,
                "Bear market bias, tough ATS, ghosting up. Closer to reality."
            };
        case Difficulty::Nightmare:
            return {
                -30, 50, 60, 10, 4, false, true,
                "ATS 50%, ghosting 60%, referrals need 10 connects, FAANG at prestige 4. GL."
            };
    }
    return getDifficultyConfig(Difficulty::Normal);
}

// Shows the difficulty selection screen and returns the chosen difficulty.
Difficulty runDifficultySelect() {
    std::cout << "\n" << BOLD << "  -- Select Difficulty --\n\n" << RESET;
    std::cout << GREEN  << "  [1] Easy       " << RESET << "-- Lenient ATS, bull market bias, referrals easier.\n";
    std::cout << CYAN   << "  [2] Normal     " << RESET << "-- Standard rates. The default experience.\n";
    std::cout << YELLOW << "  [3] Hard       " << RESET << "-- Bear market bias, tough ATS, high ghost rate.\n";
    std::cout << BRED   << "  [4] Nightmare  " << RESET << "-- ATS 50%, ghosting 60%, referrals need 10 connects.\n\n";
    std::cout << "  Choice [1-4] > ";

    std::string input;
    std::cin >> input;

    if      (input == "1") return Difficulty::Easy;
    else if (input == "2") return Difficulty::Normal;
    else if (input == "3") return Difficulty::Hard;
    else if (input == "4") return Difficulty::Nightmare;

    std::cout << DIM << "  Defaulting to Normal.\n" << RESET;
    return Difficulty::Normal;
}
