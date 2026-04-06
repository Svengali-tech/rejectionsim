#pragma once

#include <iostream>
#include <iomanip>
#include <string>
#include "colors.h"
#include "player.h"
#include "utils.h"

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

// Full HUD. Prints everything the player needs to see at a glance.
// Called at the top of each input loop iteration.
inline void printHUD(const Player& p) {
    std::cout << "\n";
    std::cout << BOLD << "  REJECTION SIMULATOR 3000" << RESET;
    std::cout << DIM << "  Day " << p.day << RESET;
    std::cout << "  [" << tierName(p.tier) << "]";
    if (p.prestige > 0)
        std::cout << BMAGENTA << "  Prestige " << p.prestige << RESET;
    std::cout << "\n";

    printSeparator();

    // Row 1: application stats
    std::cout << "Applied: "     << BOLD  << std::setw(4) << p.applied     << RESET
              << "  Rejected: "  << RED   << BOLD << std::setw(4) << p.rejected  << RESET
              << "  Ghosted: "   << DIM   << std::setw(4) << p.ghosted     << RESET
              << "  ATS: "       << DIM   << std::setw(3) << p.atsFailed   << RESET << "\n";

    // Row 2: pipeline stats
    std::cout << "Interviews: "  << YELLOW << BOLD << std::setw(3) << p.interviews << RESET
              << "  Offers: "    << GREEN  << BOLD << std::setw(4) << p.offers     << RESET
              << "  Networked: " << BCYAN  << std::setw(4) << p.networked          << RESET
              << "  Connects: "  << BCYAN  << std::setw(4) << p.connections        << RESET << "\n";

    // Row 3: reputation and referral status
    std::cout << "Reputation: "  << std::setw(3) << p.reputation
              << " (" << p.reputationLabel() << ")";
    if (p.hasReferral)
        std::cout << "  " << BGREEN << "[REFERRAL ACTIVE]" << RESET;
    std::cout << "\n";

    // Row 4: season
    std::cout << "Market: " << DIM << seasonName(p.season) << RESET << "\n";

    // Bars
    printHopeBar(p.hope);
    printEnergyBar(p.energy, p.maxEnergy);

    printSeparator();
}

// Prints the command menu. Called once at startup and after [h]elp.
inline void printHelp() {
    std::cout << "\n";
    std::cout << BOLD << "  Commands:\n" << RESET;
    std::cout << "  [a] apply       -- send an application (costs 1 energy)\n";
    std::cout << "  [n] network     -- build connections (costs 2 energy)\n";
    std::cout << "  [s] sleep       -- end the day, refill energy\n";
    std::cout << "  [p] profile     -- view full stats\n";
    std::cout << "  [l] leaderboard -- view local high scores\n";
    std::cout << "  [h] help        -- show this menu\n";
    std::cout << "  [q] quit        -- save and exit\n";
    std::cout << "  [r] reset       -- wipe save and start over\n";
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

// Prints game-over / final summary screen.
inline void printFinalSummary(const Player& p) {
    std::cout << "\n" << BOLD << "  -- Final Stats --\n" << RESET;
    std::cout << "  Days played:    " << p.day           << "\n";
    std::cout << "  Applied:        " << p.applied       << "\n";
    std::cout << "  ATS filtered:   " << p.atsFailed     << "\n";
    std::cout << "  Rejected:       " << p.rejected      << "\n";
    std::cout << "  Ghosted:        " << p.ghosted       << "\n";
    std::cout << "  Interviews:     " << p.interviews    << "\n";
    std::cout << "  Offers:         " << p.offers        << "\n";
    std::cout << "  Networked:      " << p.networked     << "\n";
    std::cout << "  Prestige:       " << p.prestige      << "\n";
    std::cout << "  Tier:           " << tierName(p.tier)<< "\n";
    std::cout << "  Reputation:     " << p.reputation << " (" << p.reputationLabel() << ")\n";
    std::cout << "  Hope remaining: " << p.hope          << "%\n";
    if (p.fastestOffer > 0)
        std::cout << "  Fastest offer:  Day " << p.fastestOffer << "\n";
    std::cout << "\n";

    if (p.offers > 0)
        std::cout << GREEN << BOLD << "  You got an offer. The grind was worth it. Maybe.\n\n" << RESET;
    else if (p.applied == 0)
        std::cout << "  You didn't apply once. Bold strategy.\n\n";
    else if (p.hope <= 0)
        std::cout << RED << "  Hope at zero. The market won. It usually does.\n\n" << RESET;
    else
        std::cout << "  Still got " << p.hope << "% hope. That's more than most.\n\n";
}
