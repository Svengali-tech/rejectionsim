#pragma once

#include <iostream>
#include <string>
#include <vector>
#include "colors.h"
#include "utils.h"

// Energy cost for interview prep.
static const int PREP_ENERGY_COST = 2;

// Pass rate bonus granted by prepping.
// Applied to the next interview pipeline only, then consumed.
static const int PREP_BONUS = 20;

// Interview prep state. Attached to Player.
struct PrepState {
    bool  active = false;   // true = bonus queued for next pipeline
    int   bonus  = 0;       // current queued bonus (PREP_BONUS when active)

    // Activates the prep bonus.
    void activate() {
        active = true;
        bonus  = PREP_BONUS;
    }

    // Consumes the bonus. Call at the start of any interview pipeline.
    // Returns the bonus amount so the caller can apply it.
    int consume() {
        if (!active) return 0;
        active = false;
        int b  = bonus;
        bonus  = 0;
        return b;
    }
};

// Runs the interview prep action.
// Costs 2 energy. Grants a one-time +20% bonus to the next pipeline.
// Returns false if not enough energy.
bool runInterviewPrep(PrepState& prep, int& energy) {
    if (energy < PREP_ENERGY_COST) {
        std::cout << RED << "  Not enough energy to prep. Sleep first ([s]).\n" << RESET;
        return false;
    }

    if (prep.active) {
        std::cout << YELLOW << "  Prep already active. It'll apply to your next interview.\n"
                  << DIM   << "  No point stacking it.\n" << RESET;
        return false;
    }

    energy -= PREP_ENERGY_COST;
    prep.activate();

    static const std::vector<std::string> prepMsgs = {
        "  You spent 4 hours on Glassdoor reading every interview question for this company.\n"
        "  You now know more about their process than some of their employees.",

        "  Mock interview with a friend. They asked a question you've never seen.\n"
        "  You handled it. That felt good.",

        "  You researched the team, the product, the recent funding round,\n"
        "  and the interviewer's last three LinkedIn posts. You're ready.",

        "  You practiced your STAR stories out loud until they sounded natural.\n"
        "  The 'tell me about a time you failed' one is finally not painful.",

        "  Four hours of system design prep. You have opinions about database sharding now.\n"
        "  You did not have those opinions yesterday.",

        "  You read the job description seventeen times and built a story for every bullet point.\n"
        "  Overkill. Necessary.",

        "  Case prep session for three hours. You can structure a profitability problem\n"
        "  in 30 seconds flat now. Your imaginary client is very impressed.",
    };

    std::cout << BBLUE << "[INTERVIEW PREP]\n" << RESET;
    std::cout << pickRandom(prepMsgs) << "\n";
    std::cout << GREEN << "  >> Prep bonus active: +" << PREP_BONUS
              << "% to all stages in your next interview pipeline.\n" << RESET;
    std::cout << DIM << "  Consumed on next pipeline entry.\n\n" << RESET;

    return true;
}
