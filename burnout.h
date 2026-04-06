#pragma once

#include <iostream>
#include <string>
#include <vector>
#include "colors.h"
#include "utils.h"

// Burnout levels. Increases when you grind without rest.
// Decreases when you sleep or network.
enum class BurnoutLevel {
    Fresh    = 0,   // no debuff
    Tired    = 1,   // -5% to all pass rates, mild flavor text
    Burned   = 2,   // -15% to all pass rates, cover letter typos
    Crispy   = 3    // -25% to all pass rates, LinkedIn messages unhinged
};

inline std::string burnoutLabel(BurnoutLevel b) {
    switch (b) {
        case BurnoutLevel::Fresh:   return "Fresh";
        case BurnoutLevel::Tired:   return "Tired";
        case BurnoutLevel::Burned:  return "Burned Out";
        case BurnoutLevel::Crispy:  return "Crispy";
    }
    return "Unknown";
}

inline const char* burnoutColor(BurnoutLevel b) {
    switch (b) {
        case BurnoutLevel::Fresh:   return GREEN;
        case BurnoutLevel::Tired:   return YELLOW;
        case BurnoutLevel::Burned:  return RED;
        case BurnoutLevel::Crispy:  return BRED;
    }
    return RESET;
}

// Returns the pass rate penalty for the current burnout level.
// Added (as negative) in effectiveRate.
inline int burnoutPenalty(BurnoutLevel b) {
    switch (b) {
        case BurnoutLevel::Fresh:   return 0;
        case BurnoutLevel::Tired:   return -5;
        case BurnoutLevel::Burned:  return -15;
        case BurnoutLevel::Crispy:  return -25;
    }
    return 0;
}

// Burnout tracker. Attached to Player.
struct Burnout {
    int  consecutiveGrindDays = 0;   // days in a row applying/training without rest
    BurnoutLevel level        = BurnoutLevel::Fresh;

    // Called at the end of each active day (applying/training).
    // Returns true if burnout level increased.
    bool onGrindDay() {
        consecutiveGrindDays++;
        BurnoutLevel prev = level;

        if      (consecutiveGrindDays >= 7) level = BurnoutLevel::Crispy;
        else if (consecutiveGrindDays >= 4) level = BurnoutLevel::Burned;
        else if (consecutiveGrindDays >= 2) level = BurnoutLevel::Tired;

        return level != prev;
    }

    // Called when the player sleeps or networks.
    // Resets grind counter and drops burnout one level.
    void onRest() {
        consecutiveGrindDays = 0;
        int cur = static_cast<int>(level);
        if (cur > 0) level = static_cast<BurnoutLevel>(cur - 1);
    }

    // Full reset (new game / prestige).
    void reset() {
        consecutiveGrindDays = 0;
        level = BurnoutLevel::Fresh;
    }
};

// Prints a warning when burnout level goes up.
inline void printBurnoutWarning(BurnoutLevel level) {
    static const std::vector<std::string> tiredMsgs = {
        "  You've been at this for days. The rejection emails are starting to blur.",
        "  Cover letter enthusiasm is fading. You copy-pasted the company name wrong.",
        "  You're tired. The LinkedIn notifications are starting to feel personal.",
    };
    static const std::vector<std::string> burnedMsgs = {
        "  You are burned out. Your cover letter has a typo in the first sentence.",
        "  You sent an application at 2am. The role was in Denver. You didn't notice.",
        "  Your cover letter now says 'I am very passionate about synergies.'",
        "  You wrote 'I look forward to hearing from you' three times in one paragraph.",
    };
    static const std::vector<std::string> crispyMsgs = {
        "  You are fully crispy. Your cover letter opens with 'To Whom It May Concern, please.'",
        "  You accidentally sent the same application to the same company twice. This week.",
        "  Your cover letter is now 800 words. The last 200 are just job titles you've been rejected from.",
        "  You replied to a rejection email asking if they were sure.",
    };

    std::cout << "\n";
    switch (level) {
        case BurnoutLevel::Tired:
            std::cout << YELLOW << BOLD << "  [BURNOUT: Tired]\n" << RESET;
            std::cout << pickRandom(tiredMsgs) << "\n";
            std::cout << DIM << "  Pass rate penalty: -5%. Sleep or network to recover.\n" << RESET;
            break;
        case BurnoutLevel::Burned:
            std::cout << RED << BOLD << "  [BURNOUT: Burned Out]\n" << RESET;
            std::cout << pickRandom(burnedMsgs) << "\n";
            std::cout << DIM << "  Pass rate penalty: -15%. Cover letter quality: questionable.\n" << RESET;
            break;
        case BurnoutLevel::Crispy:
            std::cout << BRED << BOLD << "  [BURNOUT: Crispy]\n" << RESET;
            std::cout << pickRandom(crispyMsgs) << "\n";
            std::cout << DIM << "  Pass rate penalty: -25%. You need to stop.\n" << RESET;
            break;
        default: break;
    }
    std::cout << "\n";
}

// Prints a recovery message when the player rests.
inline void printBurnoutRecovery(BurnoutLevel newLevel) {
    if (newLevel == BurnoutLevel::Fresh) {
        std::cout << GREEN << DIM << "  Burnout cleared. You feel like a person again.\n" << RESET;
    } else {
        std::cout << DIM << "  Burnout improved: now " << burnoutLabel(newLevel)
                  << ". Keep resting.\n" << RESET;
    }
}

// Returns a typo-flavored cover letter warning when burned out.
// Called in apply.h before the application outcome to add flavor.
inline void maybePrintTypoWarning(BurnoutLevel b) {
    if (b < BurnoutLevel::Burned) return;

    static const std::vector<std::string> typos = {
        "  (Your cover letter addressed them as 'Dear Hiring Manger.')",
        "  (Your cover letter mentioned your 'proven tract record.')",
        "  (You wrote 'I am exited to apply.' They noticed.)",
        "  (You spelled the company name wrong in the subject line.)",
        "  (Your cover letter ended mid-sentence. You didn't catch it.)",
        "  (You attached last week's resume by accident. Different target comp.)",
    };

    std::cout << YELLOW << DIM << pickRandom(typos) << "\n" << RESET;
}
