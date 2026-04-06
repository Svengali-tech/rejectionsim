#pragma once

#include <iostream>
#include <vector>
#include <string>
#include "colors.h"
#include "utils.h"

// Every pipeline stage that hasn't resolved yet lives in the WaitQueue.
// On each [s] sleep, daysUntilReady ticks down. When it hits zero the stage
// resolves -- pass or fail -- and the result is printed as if it just arrived
// in your inbox. This makes the game feel like real job searching where you
// apply, then wait, then get news days later instead of all at once.

// Which stage of the pipeline this pending entry represents.
enum class PipelineStage {
    PhoneScreen,
    Technical,
    FinalRound,
    ReferenceCheck,
    Offer
};

inline std::string stageName(PipelineStage s) {
    switch (s) {
        case PipelineStage::PhoneScreen:    return "Phone Screen";
        case PipelineStage::Technical:      return "Technical Interview";
        case PipelineStage::FinalRound:     return "Final Round";
        case PipelineStage::ReferenceCheck: return "Reference Check";
        case PipelineStage::Offer:          return "Offer";
    }
    return "Unknown";
}

// One pending pipeline entry. Stored in WaitQueue until daysUntilReady == 0.
struct PendingEntry {
    int           id;             // unique ID for this entry
    int           appId;          // links back to a JobBoard application
    std::string   company;
    std::string   role;
    std::string   track;          // "SWE", "Finance", "Consulting"
    PipelineStage stage;
    int           daysUntilReady; // counts down on sleep; resolves when 0
    int           passRate;       // effective pass rate baked in at queue time
    int           base;           // salary if this is an Offer stage ($k)
    bool          active = true;
};

// How many in-game days each stage typically takes before resolving.
// Finance and consulting run slower than SWE.
inline int waitDaysForStage(PipelineStage stage, const std::string& track) {
    bool isFinance     = (track == "Finance");
    bool isConsulting  = (track == "Consulting");
    bool isSlow        = isFinance || isConsulting;

    switch (stage) {
        case PipelineStage::PhoneScreen:
            return isSlow ? randRange(1, 3) : randRange(0, 2);
        case PipelineStage::Technical:
            return isSlow ? randRange(2, 4) : randRange(1, 3);
        case PipelineStage::FinalRound:
            return isSlow ? randRange(3, 5) : randRange(2, 4);
        case PipelineStage::ReferenceCheck:
            return randRange(1, 3);
        case PipelineStage::Offer:
            return randRange(1, 2);
    }
    return 1;
}

// The queue itself. One instance on Player. Holds all unresolved pipeline steps.
struct WaitQueue {
    std::vector<PendingEntry> entries;
    int nextId = 1;

    // Adds a new pending stage. Returns the ID assigned.
    int enqueue(int appId, const std::string& company, const std::string& role,
                const std::string& track, PipelineStage stage,
                int passRate, int base = 0) {
        int days = waitDaysForStage(stage, track);
        entries.push_back({
            nextId++, appId, company, role, track,
            stage, days, passRate, base, true
        });
        return nextId - 1;
    }

    // Returns true if any entries are currently waiting.
    bool hasPending() const {
        for (const auto& e : entries)
            if (e.active && e.daysUntilReady > 0) return true;
        return false;
    }

    // Ticks all active entries down by one day.
    // Returns a list of entries that just became ready (daysUntilReady hit 0).
    std::vector<PendingEntry> tick() {
        std::vector<PendingEntry> ready;
        for (auto& e : entries) {
            if (!e.active) continue;
            if (e.daysUntilReady > 0) e.daysUntilReady--;
            if (e.daysUntilReady == 0) {
                ready.push_back(e);
                e.active = false;   // mark resolved
            }
        }
        return ready;
    }

    // Clears all resolved (inactive) entries to keep the list tidy.
    void compact() {
        entries.erase(
            std::remove_if(entries.begin(), entries.end(),
                           [](const PendingEntry& e){ return !e.active; }),
            entries.end()
        );
    }
};

