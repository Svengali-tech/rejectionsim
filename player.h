#pragma once

#include <string>
#include <iostream>
#include <iomanip>
#include "colors.h"
#include "utils.h"
#include "skills.h"
#include "burnout.h"
#include "streaks.h"
#include "difficulty.h"
#include "events.h"
#include "coverletter.h"
#include "prep.h"
#include "careers.h"

enum class ResumeTier { Junior=0, Mid=1, Senior=2, Staff=3, Principal=4 };

inline std::string tierName(ResumeTier t) {
    switch(t) {
        case ResumeTier::Junior:    return "Junior";
        case ResumeTier::Mid:       return "Mid-Level";
        case ResumeTier::Senior:    return "Senior";
        case ResumeTier::Staff:     return "Staff";
        case ResumeTier::Principal: return "Principal";
    }
    return "Unknown";
}

enum class Season { BullMarket=0, NormalMarket=1, SlowMarket=2, BearMarket=3, Freezing=4 };

inline std::string seasonName(Season s) {
    switch(s) {
        case Season::BullMarket:   return "Bull Market";
        case Season::NormalMarket: return "Normal Market";
        case Season::SlowMarket:   return "Slow Market";
        case Season::BearMarket:   return "Bear Market";
        case Season::Freezing:     return "Hiring Freeze";
    }
    return "Unknown";
}

inline int seasonModifier(Season s) {
    switch(s) {
        case Season::BullMarket:   return 20;
        case Season::NormalMarket: return 0;
        case Season::SlowMarket:   return -10;
        case Season::BearMarket:   return -25;
        case Season::Freezing:     return -40;
    }
    return 0;
}

struct Player {
    // Progression
    int            prestige       = 0;
    ResumeTier     tier           = ResumeTier::Junior;
    int            reputation     = 50;
    int            connections    = 0;
    bool           hasReferral    = false;
    CareerTrack    track          = CareerTrack::SoftwareEng;

    // Resources
    int            hope           = 100;
    int            energy         = 10;
    int            maxEnergy      = 10;
    int            day            = 1;

    // Stats
    int            applied        = 0;
    int            rejected       = 0;
    int            ghosted        = 0;
    int            interviews     = 0;
    int            offers         = 0;
    int            offersAccepted = 0;
    int            networked      = 0;
    int            atsFailed      = 0;
    int            ventCount      = 0;

    // Track coverage for achievements
    bool           appliedSWE     = false;
    bool           appliedFin     = false;
    bool           appliedCon     = false;

    // Market
    Season         season         = Season::NormalMarket;
    int            daysInSeason   = 0;

    // Systems
    Skills         skills;
    Burnout        burnout;
    RejectionStreak streak;
    Difficulty     difficulty     = Difficulty::Normal;
    CoverLetterStyle coverLetter  = CoverLetterStyle::None;
    PrepState      prep;
    EventSlot      eventSlot;

    // Leaderboard
    int            fastestOffer   = -1;

    bool advanceDay();
    void prestige_up();
    void reputationUp(int n)   { reputation = clamp(reputation+n, 0, 100); }
    void reputationDown(int n) { reputation = clamp(reputation-n, 0, 100); }
    int  effectiveRate(int baseRate) const;
    std::string reputationLabel() const;
};

inline bool Player::advanceDay() {
    day++;
    energy = maxEnergy;
    daysInSeason++;
    burnout.onRest();
    eventSlot.tick();

    bool changed = false;
    if (daysInSeason >= 10) {
        daysInSeason = 0;
        const auto& cfg = getDifficultyConfig(difficulty);
        int roll = randRange(0,4);
        if (cfg.bullMarketBias && roll >= 2) roll = randRange(0,1);
        if (cfg.bearMarketBias && roll <= 2) roll = randRange(3,4);
        Season next = static_cast<Season>(roll);
        if (next != season) { season = next; changed = true; }
    }
    return changed;
}

inline void Player::prestige_up() {
    prestige++;
    offersAccepted++;
    int nextTier = clamp(static_cast<int>(tier)+1, 0, 4);
    tier = static_cast<ResumeTier>(nextTier);
    maxEnergy = clamp(maxEnergy+1, 10, 15);
    reputationUp(10);
    burnout.reset();
    streak.onOffer();
}

inline int Player::effectiveRate(int baseRate) const {
    const auto& cfg = getDifficultyConfig(difficulty);
    int rate = baseRate;
    if (hasReferral) rate = clamp(rate*3, 0, 95);
    rate += static_cast<int>(tier) * 5;
    // Skill bonuses by stage (inferred from typical base rates per track)
    if      (baseRate >= 50) rate += skillBonus(skills.portfolioLevel);
    else if (baseRate >= 38) rate += skillBonus(skills.leetcodeLevel);
    else                     rate += skillBonus(skills.cloutLevel);
    if (prep.active) rate += prep.bonus;
    rate += seasonModifier(season);
    rate += eventSlot.modifier();
    rate += cfg.passRateMod;
    rate += burnoutPenalty(burnout.level);
    return clamp(rate, 5, 95);
}

inline std::string Player::reputationLabel() const {
    if (reputation >= 80) return "Excellent";
    if (reputation >= 60) return "Good";
    if (reputation >= 40) return "Neutral";
    if (reputation >= 20) return "Poor";
    return "Blacklisted";
}
