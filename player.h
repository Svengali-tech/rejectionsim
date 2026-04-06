#pragma once

#include <string>
#include <iostream>
#include <iomanip>
#include "colors.h"
#include "utils.h"

// Resume tiers in order. Each prestige level unlocks better base odds.
// Junior -> Mid -> Senior -> Staff -> Principal
enum class ResumeTier {
    Junior    = 0,
    Mid       = 1,
    Senior    = 2,
    Staff     = 3,
    Principal = 4
};

// Returns the display name for a given resume tier.
inline std::string tierName(ResumeTier t) {
    switch (t) {
        case ResumeTier::Junior:    return "Junior";
        case ResumeTier::Mid:       return "Mid-Level";
        case ResumeTier::Senior:    return "Senior";
        case ResumeTier::Staff:     return "Staff";
        case ResumeTier::Principal: return "Principal";
    }
    return "Unknown";
}

// Season affects base acceptance rates across the board.
// Bull = hot market, Bear = freeze, everything in between.
enum class Season {
    BullMarket   = 0,   // +20% to all pass rates
    NormalMarket = 1,   // baseline rates
    SlowMarket   = 2,   // -10% to all pass rates
    BearMarket   = 3,   // -25% to all pass rates, layoffs everywhere
    Freezing     = 4    // -40%, hiring freeze memos going out daily
};

inline std::string seasonName(Season s) {
    switch (s) {
        case Season::BullMarket:   return "Bull Market (everyone's hiring)";
        case Season::NormalMarket: return "Normal Market";
        case Season::SlowMarket:   return "Slow Market (budget reviews)";
        case Season::BearMarket:   return "Bear Market (layoffs trending)";
        case Season::Freezing:     return "Hiring Freeze (good luck)";
    }
    return "Unknown";
}

// Returns the pass rate modifier for the current season.
// Applied as an additive percentage bonus/penalty to each interview stage.
inline int seasonModifier(Season s) {
    switch (s) {
        case Season::BullMarket:   return 20;
        case Season::NormalMarket: return 0;
        case Season::SlowMarket:   return -10;
        case Season::BearMarket:   return -25;
        case Season::Freezing:     return -40;
    }
    return 0;
}

// Core player state. Everything the game tracks about the player lives here.
// Pass this by reference to any function that needs to read or modify player state.
struct Player {
    // --- Progression ---
    int         prestige      = 0;        // number of offers accepted (prestige level)
    ResumeTier  tier          = ResumeTier::Junior;
    int         reputation    = 50;       // 0-100: affects referral quality and callback rate
    int         connections   = 0;        // earned through networking, unlocks referrals
    bool        hasReferral   = false;    // set when networking pays off, consumed on next apply

    // --- Resources ---
    int         hope          = 100;      // 0-100: the main emotional stat
    int         energy        = 10;       // refills each "day", capped at maxEnergy
    int         maxEnergy     = 10;       // increases with prestige
    int         day           = 1;        // current in-game day

    // --- Stats ---
    int         applied       = 0;
    int         rejected      = 0;
    int         ghosted       = 0;
    int         interviews    = 0;        // times entered interview pipeline
    int         offers        = 0;        // offers received (not necessarily accepted)
    int         offersAccepted= 0;        // prestige triggers on accept
    int         networked     = 0;        // times networking action used
    int         atsFailed     = 0;        // auto-rejected by ATS before human saw it

    // --- Market ---
    Season      season        = Season::NormalMarket;
    int         daysInSeason  = 0;        // counts up; season rotates every ~10 days

    // --- Leaderboard tracking ---
    int         fastestOffer  = -1;       // day number when first offer arrived, -1 = none yet

    // Advances one game day. Refills energy, ages the season, fires LinkedIn pings.
    // Returns true if the season changed (caller can print a market update).
    bool advanceDay();

    // Bumps prestige by 1 and upgrades the resume tier.
    // Called when the player accepts an offer.
    void prestige_up();

    // Reputation helpers
    void reputationUp(int amount)   { reputation = clamp(reputation + amount, 0, 100); }
    void reputationDown(int amount) { reputation = clamp(reputation - amount, 0, 100); }

    // Returns the effective pass rate for a stage, blending tier bonus + season modifier.
    // baseRate is the raw % chance (e.g. 45 for technical round).
    int effectiveRate(int baseRate) const;

    // Returns the display label for the current reputation bracket.
    std::string reputationLabel() const;
};

// --- Player method implementations ---

inline bool Player::advanceDay() {
    day++;
    energy = maxEnergy;   // new day, full energy
    daysInSeason++;

    // Season rotates every 10 days. Randomly picks the next one.
    bool changed = false;
    if (daysInSeason >= 10) {
        daysInSeason = 0;
        int roll = randRange(0, 4);
        Season next = static_cast<Season>(roll);
        if (next != season) {
            season = next;
            changed = true;
        }
    }
    return changed;
}

inline void Player::prestige_up() {
    prestige++;
    offersAccepted++;

    // Upgrade tier up to Principal cap
    int nextTier = clamp(static_cast<int>(tier) + 1, 0, 4);
    tier = static_cast<ResumeTier>(nextTier);

    // Each prestige level adds one max energy (more stamina as you get better)
    maxEnergy = clamp(maxEnergy + 1, 10, 15);

    // Accepting an offer is good for your reputation
    reputationUp(10);
}

inline int Player::effectiveRate(int baseRate) const {
    // Tier bonus: each prestige level adds 5% to pass rates
    int tierBonus = static_cast<int>(tier) * 5;

    // Referral bonus: 3x the base pass rate (capped so it doesn't exceed 95)
    int rate = baseRate;
    if (hasReferral) rate = clamp(rate * 3, 0, 95);

    rate += tierBonus + seasonModifier(season);
    return clamp(rate, 5, 95);   // always at least 5% chance, never guaranteed
}

inline std::string Player::reputationLabel() const {
    if (reputation >= 80) return "Excellent";
    if (reputation >= 60) return "Good";
    if (reputation >= 40) return "Neutral";
    if (reputation >= 20) return "Poor";
    return "Blacklisted";
}
