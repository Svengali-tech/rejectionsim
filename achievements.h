#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include "colors.h"
#include "utils.h"

static const std::string ACH_FILE = "rsim_achievements.txt";

// Every achievement has a unique ID (used in the save file), a display name,
// and a description. IDs must be stable -- never reuse or change them.
struct Achievement {
    std::string id;
    std::string name;
    std::string desc;
    bool        unlocked = false;
};

// Master list of all achievements.
// Add new ones at the bottom -- never change existing IDs.
inline std::vector<Achievement> ALL_ACHIEVEMENTS = {
    { "first_app",       "Sent It",              "Sent your first application."                         },
    { "first_interview", "They Called Back",      "Entered your first interview pipeline."               },
    { "first_offer",     "It Happened",           "Received your first offer."                           },
    { "offer_rescinded", "Negotiated Wrong",      "Had an offer rescinded mid-negotiation."              },
    { "apps_100",        "Century",               "Applied to 100 jobs."                                 },
    { "apps_200",        "This Is Fine",          "Applied to 200 jobs."                                 },
    { "prestige_1",      "Traded Up",             "Reached prestige level 1."                            },
    { "prestige_3",      "Serial Accepter",       "Reached prestige level 3."                            },
    { "principal",       "Principal Energy",      "Reached Principal tier."                              },
    { "faang_offer",     "The Dream",             "Received an offer from a FAANG company."              },
    { "streak_30",       "30 Straight",           "Hit a rejection streak of 30."                        },
    { "streak_50",       "Built Different",       "Hit a rejection streak of 50."                        },
    { "ghost_20",        "Nobody Home",           "Got ghosted 20 times."                                },
    { "ats_10",          "Keyword Density",       "Got ATS-filtered 10 times."                           },
    { "hiring_freeze",   "Wrong Time",            "Applied during a hiring freeze season."               },
    { "bull_offer",      "Timing Is Everything",  "Got an offer during a bull market."                   },
    { "burnout_crispy",  "Cooked",                "Reached Crispy burnout level."                        },
    { "laid_off",        "Restructured",          "Got laid off after accepting an offer."               },
    { "referral_offer",  "It's Who You Know",     "Got an offer while a referral was active."            },
    { "all_3_tracks",    "Renaissance Candidate", "Applied to SWE, Finance, and Consulting roles."       },
    { "cover_tailored",  "Actually Tried",        "Used a tailored cover letter."                        },
    { "cover_rescind",   "AI Detected",           "Had an AI-generated cover letter called out."         },
    { "networked_20",    "Schmoozer",             "Networked 20 times."                                  },
    { "skills_maxed",    "Min-Maxed",             "Maxed out all three skills."                          },
    { "prep_offer",      "Prepared This Time",    "Got an offer while interview prep was active."        },
    { "comp_offer",      "Leverage",              "Had two offers at the same time."                     },
    { "survived_bear",   "Bear Market Survivor",  "Got an offer during a bear market."                   },
    { "nightmare_offer", "Nightmare Cleared",     "Got an offer on Nightmare difficulty."                },
    { "vent_10",         "Coped",                 "Used [vent] 10 times."                                },
    { "hope_zero",       "Empty",                 "Hit 0% hope and kept playing."                        },
};

// Loads unlocked achievement IDs from the persistent file into the master list.
void loadAchievements() {
    std::ifstream f(ACH_FILE);
    if (!f.is_open()) return;

    std::string id;
    while (std::getline(f, id)) {
        if (id.empty()) continue;
        for (auto& a : ALL_ACHIEVEMENTS) {
            if (a.id == id) { a.unlocked = true; break; }
        }
    }
    f.close();
}

// Saves the currently unlocked achievement IDs to the persistent file.
void saveAchievements() {
    std::ofstream f(ACH_FILE);
    if (!f.is_open()) return;
    for (const auto& a : ALL_ACHIEVEMENTS)
        if (a.unlocked) f << a.id << "\n";
    f.close();
}

// Unlocks an achievement by ID if not already unlocked.
// Returns true if newly unlocked (so caller can print the notification).
bool unlockAchievement(const std::string& id) {
    for (auto& a : ALL_ACHIEVEMENTS) {
        if (a.id == id && !a.unlocked) {
            a.unlocked = true;
            saveAchievements();
            return true;
        }
    }
    return false;
}

// Prints the unlock notification inline. Call immediately after unlockAchievement returns true.
inline void printAchievementUnlock(const std::string& id) {
    for (const auto& a : ALL_ACHIEVEMENTS) {
        if (a.id != id) continue;
        std::cout << "\n" << BMAGENTA << BOLD
                  << "  [ACHIEVEMENT UNLOCKED] " << a.name << "\n" << RESET;
        std::cout << DIM << "  " << a.desc << "\n\n" << RESET;
        return;
    }
}

// Helper: unlock + print in one call.
inline void award(const std::string& id) {
    if (unlockAchievement(id)) printAchievementUnlock(id);
}

// Prints the full achievement list. Locked ones shown as ???.
void printAchievements() {
    loadAchievements();   // refresh from disk before displaying

    int unlocked = 0;
    for (const auto& a : ALL_ACHIEVEMENTS) if (a.unlocked) unlocked++;

    std::cout << "\n" << BOLD << "  -- Achievements (" << unlocked
              << "/" << ALL_ACHIEVEMENTS.size() << ") --\n\n" << RESET;

    for (const auto& a : ALL_ACHIEVEMENTS) {
        if (a.unlocked) {
            std::cout << BGREEN << "  [X] " << RESET
                      << BOLD << std::left << std::setw(26) << a.name << RESET
                      << DIM  << a.desc << "\n" << RESET;
        } else {
            std::cout << DIM   << "  [ ] "
                      << std::left << std::setw(26) << "???"
                      << "???\n" << RESET;
        }
    }
    std::cout << "\n";
}
