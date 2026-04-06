#pragma once

#include <string>
#include <vector>
#include "player.h"
#include "utils.h"

// Company tiers. Determines unlock requirements and base prestige of the role.
// Tier 1 = always available. Tier 4 = FAANG, requires prestige >= 2.
enum class CompanyTier {
    Startup  = 1,   // always available
    Growth   = 2,   // unlocked at prestige >= 1
    Late     = 3,   // unlocked at prestige >= 2
    FAANG    = 4    // unlocked at prestige >= 2 AND reputation >= 60
};

struct Company {
    std::string  name;
    CompanyTier  tier;
};

// Full company pool. Add entries here to expand the game's company list.
// Unlocks are checked in getAvailableCompanies() based on player state.
inline std::vector<Company> ALL_COMPANIES = {
    // Tier 1: always available
    { "Supabase",       CompanyTier::Startup },
    { "Clerk",          CompanyTier::Startup },
    { "Replit",         CompanyTier::Startup },
    { "Descript",       CompanyTier::Startup },
    { "Loom",           CompanyTier::Startup },
    { "Warpcast",       CompanyTier::Startup },
    { "Modal",          CompanyTier::Startup },
    { "Fly.io",         CompanyTier::Startup },
    { "PlanetScale",    CompanyTier::Startup },
    { "Railway",        CompanyTier::Startup },
    { "Resend",         CompanyTier::Startup },
    { "Alchemy",        CompanyTier::Startup },

    // Tier 2: unlocked at prestige >= 1
    { "Rippling",       CompanyTier::Growth },
    { "Linear",         CompanyTier::Growth },
    { "Retool",         CompanyTier::Growth },
    { "Vercel",         CompanyTier::Growth },
    { "Notion",         CompanyTier::Growth },
    { "Superhuman",     CompanyTier::Growth },
    { "Brex",           CompanyTier::Growth },
    { "Ramp",           CompanyTier::Growth },
    { "Mercury",        CompanyTier::Growth },
    { "Plaid",          CompanyTier::Growth },
    { "Deel",           CompanyTier::Growth },
    { "Gusto",          CompanyTier::Growth },

    // Tier 3: unlocked at prestige >= 2
    { "Figma",          CompanyTier::Late },
    { "Scale AI",       CompanyTier::Late },
    { "Lattice",        CompanyTier::Late },
    { "Stripe",         CompanyTier::Late },
    { "Coinbase",       CompanyTier::Late },
    { "OpenAI",         CompanyTier::Late },

    // Tier 4: FAANG, requires prestige >= 2 AND reputation >= 60
    { "Google",         CompanyTier::FAANG },
    { "Meta",           CompanyTier::FAANG },
    { "Apple",          CompanyTier::FAANG },
    { "Amazon",         CompanyTier::FAANG },
    { "Microsoft",      CompanyTier::FAANG },
    { "Netflix",        CompanyTier::FAANG },
};

// Role titles. Tier-appropriate roles are just cosmetic here;
// all roles use the same interview pipeline.
inline std::vector<std::string> ALL_ROLES = {
    "Founding Engineer", "Full Stack Engineer", "iOS Engineer",
    "Backend Engineer",  "Staff Engineer",      "Senior Software Engineer",
    "Platform Engineer", "Growth Engineer",     "API Developer",
    "Solutions Engineer","ML Engineer",         "Developer Advocate",
    "Infrastructure Engineer", "Security Engineer", "DevEx Engineer"
};

// Returns the subset of companies available to this player right now.
// Called each time the player applies so unlocks feel earned in real time.
inline std::vector<Company> getAvailableCompanies(const Player& p) {
    std::vector<Company> pool;
    for (const auto& c : ALL_COMPANIES) {
        if (c.tier == CompanyTier::Startup) {
            pool.push_back(c);
        } else if (c.tier == CompanyTier::Growth && p.prestige >= 1) {
            pool.push_back(c);
        } else if (c.tier == CompanyTier::Late && p.prestige >= 2) {
            pool.push_back(c);
        } else if (c.tier == CompanyTier::FAANG && p.prestige >= 2 && p.reputation >= 60) {
            pool.push_back(c);
        }
    }
    return pool;
}

// Prints a notice if the player just unlocked new companies.
// Call this after prestige_up() so the player knows what opened up.
inline void printUnlockNotice(const Player& p) {
    if (p.prestige == 1) {
        std::cout << BGREEN << "  >> Prestige 1: Growth-stage companies unlocked.\n" << RESET;
        std::cout << DIM << "     Rippling, Ramp, Vercel, Notion and more are now available.\n" << RESET;
    } else if (p.prestige == 2) {
        std::cout << BGREEN << "  >> Prestige 2: Late-stage and FAANG unlocked.\n" << RESET;
        std::cout << DIM << "     Stripe, Figma, OpenAI, Google, Meta, Apple and more.\n" << RESET;
        std::cout << DIM << "     FAANG requires reputation >= 60.\n" << RESET;
    }
}
