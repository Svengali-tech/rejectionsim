#pragma once

#include <iostream>
#include <string>
#include <vector>
#include "colors.h"
#include "utils.h"

// Story mode gives the player a fixed starting scenario with a debt counter
// that adds financial stakes. Reaching debt = 0 is the win condition.
// The character name is set at game start and used throughout.

struct StoryState {
    bool        active      = false;
    std::string playerName  = "Player";
    int         debt        = 0;      // current debt in $k
    int         startDebt   = 0;      // initial debt (for progress display)
    int         income      = 0;      // monthly income once employed (reduces debt per day)
    bool        employed    = false;  // true after accepting an offer
    int         storyDay    = 0;      // story-specific day counter
};

// Fixed story scenarios. Each sets starting conditions and flavor.
struct StoryScenario {
    std::string title;
    std::string setup;        // opening narrative
    int         startDebt;   // in $k
    int         difficulty;  // maps to Difficulty enum
};

inline std::vector<StoryScenario> STORY_SCENARIOS = {
    {
        "Broke and Brilliant",
        "You graduated 6 months ago. $82k in student loans. You've been freelancing\n"
        "  for $28/hr and it's not cutting it. You need a real job before the\n"
        "  deferment period ends and the interest kicks in.",
        82, 1   // Normal
    },
    {
        "The Lateral Move",
        "You're currently employed but underpaid by $40k. Your rent went up.\n"
        "  Your manager just got replaced. You have 4 months of runway before\n"
        "  you have to make a decision either way.",
        40, 2   // Hard
    },
    {
        "Post-Layoff",
        "You were laid off 90 days ago. $0 severance. COBRA is $800/month.\n"
        "  You have $24k in savings. At your current burn rate, that's 8 months.\n"
        "  The market is not cooperating.",
        24, 3   // Nightmare
    }
};

// Prints the story mode intro and scenario selection.
StoryState runStorySetup() {
    StoryState state;
    state.active = true;

    std::cout << "\n" << BOLD << CYAN << "  -- Story Mode --\n\n" << RESET;
    std::cout << "  Enter your name: > ";
    std::cin >> state.playerName;

    std::cout << "\n  Choose your scenario:\n\n";
    for (int i = 0; i < (int)STORY_SCENARIOS.size(); i++) {
        std::cout << "  [" << i+1 << "] " << BOLD << STORY_SCENARIOS[i].title << RESET << "\n";
        std::cout << "      " << STORY_SCENARIOS[i].setup << "\n";
        std::cout << "      Debt: $" << STORY_SCENARIOS[i].startDebt << "k  |  ";
        std::string diffs[] = { "Normal", "Normal", "Hard", "Nightmare" };
        std::cout << "Difficulty: " << diffs[STORY_SCENARIOS[i].difficulty] << "\n\n";
    }

    std::cout << "  Choice [1-" << STORY_SCENARIOS.size() << "] > ";
    std::string input;
    std::cin >> input;

    int choice = 0;
    try { choice = std::stoi(input) - 1; } catch (...) { choice = 0; }
    if (choice < 0 || choice >= (int)STORY_SCENARIOS.size()) choice = 0;

    const auto& sc = STORY_SCENARIOS[choice];
    state.debt      = sc.startDebt;
    state.startDebt = sc.startDebt;

    std::cout << "\n" << BOLD << "  " << state.playerName << ". "
              << sc.title << ".\n" << RESET;
    std::cout << DIM << "  Debt: $" << state.debt << "k. Clock is ticking.\n\n" << RESET;

    return state;
}

// Called each in-game day. If employed, reduces debt by income/30.
// Returns true if debt hits zero (win condition).
bool storyTick(StoryState& s) {
    if (!s.active) return false;
    s.storyDay++;

    if (s.employed && s.income > 0) {
        // Daily debt reduction: income (annual $k) / 365 days
        // Simplified to income / 30 per in-game day
        int dailyReduction = std::max(1, s.income / 30);
        s.debt -= dailyReduction;
        if (s.debt <= 0) {
            s.debt = 0;
            return true;   // debt cleared -- story complete
        }
    }
    return false;
}

// Called when player accepts an offer in story mode.
void storyOnOffer(StoryState& s, int baseSalary) {
    s.employed = true;
    s.income   = baseSalary;
    std::cout << "\n" << GREEN << BOLD
              << "  Story: employed at $" << baseSalary << "k.\n" << RESET;
    std::cout << DIM << "  Debt reduction: ~$" << (baseSalary / 30)
              << "k/day in-game.\n" << RESET;
}

// Prints the debt progress bar in the HUD when story mode is active.
inline void printDebtBar(const StoryState& s) {
    if (!s.active) return;
    int pct    = (s.startDebt > 0) ? (100 - (s.debt * 100 / s.startDebt)) : 100;
    int filled = pct / 10;
    int empty  = 10 - filled;

    std::cout << "Debt:   $" << s.debt << "k  [";
    std::cout << GREEN << std::string(filled, '#') << RESET;
    std::cout << std::string(empty, '.') << "] ";
    std::cout << pct << "% cleared\n";
}

// Prints the story win screen.
inline void printStoryWin(const StoryState& s) {
    std::cout << "\n" << BGREEN << BOLD
              << "  +--------------------------------------------------+\n"
              << "  |              DEBT CLEARED. STORY COMPLETE.        |\n"
              << "  +--------------------------------------------------+\n\n" << RESET;
    std::cout << "  " << s.playerName << ", you did it.\n";
    std::cout << "  " << s.storyDay << " in-game days. $" << s.startDebt << "k gone.\n\n";
    std::cout << DIM << "  It was never just about the money.\n\n" << RESET;
}
