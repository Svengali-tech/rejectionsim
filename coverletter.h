#pragma once

#include <iostream>
#include <string>
#include <vector>
#include "colors.h"
#include "utils.h"

// The cover letter style the player has chosen.
// Set once via [c] command, persists until changed.
// Affects the base callback rate (i.e. chance of entering interview pipeline).
enum class CoverLetterStyle {
    None      = 0,   // default: no letter, standard odds
    Generic   = 1,   // worse than none actually -- screams mass apply
    Tailored  = 2,   // best option: costs energy each time
    ChatGPTd  = 3    // decent but HR can smell it, minor penalty at final round
};

inline std::string coverLetterName(CoverLetterStyle s) {
    switch (s) {
        case CoverLetterStyle::None:     return "None";
        case CoverLetterStyle::Generic:  return "Generic";
        case CoverLetterStyle::Tailored: return "Tailored";
        case CoverLetterStyle::ChatGPTd: return "AI-Generated";
    }
    return "None";
}

// Returns the pipeline entry rate modifier for the cover letter style.
// This is added to the base 20% pipeline chance in apply.h.
inline int coverLetterBonus(CoverLetterStyle s) {
    switch (s) {
        case CoverLetterStyle::None:     return 0;
        case CoverLetterStyle::Generic:  return -5;   // actually hurts you
        case CoverLetterStyle::Tailored: return 10;   // meaningful improvement
        case CoverLetterStyle::ChatGPTd: return 3;    // better than nothing
    }
    return 0;
}

// Returns the final round pass rate modifier.
// AI-generated letters occasionally get called out late in the process.
inline int coverLetterFinalRoundPenalty(CoverLetterStyle s) {
    if (s == CoverLetterStyle::ChatGPTd) return -8;
    return 0;
}

// Prints the cover letter selection menu and returns the chosen style.
CoverLetterStyle runCoverLetterSetup() {
    std::cout << "\n" << BOLD << "  -- Cover Letter Setup --\n\n" << RESET;
    std::cout << "  Choose your cover letter strategy:\n\n";
    std::cout << "  [1] None         -- no letter. fine for most SWE roles. 0% bonus.\n";
    std::cout << "  [2] Generic      -- copy-paste template. hurts more than helps. -5%.\n";
    std::cout << "  [3] Tailored     -- written for each role. +10% pipeline rate.\n";
    std::cout << "                     costs 1 extra energy per application.\n";
    std::cout << "  [4] AI-Generated -- decent. +3% pipeline rate.\n";
    std::cout << "                     -8% final round (they've seen it before).\n\n";
    std::cout << "  Choice [1-4] > ";

    std::string input;
    std::cin >> input;

    if      (input == "1") return CoverLetterStyle::None;
    else if (input == "2") return CoverLetterStyle::Generic;
    else if (input == "3") return CoverLetterStyle::Tailored;
    else if (input == "4") return CoverLetterStyle::ChatGPTd;

    std::cout << DIM << "  Invalid choice. Defaulting to None.\n" << RESET;
    return CoverLetterStyle::None;
}

// Flavor text shown when applying with a specific cover letter style.
inline void printCoverLetterFlavor(CoverLetterStyle s) {
    static const std::vector<std::string> genericFlavor = {
        "  (Cover letter opens: 'I am writing to express my interest in this exciting opportunity.')",
        "  (Cover letter references 'leveraging synergies' in paragraph two.)",
        "  (You forgot to replace [COMPANY NAME] in one place. It shipped.)",
        "  (The template was downloaded in 2019. It shows.)",
    };
    static const std::vector<std::string> tailoredFlavor = {
        "  (Cover letter mentions their last product launch by name. Took 20 minutes.)",
        "  (You referenced the CEO's recent blog post. Risky. Might pay off.)",
        "  (Letter is specific, concise, and honest. Rare combination.)",
        "  (You mentioned a specific pain point from their job description. Solid.)",
    };
    static const std::vector<std::string> aiFlavorText = {
        "  (Cover letter is well-structured and completely soulless.)",
        "  (The recruiter has seen this exact letter 40 times this week. Different name.)",
        "  (Phrases like 'dynamic and fast-paced environment' appear twice.)",
        "  (It's fine. It gets the job done. That's all you can say.)",
    };

    switch (s) {
        case CoverLetterStyle::Generic:
            std::cout << DIM << pickRandom(genericFlavor) << "\n" << RESET;
            break;
        case CoverLetterStyle::Tailored:
            std::cout << DIM << pickRandom(tailoredFlavor) << "\n" << RESET;
            break;
        case CoverLetterStyle::ChatGPTd:
            std::cout << DIM << pickRandom(aiFlavorText) << "\n" << RESET;
            break;
        default: break;
    }
}
