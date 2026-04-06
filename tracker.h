#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include "colors.h"
#include "utils.h"

// Every application the player sends creates one of these entries.
// Status advances as the player progresses through interview stages.
enum class AppStatus {
    Applied,        // sent, no response yet
    ATSFiltered,    // auto-rejected before a human saw it
    PhoneScreen,    // in phone screen stage
    Technical,      // in technical/skills round
    FinalRound,     // in final round
    OfferPending,   // passed final, offer being drafted
    OfferReceived,  // offer in hand
    Rejected,       // rejected at any stage
    Ghosted,        // no response after N days
    Withdrawn       // player declined offer
};

inline std::string statusLabel(AppStatus s) {
    switch (s) {
        case AppStatus::Applied:       return "Applied";
        case AppStatus::ATSFiltered:   return "ATS Filtered";
        case AppStatus::PhoneScreen:   return "Phone Screen";
        case AppStatus::Technical:     return "Technical";
        case AppStatus::FinalRound:    return "Final Round";
        case AppStatus::OfferPending:  return "Offer Pending";
        case AppStatus::OfferReceived: return "OFFER";
        case AppStatus::Rejected:      return "Rejected";
        case AppStatus::Ghosted:       return "Ghosted";
        case AppStatus::Withdrawn:     return "Withdrawn";
    }
    return "Unknown";
}

inline const char* statusColor(AppStatus s) {
    switch (s) {
        case AppStatus::Applied:       return CYAN;
        case AppStatus::ATSFiltered:   return DIM;
        case AppStatus::PhoneScreen:   return YELLOW;
        case AppStatus::Technical:     return YELLOW;
        case AppStatus::FinalRound:    return MAGENTA;
        case AppStatus::OfferPending:  return BGREEN;
        case AppStatus::OfferReceived: return BGREEN;
        case AppStatus::Rejected:      return RED;
        case AppStatus::Ghosted:       return DIM;
        case AppStatus::Withdrawn:     return DIM;
    }
    return RESET;
}

// One row on the job tracking board.
struct Application {
    int         id;
    std::string company;
    std::string role;
    std::string track;      // "SWE", "Finance", "Consulting"
    AppStatus   status;
    int         dayApplied;
    int         dayUpdated;
    std::string lastNote;   // most recent flavor text for this app
};

// The board holds all applications. Pass by reference to apply actions.
struct JobBoard {
    std::vector<Application> apps;
    int nextId = 1;

    // Adds a new application entry and returns a reference to it.
    Application& add(const std::string& company, const std::string& role,
                     const std::string& track, int day) {
        apps.push_back({ nextId++, company, role, track,
                         AppStatus::Applied, day, day, "Sent." });
        return apps.back();
    }

    // Finds an application by ID. Returns nullptr if not found.
    Application* find(int id) {
        for (auto& a : apps)
            if (a.id == id) return &a;
        return nullptr;
    }

    // Updates the status of an application by ID.
    void update(int id, AppStatus status, int day, const std::string& note = "") {
        auto* a = find(id);
        if (!a) return;
        a->status     = status;
        a->dayUpdated = day;
        if (!note.empty()) a->lastNote = note;
    }

    // Returns count of applications currently in active stages (not resolved).
    int activeCount() const {
        int count = 0;
        for (const auto& a : apps) {
            if (a.status != AppStatus::Rejected  &&
                a.status != AppStatus::Ghosted   &&
                a.status != AppStatus::ATSFiltered &&
                a.status != AppStatus::Withdrawn)
                count++;
        }
        return count;
    }
};

// Prints the full job tracking board.
// Active applications first, then resolved ones dimmed out.
void printJobBoard(const JobBoard& board, int currentDay) {
    std::cout << "\n" << BOLD << "  -- Job Tracking Board --\n\n" << RESET;

    if (board.apps.empty()) {
        std::cout << DIM << "  No applications yet. Use [a] to apply.\n\n" << RESET;
        return;
    }

    // Header
    std::cout << DIM
              << "  " << std::left
              << std::setw(4)  << "ID"
              << std::setw(20) << "Company"
              << std::setw(22) << "Role"
              << std::setw(8)  << "Track"
              << std::setw(15) << "Status"
              << std::setw(6)  << "Day"
              << "\n"
              << "  " << std::string(73, '-') << "\n"
              << RESET;

    // Active applications first
    bool hasActive = false;
    for (const auto& a : board.apps) {
        if (a.status == AppStatus::Rejected  ||
            a.status == AppStatus::Ghosted   ||
            a.status == AppStatus::ATSFiltered ||
            a.status == AppStatus::Withdrawn)
            continue;

        hasActive = true;
        int age = currentDay - a.dayUpdated;

        std::cout << "  "
                  << std::left << std::setw(4)  << a.id
                  << std::setw(20) << a.company.substr(0, 19)
                  << std::setw(22) << a.role.substr(0, 21)
                  << std::setw(8)  << a.track
                  << statusColor(a.status) << BOLD
                  << std::setw(15) << statusLabel(a.status)
                  << RESET
                  << DIM << "Day " << a.dayApplied;

        // Warn if an application has been sitting for a while
        if (age >= 5 && a.status == AppStatus::Applied)
            std::cout << "  (silent " << age << "d)";

        std::cout << RESET << "\n";
        std::cout << DIM << "    " << a.lastNote << "\n" << RESET;
    }

    if (!hasActive)
        std::cout << DIM << "  No active applications.\n" << RESET;

    // Divider before resolved
    bool hasResolved = false;
    for (const auto& a : board.apps) {
        if (a.status == AppStatus::Rejected  ||
            a.status == AppStatus::Ghosted   ||
            a.status == AppStatus::ATSFiltered ||
            a.status == AppStatus::Withdrawn) {
            hasResolved = true;
            break;
        }
    }

    if (hasResolved) {
        std::cout << "\n" << DIM << "  -- Resolved --\n" << RESET;
        for (const auto& a : board.apps) {
            if (a.status != AppStatus::Rejected  &&
                a.status != AppStatus::Ghosted   &&
                a.status != AppStatus::ATSFiltered &&
                a.status != AppStatus::Withdrawn)
                continue;

            std::cout << DIM << "  "
                      << std::left << std::setw(4)  << a.id
                      << std::setw(20) << a.company.substr(0, 19)
                      << std::setw(22) << a.role.substr(0, 21)
                      << std::setw(8)  << a.track
                      << std::setw(15) << statusLabel(a.status)
                      << "Day " << a.dayApplied
                      << RESET << "\n";
        }
    }

    std::cout << "\n  " << DIM << board.activeCount() << " active  |  "
              << board.apps.size() << " total\n\n" << RESET;
}