// Prints a summary of what's currently in the queue. Called in HUD if non-empty.
inline void printPendingNotice(const WaitQueue& q) {
    int count = 0;
    for (const auto& e : q.entries)
        if (e.active && e.daysUntilReady > 0) count++;

    if (count == 0) return;
    std::cout << BCYAN << "Pending: " << count << " pipeline stage(s) waiting to resolve. Sleep to advance.\n" << RESET;
}

// Prints the full pending queue as a table. Called from [v]iew board.
inline void printPendingQueue(const WaitQueue& q, int currentDay) {
    bool any = false;
    for (const auto& e : q.entries)
        if (e.active && e.daysUntilReady > 0) { any = true; break; }

    if (!any) return;

    std::cout << "\n" << BOLD << "  -- Pending Stages --\n\n" << RESET;
    std::cout << DIM << "  " << std::left
              << std::setw(20) << "Company"
              << std::setw(22) << "Stage"
              << std::setw(8)  << "Track"
              << "Resolves in\n"
              << "  " << std::string(55, '-') << "\n" << RESET;

    for (const auto& e : q.entries) {
        if (!e.active || e.daysUntilReady <= 0) continue;
        std::cout << "  "
                  << std::left << std::setw(20) << e.company.substr(0, 19)
                  << std::setw(22) << stageName(e.stage)
                  << std::setw(8)  << e.track;
        std::cout << YELLOW << e.daysUntilReady << " day(s)" << RESET << "\n";
    }
    std::cout << "\n";
}

// Pass/fail flavor text for each stage when it resolves.
// Kept here so the resolution logic in main.cpp stays clean.

inline std::string waitPassMsg(PipelineStage stage) {
    static const std::vector<std::string> phonePass = {
        "  After a few days they finally replied. You passed the phone screen.",
        "  Recruiter got back to you. 'Moving forward.' Four days later.",
        "  They said you were 'a strong communicator.' It took them 3 days to decide that.",
    };
    static const std::vector<std::string> techPass = {
        "  Technical feedback came back positive. 'Moving to next round.'",
        "  They liked your solution. It took them a week to say so.",
        "  'Strong technical performance.' Three business days to deliver that sentence.",
    };
    static const std::vector<std::string> finalPass = {
        "  Panel feedback was positive. Offer coming.",
        "  'We'd like to move forward.' You've been waiting 4 days for those words.",
        "  They said the team loved you. It took a long weekend to figure that out.",
    };
    switch (stage) {
        case PipelineStage::PhoneScreen: return pickRandom(phonePass);
        case PipelineStage::Technical:   return pickRandom(techPass);
        case PipelineStage::FinalRound:  return pickRandom(finalPass);
        default: return "  Stage cleared.";
    }
}

inline std::string waitFailMsg(PipelineStage stage) {
    static const std::vector<std::string> phoneFail = {
        "  Three days of silence then: 'We've decided not to move forward.'",
        "  They ghosted for a week then sent a form rejection.",
        "  Portal updated to 'No longer under consideration.' No email.",
    };
    static const std::vector<std::string> techFail = {
        "  Feedback after 5 days: 'We went with stronger technical candidates.'",
        "  They waited 6 days to say 'not the right fit.'",
        "  A week passed. Then: 'We've decided to move forward with other candidates.'",
    };
    static const std::vector<std::string> finalFail = {
        "  Four days after the final round: 'We went with another candidate.'",
        "  They took a week to tell you they chose someone else.",
        "  'After deliberation.' It took them 5 days to deliberate.",
    };
    switch (stage) {
        case PipelineStage::PhoneScreen: return pickRandom(phoneFail);
        case PipelineStage::Technical:   return pickRandom(techFail);
        case PipelineStage::FinalRound:  return pickRandom(finalFail);
        default: return "  Stage failed.";
    }
}
