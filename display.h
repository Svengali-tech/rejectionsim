#pragma once

#include <iostream>
#include <iomanip>
#include <string>
#include "colors.h"
#include "player.h"
#include "utils.h"
#include "skills.h"

// Returns the ASCII face for the player character based on current hope level.
// Displayed in the HUD so the player has a visual emotional anchor.
inline std::string getAsciiCharacter(int hope) {
    if (hope >= 80) return "(^_^)";   // thriving
    if (hope >= 60) return "(-_-)";   // fine, I guess
    if (hope >= 40) return "(._.)";   // concerned
    if (hope >= 20) return "(;_;)";   // hurting
    return "(x_x)";                   // the market won
}

// Prints the hope bar with a color gradient and the ASCII character alongside.
inline void printHopeBar(int hope) {
    int filled = hope / 5;
    int empty  = 20 - filled;
    const char* barColor = (hope > 60) ? GREEN : (hope > 30) ? YELLOW : RED;

    std::cout << "Hope:  [" << barColor
              << std::string(filled, '#') << RESET
              << std::string(empty, '.')
              << "] " << std::setw(3) << hope << "%"
              << "  " << getAsciiCharacter(hope) << "\n";
}

// Prints the energy bar. Energy is spent applying and networking each day.
inline void printEnergyBar(int energy, int maxEnergy) {
    std::cout << "Energy:[";
    for (int i = 0; i < maxEnergy; i++) {
        if (i < energy) std::cout << CYAN << "#" << RESET;
        else            std::cout << ".";
    }
    std::cout << "] " << energy << "/" << maxEnergy << "\n";
}

// printHUD is defined in main.cpp (requires full Player type)

// Prints the command menu. Called once at startup and after [h]elp.
inline void printHelp() {
    std::cout << "\n";
    std::cout << BOLD << "  Commands:\n" << RESET;
    std::cout << "  [a]           apply        -- send an application (costs 1 energy)\n";
    std::cout << "  [n]           network      -- build connections (costs 2 energy)\n";
    std::cout << "  [t leet]      train         -- grind leetcode, improves technical round\n";
    std::cout << "  [t portfolio] train         -- work on projects, improves ATS + phone\n";
    std::cout << "  [t clout]     train         -- build presence, improves final round\n";
    std::cout << "  [i]           prep          -- interview prep, one-time +20% (costs 2)\n";
    std::cout << "  [s]           sleep         -- end the day, refill energy\n";
    std::cout << "  [c]           cover letter  -- change cover letter strategy\n";
    std::cout << "  [x]           switch track  -- SWE / Finance / Consulting\n";
    std::cout << "  [v]           view board    -- job tracking board\n";
    std::cout << "  [p]           profile       -- full stats and skills\n";
    std::cout << "  [z]           achievements  -- view unlocked achievements\n";
    std::cout << "  [l]           leaderboard   -- local high scores\n";
    std::cout << "  [vent]        vent          -- +5 hope, no judgment\n";
    std::cout << "  [h]           help          -- show this menu\n";
    std::cout << "  [q]           quit          -- save and exit\n";
    std::cout << "  [r]           reset         -- wipe save and start over\n";
    std::cout << "\n";
}

// Prints a random LinkedIn-style notification popup.
// Called occasionally (15% chance) after any action to add flavor.
inline void maybeLinkedInPing() {
    if (randDouble() > 0.15) return;

    static const std::vector<std::string> pings = {
        "  [LinkedIn] A recruiter viewed your profile and did nothing.",
        "  [LinkedIn] Someone endorsed you for 'JavaScript'. You don't list JavaScript.",
        "  [LinkedIn] You appeared in 3 searches this week. 0 reached out.",
        "  [LinkedIn] A hiring manager viewed your profile for 11 seconds.",
        "  [LinkedIn] Congratulate John on his new role at the company that rejected you.",
        "  [LinkedIn] Your connection got promoted. You've applied to 40 jobs this month.",
        "  [LinkedIn] 'Open to Work' frame is now visible to all members.",
        "  [LinkedIn] Someone you don't know viewed your profile from Palo Alto.",
        "  [LinkedIn] Your post got 4 impressions. One was you.",
        "  [LinkedIn] A recruiter sent you InMail. It's about a role in Tulsa.",
    };

    std::cout << "\n" << DIM << pickRandom(pings) << RESET << "\n";
}

// Prints the color-coded inbox tag for an application outcome.
// Each outcome type has a distinct color so the inbox feels scannable.
inline void printInboxTag(const std::string& label, const char* color) {
    std::cout << color << BOLD << "[" << label << "]" << RESET << "\n";
}

// Prints the full-screen offer banner.
inline void printOfferBanner(const std::string& co, const std::string& role) {
    std::cout << "\n";
    std::cout << GREEN << BOLD
              << "  +--------------------------------------------------+\n"
              << "  |                  OFFER RECEIVED                  |\n"
              << "  +--------------------------------------------------+\n" << RESET;
    std::cout << "\n  " << BOLD << co << " -- " << role << RESET << "\n\n";
}

// printFinalSummary is defined in main.cpp
