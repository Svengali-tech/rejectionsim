#pragma once

#include <iostream>
#include <string>
#include <vector>
#include "colors.h"
#include "utils.h"
#include "player.h"

// Layoff chance per day while employed (post-offer accepted).
// 8% per day -- enough to feel threatening, not guaranteed.
static const int LAYOFF_DAILY_CHANCE = 8;

// Returns true if a layoff event should fire today.
// Only fires if the player has accepted at least one offer.
bool shouldLayoff(const Player& p) {
    if (p.offersAccepted == 0) return false;
    return (randRange(1, 100) <= LAYOFF_DAILY_CHANCE);
}

// Runs the full layoff event.
// Player keeps their prestige tier but loses the "employed" buffer.
// Hope tanks. Streak resets. Back to the grind.
void runLayoffEvent(Player& p) {
    static const std::vector<std::string> subjectLines = {
        "An important message from our leadership team",
        "Company update",
        "A difficult but necessary decision",
        "Message from the CEO",
        "Important update regarding your role",
        "Organizational changes",
    };

    static const std::vector<std::string> openings = {
        "I'm writing to share some difficult news.",
        "This is one of the hardest messages I've had to send.",
        "Today we are announcing some significant organizational changes.",
        "After careful consideration, we have made the difficult decision to reduce our workforce.",
        "I want to be transparent with you about some changes that will affect our team.",
    };

    static const std::vector<std::string> explanations = {
        "The macroeconomic environment has shifted in ways we did not anticipate when we built our plan.",
        "We grew too fast and need to right-size the organization to reflect current market realities.",
        "After a thorough review, we've made the decision to restructure several teams.",
        "To ensure the long-term health of the business, we need to make some hard tradeoffs today.",
        "Our investors have asked us to focus on a path to profitability, which requires us to make cuts.",
        "We over-hired in 2021 and 2022, and we are now correcting that.",
    };

    static const std::vector<std::string> yourRoleMsgs = {
        "Unfortunately, your role has been eliminated as part of this restructuring.",
        "Your position is one of those impacted by today's changes.",
        "After careful consideration, we have made the difficult decision to eliminate your role.",
        "Your team is one of the areas most affected by this reorganization.",
    };

    static const std::vector<std::string> closings = {
        "We will be providing severance, and a calendar invite for a 15-minute offboarding call\n  will arrive shortly.",
        "Your access to company systems will be revoked at 5pm today.",
        "HR will be in touch with next steps. We wish you the best.",
        "We are grateful for everything you contributed. This was not a reflection of your performance.",
        "Please return any company equipment per the instructions you will receive by email.",
    };

    static const std::vector<std::string> psLines = {
        "P.S. The all-hands where they announced this was 8 minutes long.",
        "P.S. The CEO still has his company jet.",
        "P.S. A 'company update' Slack post went out 4 minutes before this email.",
        "P.S. The role was reposted 3 weeks later as a contractor position.",
        "P.S. Your manager found out the same time you did.",
        "P.S. The executives who made this call all kept their jobs.",
    };

    // Ring the bell -- this is bad news
    printf("\a");
    sleepMs(300);
    printf("\a");

    std::cout << "\n";
    std::cout << RED << BOLD
              << "  +--------------------------------------------------+\n"
              << "  |                  YOU'VE BEEN LAID OFF             |\n"
              << "  +--------------------------------------------------+\n\n" << RESET;

    std::cout << DIM << "  From:    people@" << "yourformeremployer.com\n";
    std::cout << "  Subject: " << pickRandom(subjectLines) << "\n\n" << RESET;

    std::cout << "  " << pickRandom(openings) << "\n\n";
    std::cout << "  " << pickRandom(explanations) << "\n\n";
    std::cout << "  " << pickRandom(yourRoleMsgs) << "\n\n";
    std::cout << "  " << pickRandom(closings) << "\n\n";
    std::cout << DIM << "  " << pickRandom(psLines) << "\n\n" << RESET;

    // Stat impact: hope tanks, but prestige stays. You keep everything you learned.
    int hopeLoss = randRange(25, 40);
    p.hope = clamp(p.hope - hopeLoss, 0, 100);

    std::cout << RED << "  Hope -" << hopeLoss << ".\n" << RESET;
    std::cout << DIM << "  Prestige and tier preserved. You keep what you earned.\n"
              << "  Back to the grind.\n\n" << RESET;
}
