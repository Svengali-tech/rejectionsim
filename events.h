#pragma once

#include <iostream>
#include <string>
#include <vector>
#include "colors.h"
#include "utils.h"
#include "player.h"

// A market event fires occasionally and has a temporary effect on pass rates.
// Effects last EVENT_DURATION days then expire.
static const int EVENT_DURATION    = 3;   // days an event stays active
static const int EVENT_FIRE_CHANCE = 20;  // % chance per day sleep to trigger a new event

struct MarketEvent {
    std::string headline;   // what the player sees
    int         modifier;   // additive % change to all pass rates (positive or negative)
    int         daysLeft;   // counts down on advanceDay
    bool        active = false;

    void tick() { if (active && daysLeft > 0) daysLeft--; if (daysLeft == 0) active = false; }
};

// The active event slot. Only one event active at a time.
// Stored on Player.
struct EventSlot {
    MarketEvent current;

    bool isActive() const { return current.active; }
    int  modifier() const { return current.active ? current.modifier : 0; }

    void tick() { current.tick(); }

    void trigger(const std::string& headline, int mod) {
        current = { headline, mod, EVENT_DURATION, true };
    }

    void clear() { current.active = false; }
};

// Pool of possible market events.
// Positive modifier = hiring is up. Negative = hiring is down.
struct EventTemplate { std::string headline; int modifier; };

inline std::vector<EventTemplate> ALL_EVENTS = {
    // Good news
    { "VC funding up 40% this quarter. Everyone is hiring again.",              +15 },
    { "Fed cut rates. Tech hiring picking up.",                                 +10 },
    { "Major IPO this week. Sentiment is high.",                                +8  },
    { "AI hype cycle is back. Every company needs an AI engineer.",             +12 },
    { "A major company just announced a huge hiring push.",                     +10 },
    { "Remote work fully normalized. Candidate pool shrinks, your odds improve.",+8 },

    // Bad news
    { "Major tech company announced 12% layoffs. Candidate pool just flooded.", -15 },
    { "Recession fears. Hiring slowing across the board.",                      -12 },
    { "VC funding dried up. Series B startups pausing headcount.",              -10 },
    { "A bank run spooked the market. Finance roles on hold.",                  -18 },
    { "Consulting firms cutting analyst classes by 30% this cycle.",            -12 },
    { "Interest rates hiked again. M&A dried up. Finance hiring frozen.",       -20 },
    { "ChatGPT just got way better. Three companies just cancelled SWE roles.", -10 },
    { "Mass layoffs trending on LinkedIn. Candidate pool is enormous.",         -15 },

    // Neutral / mixed
    { "Q4 hiring push starting early. Some roles accelerating.",                +5  },
    { "End of fiscal year. Budgets being evaluated. Mixed signals.",            -5  },
    { "Mergers announced. Roles in flux. Some open, some frozen.",              0   },
};

// Tries to fire a random market event.
// Only fires if no event is currently active.
// Call this on sleep/day advance.
void maybeTriggerEvent(EventSlot& slot) {
    if (slot.isActive()) return;
    if (randRange(1, 100) > EVENT_FIRE_CHANCE) return;

    const auto& e = ALL_EVENTS[rand() % ALL_EVENTS.size()];
    slot.trigger(e.headline, e.modifier);

    std::cout << "\n" << BYELLOW << BOLD << "  -- Market Event --\n" << RESET;
    std::cout << "  " << e.headline << "\n";

    if (e.modifier > 0)
        std::cout << GREEN << "  Pass rates +" << e.modifier << "% for " << EVENT_DURATION << " days.\n" << RESET;
    else if (e.modifier < 0)
        std::cout << RED << "  Pass rates " << e.modifier << "% for " << EVENT_DURATION << " days.\n" << RESET;
    else
        std::cout << DIM << "  No direct pass rate change. Market is uncertain.\n" << RESET;

    std::cout << "\n";
}

// Prints a reminder if an event is still active. Called in HUD.
inline void printActiveEvent(const EventSlot& slot) {
    if (!slot.isActive()) return;
    const char* col = (slot.current.modifier >= 0) ? GREEN : RED;
    std::cout << col << "Event: " << slot.current.headline
              << " (" << slot.current.daysLeft << "d left, "
              << (slot.current.modifier >= 0 ? "+" : "") << slot.current.modifier << "%)"
              << RESET << "\n";
}
