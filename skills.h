#pragma once

#include <iostream>
#include <string>
#include <vector>
#include "colors.h"
#include "utils.h"

// The three player skills. Each has a level (0-5) and an XP counter.
// They're separate from reputation and connections -- these are things
// you actively grind, and they directly affect interview pass rates.
//
// leetcodeGrind  -> bonus to technical round pass rate
// portfolioStrength -> bonus to ATS pass rate and phone screen
// networkClout   -> bonus to referral gain speed and final round
//
// Max level for all skills is 5. XP needed per level scales up.

static const int SKILL_MAX_LEVEL = 5;

// XP thresholds to reach each level (index = target level, value = XP needed).
// Level 0 is free. Each level costs more grind than the last.
static const int SKILL_XP_THRESHOLDS[SKILL_MAX_LEVEL + 1] = {
    0,    // level 0 -> 1: 0 XP (everyone starts at 0)
    10,   // level 1 -> 2
    25,   // level 2 -> 3
    50,   // level 3 -> 4
    90,   // level 4 -> 5
    999   // level 5 -> cap (never reached intentionally)
};

// Per-level pass rate bonus added to the relevant stage.
// Level 1 gives +3%, level 5 gives +15%. Meaningful but not game-breaking.
inline int skillBonus(int level) {
    return level * 3;
}

// Returns a label for the skill level so the HUD doesn't just show a number.
inline std::string skillLevelLabel(int level) {
    switch (level) {
        case 0: return "Untrained";
        case 1: return "Familiar";
        case 2: return "Practiced";
        case 3: return "Solid";
        case 4: return "Sharp";
        case 5: return "Expert";
    }
    return "Unknown";
}

// The three skills as a struct so we can pass them around together
// and serialize them cleanly in saveload.h.
struct Skills {
    // --- Leetcode Grind ---
    // Improves technical interview pass rate.
    // Train with [t leet].
    int leetcodeLevel = 0;
    int leetcodeXP    = 0;

    // --- Portfolio Strength ---
    // Improves ATS pass rate and phone screen odds.
    // Train with [t portfolio].
    int portfolioLevel = 0;
    int portfolioXP    = 0;

    // --- Network Clout ---
    // Speeds up referral unlock (lowers threshold) and adds bonus to final round.
    // Train with [t clout].
    int cloutLevel = 0;
    int cloutXP    = 0;
};

// Tries to add XP to a skill, handling level-up logic.
// Pass the level and XP fields by reference.
// Returns true if a level-up occurred so the caller can print a message.
inline bool addSkillXP(int& level, int& xp, int amount) {
    if (level >= SKILL_MAX_LEVEL) return false;   // already maxed

    xp += amount;
    if (xp >= SKILL_XP_THRESHOLDS[level + 1]) {
        xp -= SKILL_XP_THRESHOLDS[level + 1];
        level++;
        return true;
    }
    return false;
}

// Prints a single skill row for the profile/HUD display.
// Shows level label, numeric level, and an XP progress bar.
inline void printSkillRow(const std::string& name, int level, int xp) {
    int nextThreshold = (level < SKILL_MAX_LEVEL) ? SKILL_XP_THRESHOLDS[level + 1] : 999;
    int barFilled     = (level < SKILL_MAX_LEVEL) ? (xp * 10 / nextThreshold) : 10;
    int barEmpty      = 10 - barFilled;

    std::cout << "  " << std::left << std::setw(20) << name
              << " Lv" << level << " [" << skillLevelLabel(level) << "]"
              << "  XP: [";

    if (level >= SKILL_MAX_LEVEL) {
        std::cout << BGREEN << std::string(10, '#') << RESET;
    } else {
        std::cout << CYAN << std::string(barFilled, '#') << RESET
                  << std::string(barEmpty, '.');
    }
    std::cout << "] ";

    if (level < SKILL_MAX_LEVEL)
        std::cout << xp << "/" << nextThreshold;
    else
        std::cout << "MAX";

    std::cout << "\n";
}

