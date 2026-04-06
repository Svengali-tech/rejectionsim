#pragma once

#include <iostream>
#include <string>
#include <vector>
#include "colors.h"
#include "utils.h"
#include "player.h"
#include "display.h"

// Cost in energy to network for the day.
static const int NETWORK_ENERGY_COST = 2;

// Connections needed before a referral can fire.
static const int REFERRAL_THRESHOLD = 5;

// Runs the networking action. Costs 2 energy, builds connections over time,
// and eventually unlocks a referral that triples pass rates on the next application.
// Returns false if the player didn't have enough energy.
bool runNetworking(Player& p) {
    if (p.energy < NETWORK_ENERGY_COST) {
        std::cout << RED << "  Not enough energy to network. "
                  << "Sleep first ([s]).\n" << RESET;
        return false;
    }

    p.energy -= NETWORK_ENERGY_COST;
    p.networked++;

    static const std::vector<std::string> activities = {
        "  You messaged a second-degree connection. They left you on read for 4 days\n"
        "  before sending a calendar link to a 'quick intro call.'",

        "  You attended a virtual meetup. Made one genuine connection.\n"
        "  They work at a company you'd actually want to join.",

        "  You cold DM'd a founder on Twitter. They actually replied.\n"
        "  They said to reach out when you're ready. You're ready now.",

        "  You went to a local tech event. Talked to three engineers.\n"
        "  One of them works at a company on your list.",

        "  You commented thoughtfully on a hiring manager's post.\n"
        "  They followed you back. Small win.",

        "  You bought someone coffee (virtually) for a resume review.\n"
        "  They gave real feedback. You updated three bullet points.",

        "  You posted about a project on LinkedIn.\n"
        "  Seven people liked it. One was a recruiter. They said nothing.",

        "  You reached out to a former colleague.\n"
        "  They said they'd 'keep an ear out.' That's not nothing.",
    };

    std::cout << BCYAN << "[NETWORKING]\n" << RESET;
    std::cout << pickRandom(activities) << "\n";

    // Connections build up with reputation modifier
    int gained = randRange(1, 3);
    if (p.reputation >= 70) gained++;   // well-known people get more traction

    p.connections += gained;
    p.reputationUp(1);

    std::cout << DIM << "  +" << gained << " connection(s). Total: "
              << p.connections << "\n" << RESET;

    // Check if referral threshold hit and referral not already queued
    if (!p.hasReferral && p.connections >= REFERRAL_THRESHOLD) {
        p.hasReferral = true;
        std::cout << "\n" << BGREEN << BOLD
                  << "  >> REFERRAL UNLOCKED\n" << RESET;
        std::cout << "  A contact put in a word for you.\n";
        std::cout << DIM << "  Your next application gets a 3x pass rate bonus.\n"
                  << "  Referral consumed on apply.\n" << RESET;

        // Reset connection counter so next referral requires more work
        p.connections = 0;
    }

    maybeLinkedInPing();
    return true;
}

// Prints a reminder that the referral is active and will be consumed on next apply.
inline void printReferralReminder(const Player& p) {
    if (!p.hasReferral) return;
    std::cout << BGREEN << DIM
              << "  (Referral active -- 3x pass rate on next application)\n"
              << RESET;
}
