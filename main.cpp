#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>

// All game modules. Each header is self-contained and can be included independently.
#include "colors.h"
#include "utils.h"
#include "player.h"
#include "skills.h"
#include "companies.h"
#include "display.h"
#include "interviews.h"
#include "offer.h"
#include "networking.h"
#include "apply.h"
#include "saveload.h"

// Prints the title screen. Called once on startup.
void printTitle() {
    std::cout << "\n" << BOLD << CYAN
              << "  +-------------------------------------------+\n"
              << "  |      REJECTION SIMULATOR 3000 (C++ ed.)   |\n"
              << "  |           A game based on a true story.    |\n"
              << "  +-------------------------------------------+\n" << RESET << "\n";
}

// Attempts to load a saved game. Returns true if one was found.
// Gives the player the option to continue or start fresh.
bool handleSaveLoad(Player& p) {
    std::ifstream check(SAVE_FILE);
    if (!check.is_open()) return false;
    check.close();

    std::cout << "  Save file found. Continue? [y/n] > ";
    std::string input;
    std::cin >> input;
    if (input == "y" || input == "Y") {
        if (loadGame(p)) {
            std::cout << GREEN << "  Save loaded. Day " << p.day
                      << ", prestige " << p.prestige << ".\n\n" << RESET;
            return true;
        }
    }
    deleteSave();
    return false;
}

// Called when the player sleeps ([s]).
// Advances the day, refills energy, and optionally prints a season change.
void handleSleep(Player& p) {
    bool seasonChanged = p.advanceDay();

    std::cout << "\n" << DIM
              << "  Day " << p.day - 1 << " done. "
              << "Energy refilled.\n" << RESET;

    if (seasonChanged) {
        std::cout << "\n" << BYELLOW << BOLD
                  << "  -- Market Update --\n" << RESET;
        std::cout << "  " << seasonName(p.season) << "\n";

        // Add flavor based on which season we hit
        switch (p.season) {
            case Season::BullMarket:
                std::cout << DIM << "  Companies are hiring. "
                          << "VCs are throwing money at everything.\n" << RESET;
                break;
            case Season::SlowMarket:
                std::cout << DIM << "  Budget reviews are in. "
                          << "Headcount approvals taking longer.\n" << RESET;
                break;
            case Season::BearMarket:
                std::cout << DIM << "  Layoff announcements trending on Twitter.\n"
                          << "  Everyone's posting 'excited to share my next chapter.'\n" << RESET;
                break;
            case Season::Freezing:
                std::cout << DIM << "  Hiring freeze memos going out.\n"
                          << "  Roles being pulled mid-process.\n" << RESET;
                break;
            default:
                break;
        }
    }

    std::cout << "\n  Day " << p.day << " begins.\n\n";
}