// Runs the training action. Costs energy (set by caller).
// subcommand is "leet", "portfolio", or "clout".
// Returns false if subcommand was invalid.
bool runTraining(Skills& sk, const std::string& subcommand) {
    static const std::vector<std::string> leetMsgs = {
        "  You did 3 mediums and one hard that broke your spirit.\n"
        "  You understand sliding window now. Sort of.",

        "  Two Sum -> Three Sum -> Four Sum.\n"
        "  You're starting to see the pattern. It's bleak.",

        "  Dynamic programming session. You got through 2 problems in 2 hours.\n"
        "  Both were marked Easy.",

        "  You solved a graph problem without looking at the hint.\n"
        "  Small. But real.",

        "  Mock interview with a random on Pramp.\n"
        "  They were worse than you. That helped.",

        "  Studied system design for 3 hours.\n"
        "  You now have opinions about consistent hashing.",

        "  Grinding NeetCode 150. You're on problem 37.\n"
        "  Estimated completion: never.",
    };

    static const std::vector<std::string> portfolioMsgs = {
        "  Updated your GitHub README. Added badges. Felt productive.\n"
        "  It was 90% badges.",

        "  Pushed a new project. It's a todo app.\n"
        "  You told yourself it demonstrates full-stack competence.",

        "  Wrote a blog post about something you built.\n"
        "  Four people read it. Two were bots.",

        "  Cleaned up three old repos that had 'WIP' in the README.\n"
        "  They still have 'WIP' in the README.",

        "  Recorded a demo video of your best project.\n"
        "  Took 14 takes because of mic quality.",

        "  Added tests to a project you said you'd add tests to months ago.\n"
        "  Coverage is now 40%. You put that on your resume.",

        "  Updated your resume bullet points to be more impact-driven.\n"
        "  'Built things' became 'architected scalable solutions.' Same thing.",
    };

    static const std::vector<std::string> cloutMsgs = {
        "  Posted a thread about something you learned.\n"
        "  Twelve people liked it. One was a hiring manager.",

        "  Went to a meetup. Talked to four engineers.\n"
        "  Got two LinkedIn connections and one lukewarm 'let's keep in touch.'",

        "  Gave a short talk at a local event.\n"
        "  Eight people showed up. Three were there for the pizza.",

        "  Responded thoughtfully to five people's posts.\n"
        "  One of them followed you back.",

        "  Contributed to an open source project.\n"
        "  A maintainer said 'looks good.' You screenshotted it.",

        "  Helped someone debug their code in a Discord server.\n"
        "  They said 'thanks.' That's it. Still counts.",
    };

    int xpGain = randRange(3, 7);   // random XP per session so grind isn't perfectly predictable
    bool leveledUp = false;

    if (subcommand == "leet") {
        std::cout << YELLOW << "[LEETCODE GRIND]\n" << RESET;
        std::cout << pickRandom(leetMsgs) << "\n";
        leveledUp = addSkillXP(sk.leetcodeLevel, sk.leetcodeXP, xpGain);
        std::cout << DIM << "  Leetcode Grind XP +" << xpGain;
        if (leveledUp)
            std::cout << BGREEN << "  LEVEL UP -> " << skillLevelLabel(sk.leetcodeLevel) << RESET;
        std::cout << "\n" << RESET;

    } else if (subcommand == "portfolio") {
        std::cout << CYAN << "[PORTFOLIO WORK]\n" << RESET;
        std::cout << pickRandom(portfolioMsgs) << "\n";
        leveledUp = addSkillXP(sk.portfolioLevel, sk.portfolioXP, xpGain);
        std::cout << DIM << "  Portfolio Strength XP +" << xpGain;
        if (leveledUp)
            std::cout << BGREEN << "  LEVEL UP -> " << skillLevelLabel(sk.portfolioLevel) << RESET;
        std::cout << "\n" << RESET;

    } else if (subcommand == "clout") {
        std::cout << MAGENTA << "[NETWORK CLOUT]\n" << RESET;
        std::cout << pickRandom(cloutMsgs) << "\n";
        leveledUp = addSkillXP(sk.cloutLevel, sk.cloutXP, xpGain);
        std::cout << DIM << "  Network Clout XP +" << xpGain;
        if (leveledUp)
            std::cout << BGREEN << "  LEVEL UP -> " << skillLevelLabel(sk.cloutLevel) << RESET;
        std::cout << "\n" << RESET;

    } else {
        std::cout << RED << "  Unknown skill. Use: [t leet] [t portfolio] [t clout]\n" << RESET;
        return false;
    }

    return true;
}
