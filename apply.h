#pragma once

#include <iostream>
#include <string>
#include <vector>
#include "colors.h"
#include "utils.h"
#include "player.h"
#include "companies.h"
#include "display.h"
#include "interviews.h"
#include "offer.h"
#include "networking.h"

// Energy cost per application.
static const int APPLY_ENERGY_COST = 1;

// Milestones: fires a message when applied count hits the threshold.
// Add entries here to add more milestone callouts.
static const std::vector<std::pair<int, std::string>> MILESTONES = {
    {   5, "5 shots. Just warming up."                                          },
    {  10, "10 deep. Easy Apply is muscle memory now."                          },
    {  20, "20 apps. Cover letter is basically a template."                     },
    {  30, "30. You have more data on hiring cycles than most analysts."         },
    {  50, "50. Applied to the same company twice by accident."                 },
    {  75, "75. You check the portal before breakfast."                         },
    { 100, "100 applications. A century of unanswered effort. LEGENDARY for you."       },
    { 150, "150. The number is no longer fucking surprising. That's the scariest part." },
    { 200, "200. You could write a book. Nobody would want to fucking read it."         },
};

static const std::vector<std::string> REJECTIONS = {
    "After careful consideration, we've decided to move forward\n  with other candidates.",
    "We were impressed, but we're looking for someone with more\n  experience in our specific stack.",
    "Unfortunately, we don't think this is the right fit at this time.",
    "We've decided to pause this search indefinitely. Best of luck!",
    "We received an overwhelming number of applications.",
    "We're looking for someone local. No relocation budget.",
    "We've decided to go with an internal candidate.",
    "We're restructuring this role and putting the search on hold.",
    "Your experience doesn't quite align with our current needs.",
    "After further review, we will not be moving forward.",
    "We went with a candidate who was a stronger overall match. (we don't like you)",
    "We've reprioritized our engineering roadmap. Role on hold. (did i mention we don't like you)",
};

static const std::vector<std::string> GHOST_MSGS = {
    "No response. Read receipts: off.",
    "Left on read. Classic.",
    "Your follow-up email was also ignored.",
    "The job posting was quietly deleted.",
    "They reposted the same role the following week.",
    "Opened at 11pm. Never replied.",
    "ATS auto-acknowledged. Human: never.",
    "Status in portal: 'Under Review' for 6 weeks.",
    "Their careers page now says 'no open roles.'",
};

static const std::vector<std::string> RECRUITER_SPAM = {
    "Hi! I came across your profile. We have a GREAT opportunity --\n"
    "  Junior Dev role, mostly Excel, pays $52k. LMK!",

    "Hey! Are you open to roles? We need a Principal Architect\n"
    "  (12-15 YOE), $58k base, fully onsite in Tulsa.",

    "Quick question -- interested in a 3-month contract?\n"
    "  No benefits. Hybrid. Parsippany, NJ.",

    "Your background is impressive! We're hiring a manual QA tester.\n"
    "  Interested?",

    "Hi! Saw your profile. We're building something big in the Web3 space.\n"
    "  Paying in tokens. Huge upside.",

    "Top opportunity: senior role at a stealth startup.\n"
    "  Cannot share company name, stack, or comp range at this time.",
};

// Runs one full application cycle.
// Handles energy check, ATS, interview pipeline, offer, and all player stat updates.
// Returns false if the action was blocked (not enough energy).
bool runApply(Player& p) {
    if (p.energy < APPLY_ENERGY_COST) {
        std::cout << RED << "  Out of energy. Sleep first ([s]).\n" << RESET;
        return false;
    }

    // Get the pool of companies the player can currently access
    std::vector<Company> pool = getAvailableCompanies(p);
    if (pool.empty()) {
        std::cout << RED << "  No companies available. Network more.\n" << RESET;
        return false;
    }

    p.energy -= APPLY_ENERGY_COST;
    p.applied++;

    Company  co   = pool[rand() % pool.size()];
    std::string role = pickRandom(ALL_ROLES);

    std::cout << "\n" << BOLD << "To: " << co.name << " -- " << role << RESET << "\n";

    // Show referral status if active
    printReferralReminder(p);
    sleepMs(400);

    double roll = randDouble();

    // -- 5% recruiter spam (not a real application outcome)
    if (roll < 0.05) {
        printInboxTag("RECRUITER SPAM", BLUE);
        std::cout << "  " << pickRandom(RECRUITER_SPAM) << "\n";
        p.hope = clamp(p.hope + 2, 0, 100);
        std::cout << DIM << "  (At least it's technically a response.)\n" << RESET;

    // -- ATS filter check first (before any human involvement)
    } else if (!runATSFilter(p, co.name)) {
        // ATS filtered: already handled inside runATSFilter
        printf("\a");   // bell of rejection

    // -- 30% ghosted
    } else if (roll < 0.35) {
        p.ghosted++;
        p.hope = clamp(p.hope - 8, 0, 100);
        printInboxTag("GHOSTED", DIM);
        std::cout << "  " << pickRandom(GHOST_MSGS) << "\n";

    // -- 45% flat rejected
    } else if (roll < 0.80) {
        p.rejected++;
        p.hope = clamp(p.hope - 12, 0, 100);
        printInboxTag("REJECTED", RED);
        std::cout << "  " << pickRandom(REJECTIONS) << "\n";
        printf("\a");   // bell of rejection

    // -- 20% interview pipeline
    } else {
        p.interviews++;
        p.hope = clamp(p.hope + 5, 0, 100);
        printInboxTag("INTERVIEW PIPELINE", CYAN);
        std::cout << DIM << "  They actually responded. Suspicious.\n\n" << RESET;
        sleepMs(300);

        // Consume referral if active
        bool hadReferral = p.hasReferral;
        p.hasReferral = false;

        if (!runPhoneScreen(p, co.name)) {
            p.rejected++;
            std::cout << "\n";
        } else {
            std::cout << "\n";
            sleepMs(400);

            if (!runTechnical(p, co.name)) {
                p.rejected++;
                std::cout << "\n";
            } else {
                std::cout << "\n";
                sleepMs(400);

                if (!runFinalRound(p, co.name)) {
                    p.rejected++;
                    std::cout << "\n";
                } else {
                    // Reference check before offer
                    bool refPassed = runReferenceCheck(p);
                    std::cout << "\n";
                    sleepMs(500);

                    if (!refPassed) {
                        p.rejected++;
                        std::cout << RED << "  Offer withdrawn before it was made. "
                                  << "Your reference did something.\n" << RESET;
                    } else {
                        runOffer(p, co.name, role);
                    }
                }
            }
        }

        // Restore referral note in output if it was consumed
        if (hadReferral) {
            std::cout << DIM << "  (Referral consumed.)\n" << RESET;
        }
    }

    // Milestone check
    for (const auto& [n, msg] : MILESTONES) {
        if (p.applied == n) {
            std::cout << "\n" << BYELLOW << BOLD << "  >> " << msg << RESET << "\n";
        }
    }

    // LinkedIn ping (random flavor)
    maybeLinkedInPing();

    // Hope floor warning
    if (p.hope <= 0) {
        std::cout << "\n" << RED << BOLD
                  << "  !! HOPE DEPLETED. The market has won.\n"
                  << "     But you're still here. That counts.\n" << RESET;
    }

    std::cout << "\n";
    return true;
}
