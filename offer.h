#pragma once

#include <iostream>
#include <string>
#include <vector>
#include "colors.h"
#include "utils.h"
#include "player.h"
#include "display.h"
#include "companies.h"

// Salary ranges scale with resume tier so prestige feels rewarding.
struct SalaryRange {
    int baseMin;
    int baseMax;
    int equityMin;
    int equityMax;
    int bonusMin;
    int bonusMax;
};

// Each tier gets a pay bump. Numbers are rough approximations of real market rates.
inline SalaryRange getSalaryRange(ResumeTier tier) {
    switch (tier) {
        case ResumeTier::Junior:    return { 100, 140,  10,  50,  5, 15 };
        case ResumeTier::Mid:       return { 130, 170,  20,  80, 10, 20 };
        case ResumeTier::Senior:    return { 160, 220,  40, 120, 15, 30 };
        case ResumeTier::Staff:     return { 200, 280,  80, 200, 20, 40 };
        case ResumeTier::Principal: return { 250, 350, 100, 300, 25, 50 };
    }
    return { 100, 150, 10, 50, 5, 15 };
}

// Runs the full offer flow: display, optional negotiation, accept/decline.
// Modifies player state in place (offers count, hope, prestige if accepted).
void runOffer(Player& p, const std::string& co, const std::string& role) {
    static const std::vector<std::string> offerRemarks = {
        "  They say it's 'competitive for the market.' It isn't.",
        "  The equity 'could be worth a lot someday.' Could be.",
        "  Benefits include 'unlimited PTO.' You will never use it.",
        "  Sign-on claws back if you leave within 12 months. Fine print.",
        "  'We move fast here.' You will find out what that means on day one.",
        "  'Strong team culture.' The Glassdoor reviews say otherwise.",
        "  'Competitive total comp.' Base is low. Equity is optimistic.",
    };

    SalaryRange range = getSalaryRange(p.tier);
    int base   = randRange(range.baseMin,   range.baseMax);
    int equity = randRange(range.equityMin, range.equityMax);
    int bonus  = randRange(range.bonusMin,  range.bonusMax);

    p.offers++;
    p.hope = clamp(p.hope + 40, 0, 100);

    // Track fastest offer for leaderboard
    if (p.fastestOffer < 0) p.fastestOffer = p.day;

    // Play the bell once on offer -- the one time it should feel good
    printf("\a");

    printOfferBanner(co, role);
    std::cout << "  Base salary:   " << GREEN << BOLD << "$" << base << "k" << RESET << "\n";
    std::cout << "  Equity (est.): " << YELLOW << "$" << equity << "k" << RESET
              << DIM << "  (4yr vest, 1yr cliff)" << RESET << "\n";
    std::cout << "  Sign-on:       $" << bonus << "k\n";
    std::cout << "\n" << DIM << pickRandom(offerRemarks) << RESET << "\n\n";

    // Negotiation attempt
    bool negotiate = askYesNo("Negotiate?");

    if (negotiate) {
        int roll = randRange(1, 100);
        if (roll <= 40) {
            // They budge a little
            int bump = randRange(5, 15);
            std::cout << GREEN << "\n  They came up $" << bump << "k. 'Best they could do.'\n" << RESET;
            std::cout << DIM << "  It wasn't.\n" << RESET;
        } else if (roll <= 70) {
            // Equity instead of base
            std::cout << YELLOW << "\n  'No flexibility on base. We can add equity.'\n" << RESET;
            std::cout << DIM << "  Vests in 4 years. You'll see.\n" << RESET;
        } else {
            // Rescinded
            std::cout << RED << "\n  They rescinded the offer.\n" << RESET;
            std::cout << DIM << "  'Your expectations weren't aligned with the role.'\n" << RESET;

            // Bell of shame
            printf("\a");

            p.offers--;
            p.hope = clamp(p.hope - 50, 0, 100);
            p.reputationDown(5);
            return;
        }
    } else {
        std::cout << DIM << "\n  Smart. Probably.\n" << RESET;
    }

    // Accept prompt
    bool accept = askYesNo("Accept the offer?");

    if (accept) {
        std::cout << "\n" << BOLD << GREEN
                  << "  You accepted. Start date TBD.\n"
                  << "  They said paperwork coming 'this week.'\n" << RESET;
        std::cout << DIM << "  It arrived 12 days later.\n\n" << RESET;

        p.prestige_up();
        printUnlockNotice(p);

        std::cout << "\n" << BOLD
                  << "  Resume tier is now: " << tierName(p.tier) << "\n"
                  << "  Max energy increased to " << p.maxEnergy << " per day.\n\n" << RESET;

    } else {
        // Declined the offer
        std::cout << DIM << "\n  You declined. "
                  << "The recruiter said 'no worries' and immediately moved on.\n" << RESET;
        p.reputationUp(2);   // declining professionally is fine
    }
}
