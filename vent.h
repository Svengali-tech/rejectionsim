#pragma once

#include <iostream>
#include <string>
#include <vector>
#include "colors.h"
#include "utils.h"
#include "player.h"

// ---- VENT COMMAND -------------------------------------------------------------
// Does nothing mechanically except restore 5 hope and print something honest.
// Everyone needs this.

void runVent(Player& p, int& ventCount) {
    static const std::vector<std::string> ventMsgs = {
        "  This process is broken and everyone who's been through it knows it.\n"
        "  The interview pipeline was designed by people who already have jobs.",

        "  You are more than your rejection count.\n"
        "  That doesn't make any of this less exhausting.",

        "  The worst part isn't the rejection. It's the silence.\n"
        "  At least a no is a complete sentence.",

        "  Your GitHub is good. Your resume is good. The market is just bad right now.\n"
        "  Those are three separate facts.",

        "  Every person who got a job offer also got rejected a lot.\n"
        "  The difference is mostly timing and luck. That's genuinely true.",

        "  You've been applying for weeks. You're allowed to be tired.\n"
        "  Tired is not the same as done.",

        "  The ATS that filtered you was trained on resumes from people\n"
        "  who are less qualified than you. That's not a metaphor.",

        "  Some of these companies will reach out 6 months from now\n"
        "  when the role opens again. That's not comforting. Just true.",

        "  The recruiter who ghosted you also has a quota and a manager\n"
        "  they're afraid of. Not an excuse. Just context.",

        "  You're still here. That's not nothing.\n"
        "  Actually it's kind of a lot.",
    };

    ventCount++;
    p.hope = clamp(p.hope + 5, 0, 100);

    std::cout << "\n" << DIM << pickRandom(ventMsgs) << "\n\n";
    std::cout << GREEN << "  (+5 hope. You needed that.)\n\n" << RESET;
}

// ---- INTERVIEW FEEDBACK TIERS -------------------------------------------------
// Feedback quality is tied to reputation. Higher rep = more useful feedback.
// Called after a failed interview stage to replace or supplement the standard msg.

enum class FeedbackQuality { None, Vague, Useful };

inline FeedbackQuality getFeedbackQuality(int reputation) {
    if (reputation >= 70) return FeedbackQuality::Useful;
    if (reputation >= 40) return FeedbackQuality::Vague;
    return FeedbackQuality::None;
}

inline void printFeedback(FeedbackQuality q) {
    static const std::vector<std::string> noFeedback = {
        "  (No feedback provided. Portal updated.)",
        "  (They said they'd send feedback. They did not.)",
        "  (Automated email. No human signed it.)",
    };
    static const std::vector<std::string> vagueFeedback = {
        "  Feedback: 'We went with candidates more aligned with our current needs.'",
        "  Feedback: 'Strong candidate pool this cycle.'",
        "  Feedback: 'We felt there were stronger fits for the role at this time.'",
        "  Feedback: 'Your background is impressive but not quite what we need right now.'",
    };
    static const std::vector<std::string> usefulFeedback = {
        "  Feedback: 'Your system design was strong but we wanted deeper infra experience.\n"
        "  Consider focusing on distributed systems projects.'",

        "  Feedback: 'Great culture fit. Technical round was close -- your time complexity\n"
        "  analysis could be sharper. Work on explaining tradeoffs out loud.'",

        "  Feedback: 'Case structure was good but you went to solutions too fast.\n"
        "  Take more time on the framework before diving in.'",

        "  Feedback: 'Your DCF was solid but we wanted stronger LBO intuition.\n"
        "  Practice walking through a full model from scratch.'",

        "  Feedback: 'Really strong in the behavioral rounds. Technical depth on\n"
        "  distributed systems is the gap. Would reapply in 6 months.'",
    };

    switch (q) {
        case FeedbackQuality::None:
            std::cout << DIM << pickRandom(noFeedback) << "\n" << RESET; break;
        case FeedbackQuality::Vague:
            std::cout << DIM << pickRandom(vagueFeedback) << "\n" << RESET; break;
        case FeedbackQuality::Useful:
            std::cout << CYAN << pickRandom(usefulFeedback) << "\n" << RESET; break;
    }
}

// ---- EMAIL SIGNATURES ---------------------------------------------------------
// Each rejection gets a sign-off. Passive aggression scales with stage depth.

inline std::string pickSignOff(int stage) {
    // stage 0 = ATS/flat rejection, 1 = phone, 2 = technical, 3 = final
    static const std::vector<std::string> early  = { "Best,", "Thanks,", "Regards," };
    static const std::vector<std::string> mid    = { "Warm regards,", "All the best,", "Best wishes," };
    static const std::vector<std::string> late   = {
        "We wish you well in your search,",
        "We hope our paths cross again,",
        "All the best on your continued search,",
        "We're rooting for you,"   // this one is the cruelest
    };

    if (stage <= 0) return pickRandom(early);
    if (stage <= 2) return pickRandom(mid);
    return pickRandom(late);
}

// ---- COMPETING OFFER MECHANIC -------------------------------------------------
// If the player has 2+ pending offers, they can use one to negotiate the other.

struct OfferInHand {
    std::string company;
    std::string role;
    int         base;     // $k
    bool        active = true;
};

// Tries to use offerA to negotiate a bump on offerB.
// Returns the salary bump granted (0 if the tactic backfired).
int runCompetingOfferNegotiation(const OfferInHand& other, Player& p) {
    std::cout << "\n" << BBLUE << BOLD << "  [COMPETING OFFER LEVERAGE]\n" << RESET;
    std::cout << "  You tell them you have an offer from " << other.company
              << " at $" << other.base << "k.\n";
    sleepMs(500);

    int roll = randRange(1, 100);
    if (roll <= 55) {
        int bump = randRange(8, 20);
        std::cout << GREEN << "\n  They matched. Base up $" << bump << "k.\n" << RESET;
        std::cout << DIM << "  'We really want you on the team.'\n" << RESET;
        p.reputationUp(3);
        return bump;
    } else if (roll <= 80) {
        std::cout << YELLOW << "\n  They couldn't match base but added equity.\n" << RESET;
        std::cout << DIM << "  RSUs vest in 4 years. Sure.\n" << RESET;
        return 0;
    } else {
        std::cout << RED << "\n  They said 'we understand' and went quiet for 3 days.\n" << RESET;
        std::cout << DIM << "  Offer still open. Technically. They're not thrilled.\n" << RESET;
        p.reputationDown(3);
        return 0;
    }
}

// ---- WAITING MECHANIC ---------------------------------------------------------
// Some applications take multiple days before the next stage.
// A pending pipeline step is queued and resolves on the next [s] sleep.

struct PendingStage {
    int         appId;        // references a JobBoard application
    std::string company;
    std::string role;
    std::string stageName;    // "Phone Screen", "Technical Interview", etc.
    int         daysUntilReady; // counts down on sleep
    bool        active = false;
};

// Returns a wait time (in days) for the given stage.
// Finance and consulting tend to make you wait longer.
inline int stageWaitDays(const std::string& stageName, const std::string& track) {
    // Final rounds and supdays always take a few days
    if (stageName == "Final Round"  ||
        stageName == "Partner Round" ||
        stageName == "Superday")
        return randRange(2, 4);

    // Technical interviews usually 1-2 days
    if (stageName == "Technical Interview" ||
        stageName == "Case Interview"       ||
        stageName == "Technical/Fit Interview")
        return randRange(1, 3);

    // Finance is slow
    if (track == "Finance") return randRange(1, 3);

    return randRange(0, 1);   // SWE tends to move faster
}