// Prints the full profile view ([p] command).
// More detail than the HUD -- includes everything.
void handleProfile(const Player& p) {
    std::cout << "\n" << BOLD << "  -- Profile --\n" << RESET;
    std::cout << "  Name:        You\n";
    std::cout << "  Tier:        " << tierName(p.tier) << "\n";
    std::cout << "  Prestige:    " << p.prestige << "\n";
    std::cout << "  Reputation:  " << p.reputation << " (" << p.reputationLabel() << ")\n";
    std::cout << "  Connections: " << p.connections << "\n";
    std::cout << "  Referral:    " << (p.hasReferral ? "Active" : "None") << "\n";
    std::cout << "  Season:      " << seasonName(p.season) << "\n";
    std::cout << "\n";
    std::cout << "  Applied:     " << p.applied    << "\n";
    std::cout << "  ATS fails:   " << p.atsFailed  << "\n";
    std::cout << "  Rejected:    " << p.rejected   << "\n";
    std::cout << "  Ghosted:     " << p.ghosted    << "\n";
    std::cout << "  Interviews:  " << p.interviews << "\n";
    std::cout << "  Offers:      " << p.offers     << "\n";
    std::cout << "  Networked:   " << p.networked  << "\n";
    std::cout << "\n";

    // Conversion rates (guard div by zero)
    if (p.applied > 0) {
        float interviewRate = 100.0f * p.interviews / p.applied;
        float offerRate     = 100.0f * p.offers     / p.applied;
        std::cout << "  Interview rate: " << std::fixed << std::setprecision(1)
                  << interviewRate << "%\n";
        std::cout << "  Offer rate:     " << offerRate << "%\n";
    }

    // Full skill breakdown with XP bars
    std::cout << "\n" << BOLD << "  -- Skills --\n" << RESET;
    std::cout << DIM
              << "  Leetcode Grind   -> bonus to technical round pass rate\n"
              << "  Portfolio        -> bonus to ATS pass rate + phone screen\n"
              << "  Network Clout    -> bonus to final round + faster referrals\n\n"
              << RESET;
    printSkillRow("Leetcode Grind",     p.skills.leetcodeLevel,  p.skills.leetcodeXP);
    printSkillRow("Portfolio Strength", p.skills.portfolioLevel, p.skills.portfolioXP);
    printSkillRow("Network Clout",      p.skills.cloutLevel,     p.skills.cloutXP);

    // Show what the current bonuses actually translate to in pass rates
    std::cout << "\n" << DIM
              << "  Current pass rate bonuses:\n"
              << "    Technical round:  +" << skillBonus(p.skills.leetcodeLevel)  << "%\n"
              << "    ATS + Phone:      +" << skillBonus(p.skills.portfolioLevel) << "%\n"
              << "    Final round:      +" << skillBonus(p.skills.cloutLevel)     << "%\n"
              << RESET;
    std::cout << "\n";
}

// Main game loop.
int main() {
    srand(static_cast<unsigned>(time(nullptr)));

    printTitle();

    Player p;

    // Try to resume a saved run
    bool loaded = handleSaveLoad(p);
    if (!loaded) {
        std::cout << "  Starting a new run.\n";
        printHelp();
    }

    std::string input;

    while (true) {
        printHUD(p);
        std::cout << "> ";
        std::cin >> input;

        // ---- APPLY ----
        if (input == "a" || input == "apply") {
            runApply(p);
            saveGame(p);   // auto-save after every action

        // ---- TRAIN ----
        // Subcommand comes as a second word: "t leet", "t portfolio", "t clout"
        } else if (input == "t" || input == "train") {
            std::string sub;
            std::cin >> sub;

            // Training costs 1 energy regardless of which skill
            if (p.energy < 1) {
                std::cout << RED << "  No energy. Sleep first ([s]).\n" << RESET;
            } else {
                p.energy--;
                runTraining(p.skills, sub);
                saveGame(p);
            }

        // ---- NETWORK ----
        } else if (input == "n" || input == "network") {
            runNetworking(p);
            saveGame(p);

        // ---- SLEEP / END DAY ----
        } else if (input == "s" || input == "sleep") {
            handleSleep(p);
            saveGame(p);

        // ---- PROFILE ----
        } else if (input == "p" || input == "profile") {
            handleProfile(p);

        // ---- LEADERBOARD ----
        } else if (input == "l" || input == "leaderboard") {
            printLeaderboard();

        // ---- HELP ----
        } else if (input == "h" || input == "help") {
            printHelp();

        // ---- QUIT ----
        } else if (input == "q" || input == "quit") {
            submitToLeaderboard(p);
            saveGame(p);
            printFinalSummary(p);
            std::cout << DIM << "  Progress saved. Run again to continue.\n\n" << RESET;
            break;

        // ---- RESET ----
        } else if (input == "r" || input == "reset") {
            std::cout << "\n  This will wipe your save. Are you sure? [y/n] > ";
            std::string confirm;
            std::cin >> confirm;
            if (confirm == "y" || confirm == "Y") {
                submitToLeaderboard(p);   // submit the run before wiping
                deleteSave();
                p = Player();             // reset to defaults
                std::cout << GREEN << "\n  Reset. Same market. New you.\n\n" << RESET;
                printHelp();
            }

        } else {
            std::cout << RED << "  Unknown command. [h] for help.\n" << RESET;
        }
    }

    return 0;
}
