#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include "player.h"
#include "colors.h"

// File paths. Stored next to the executable by default.
static const std::string SAVE_FILE        = "rsim_save.txt";
static const std::string LEADERBOARD_FILE = "rsim_leaderboard.txt";

// ---- SAVE / LOAD --------------------------------------------------------------
// Simple key=value plaintext format. One field per line.
// This is intentionally not JSON -- no dependencies, easy to hand-edit.

// Saves the full player state to SAVE_FILE.
// Overwrites any existing save. Returns true on success.
bool saveGame(const Player& p) {
    std::ofstream f(SAVE_FILE);
    if (!f.is_open()) return false;

    f << "prestige="       << p.prestige       << "\n";
    f << "tier="           << static_cast<int>(p.tier) << "\n";
    f << "reputation="     << p.reputation     << "\n";
    f << "connections="    << p.connections    << "\n";
    f << "hasReferral="    << p.hasReferral    << "\n";
    f << "hope="           << p.hope           << "\n";
    f << "energy="         << p.energy         << "\n";
    f << "maxEnergy="      << p.maxEnergy      << "\n";
    f << "day="            << p.day            << "\n";
    f << "applied="        << p.applied        << "\n";
    f << "rejected="       << p.rejected       << "\n";
    f << "ghosted="        << p.ghosted        << "\n";
    f << "interviews="     << p.interviews     << "\n";
    f << "offers="         << p.offers         << "\n";
    f << "offersAccepted=" << p.offersAccepted << "\n";
    f << "networked="      << p.networked      << "\n";
    f << "atsFailed="      << p.atsFailed      << "\n";
    f << "season="         << static_cast<int>(p.season) << "\n";
    f << "daysInSeason="   << p.daysInSeason   << "\n";
    f << "fastestOffer="   << p.fastestOffer   << "\n";

    f.close();
    return true;
}

// Loads a saved game into the provided Player struct.
// Returns true if a valid save was found and loaded.
bool loadGame(Player& p) {
    std::ifstream f(SAVE_FILE);
    if (!f.is_open()) return false;

    std::string line;
    while (std::getline(f, line)) {
        // Find the '=' delimiter
        size_t eq = line.find('=');
        if (eq == std::string::npos) continue;

        std::string key = line.substr(0, eq);
        std::string val = line.substr(eq + 1);
        int v = std::stoi(val);

        if      (key == "prestige")       p.prestige       = v;
        else if (key == "tier")           p.tier           = static_cast<ResumeTier>(v);
        else if (key == "reputation")     p.reputation     = v;
        else if (key == "connections")    p.connections    = v;
        else if (key == "hasReferral")    p.hasReferral    = (v != 0);
        else if (key == "hope")           p.hope           = v;
        else if (key == "energy")         p.energy         = v;
        else if (key == "maxEnergy")      p.maxEnergy      = v;
        else if (key == "day")            p.day            = v;
        else if (key == "applied")        p.applied        = v;
        else if (key == "rejected")       p.rejected       = v;
        else if (key == "ghosted")        p.ghosted        = v;
        else if (key == "interviews")     p.interviews     = v;
        else if (key == "offers")         p.offers         = v;
        else if (key == "offersAccepted") p.offersAccepted = v;
        else if (key == "networked")      p.networked      = v;
        else if (key == "atsFailed")      p.atsFailed      = v;
        else if (key == "season")         p.season         = static_cast<Season>(v);
        else if (key == "daysInSeason")   p.daysInSeason   = v;
        else if (key == "fastestOffer")   p.fastestOffer   = v;
    }

    f.close();
    return true;
}

// Deletes the save file so the player starts fresh.
void deleteSave() {
    std::remove(SAVE_FILE.c_str());
}

// ---- LEADERBOARD --------------------------------------------------------------
// Each entry is one line in LEADERBOARD_FILE.
// Format: applied,rejected,offers,fastestOffer,prestige,day
// Sorted by offers desc, then fastest offer asc (fewer days = better).

struct LeaderboardEntry {
    int applied;
    int rejected;
    int offers;
    int fastestOffer;   // day number, -1 = never
    int prestige;
    int day;
};

// Appends the current player run to the leaderboard file.
void submitToLeaderboard(const Player& p) {
    std::ofstream f(LEADERBOARD_FILE, std::ios::app);
    if (!f.is_open()) return;
    f << p.applied      << ","
      << p.rejected     << ","
      << p.offers       << ","
      << p.fastestOffer << ","
      << p.prestige     << ","
      << p.day          << "\n";
    f.close();
}

// Loads all leaderboard entries, sorts them, and prints the top 10.
void printLeaderboard() {
    std::ifstream f(LEADERBOARD_FILE);
    if (!f.is_open()) {
        std::cout << DIM << "  No leaderboard entries yet.\n" << RESET;
        return;
    }

    std::vector<LeaderboardEntry> entries;
    std::string line;
    while (std::getline(f, line)) {
        if (line.empty()) continue;
        LeaderboardEntry e = {};
        // Parse the 6 comma-separated fields
        try {
            size_t p0 = 0, p1;
            auto next = [&]() {
                p1 = line.find(',', p0);
                std::string tok = (p1 == std::string::npos)
                    ? line.substr(p0)
                    : line.substr(p0, p1 - p0);
                p0 = (p1 == std::string::npos) ? line.size() : p1 + 1;
                return std::stoi(tok);
            };
            e.applied       = next();
            e.rejected      = next();
            e.offers        = next();
            e.fastestOffer  = next();
            e.prestige      = next();
            e.day           = next();
            entries.push_back(e);
        } catch (...) {
            // Skip malformed lines
        }
    }
    f.close();

    // Sort: most offers first, then fastest offer, then fewest rejections
    std::sort(entries.begin(), entries.end(), [](const LeaderboardEntry& a, const LeaderboardEntry& b) {
        if (a.offers != b.offers) return a.offers > b.offers;
        if (a.fastestOffer != b.fastestOffer) {
            if (a.fastestOffer < 0) return false;
            if (b.fastestOffer < 0) return true;
            return a.fastestOffer < b.fastestOffer;
        }
        return a.rejected < b.rejected;
    });

    std::cout << "\n" << BOLD << "  -- Leaderboard (Top 10) --\n\n" << RESET;
    std::cout << DIM
              << "  #   Offers  Prestige  FastestOffer  Applied  Rejected  Days\n"
              << "  " << std::string(60, '-') << "\n" << RESET;

    int shown = 0;
    for (const auto& e : entries) {
        if (shown >= 10) break;
        shown++;

        std::string fastest = (e.fastestOffer > 0)
            ? ("Day " + std::to_string(e.fastestOffer))
            : "none";

        printf("  %-3d %-7d %-9d %-13s %-8d %-9d %d\n",
               shown, e.offers, e.prestige,
               fastest.c_str(),
               e.applied, e.rejected, e.day);
    }

    if (shown == 0) {
        std::cout << DIM << "  No entries yet.\n" << RESET;
    }
    std::cout << "\n";
}
