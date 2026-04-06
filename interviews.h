#pragma once

#include <iostream>
#include <string>
#include <vector>
#include "colors.h"
#include "utils.h"
#include "player.h"
#include "display.h"

// Base pass rates for each interview stage (before modifiers).
// These get adjusted by player tier, season, and referral in player.effectiveRate().
static const int RATE_PHONE     = 55;
static const int RATE_TECHNICAL = 45;
static const int RATE_FINAL     = 35;
static const int RATE_ATS       = 80;  // % chance of clearing ATS (failing = auto-reject)

// ---- ATS FILTER ---------------------------------------------------------------
// Auto-rejects some applications before a human ever sees them.
// Returns true if the application cleared ATS, false if filtered out.
bool runATSFilter(Player& p, const std::string& co) {
    int rate = p.effectiveRate(RATE_ATS);
    bool cleared = (randRange(1, 100) <= rate);

    if (!cleared) {
        p.atsFailed++;
        p.hope = clamp(p.hope - 5, 0, 100);

        static const std::vector<std::string> msgs = {
            "  Auto-rejected. A human never saw your name.",
            "  ATS flagged 'insufficient keyword density.' You had all the keywords.",
            "  Filtered out. The job required 5 years of a framework released 3 years ago.",
            "  Resume score: 62/100. Cutoff was 63. Not kidding.",
            "  ATS rejected you for not having a degree listed in the right field on the form.",
            "  Position filled by internal referral. Still went through the ATS though.",
        };

        printInboxTag("ATS FILTERED", DIM);
        std::cout << pickRandom(msgs) << "\n";
    }
    return cleared;
}

// ---- PHONE SCREEN -------------------------------------------------------------
// First human contact. High pass rate but condescending feedback on fail.
// Returns true if passed.
bool runPhoneScreen(Player& p, const std::string& co) {
    static const std::vector<std::string> passMsgs = {
        "  Recruiter says you seem 'great'. You will never hear that word again.",
        "  Passed. They said you were 'a strong communicator.' Low bar.",
        "  Made it through. The recruiter laughed at your joke. Once.",
        "  They liked you enough to waste more of your time. Congrats.",
        "  'Great culture fit so far.' Means nothing. Moving on.",
    };
    static const std::vector<std::string> failMsgs = {
        "  'We'll be in touch.' They were not.",
        "  You mentioned salary expectations. Call ended shortly after.",
        "  You said you knew their stack. They asked a follow-up. Silence.",
        "  The recruiter had another call. You were not rescheduled.",
        "  They said the role 'might be changing.' It wasn't. You just failed.",
        "  Recruiter ghosted the debrief. You found out via portal status.",
    };

    printInboxTag("PHONE SCREEN", YELLOW);
    std::cout << DIM << "  " << co << " wants 30 minutes of your life.\n" << RESET;
    sleepMs(500);

    bool passed = (randRange(1, 100) <= p.effectiveRate(RATE_PHONE));
    if (passed) {
        std::cout << GREEN << "  >> PASSED\n" << RESET;
        std::cout << pickRandom(passMsgs) << "\n";
        p.reputationUp(2);
    } else {
        std::cout << RED << "  >> FAILED\n" << RESET;
        std::cout << pickRandom(failMsgs) << "\n";
        p.hope = clamp(p.hope - 15, 0, 100);
    }
    return passed;
}

// ---- COMPETING APPLICANTS -----------------------------------------------------
// Occasionally surfaces mid-pipeline to remind you that you're not alone.
// Called between technical and final rounds.
void maybeShowCompetingApplicant() {
    if (randDouble() > 0.40) return;   // 40% chance to appear

    static const std::vector<std::string> msgs = {
        "  (The other finalist went to Stanford. You did not.)",
        "  (Your competition has a Y Combinator startup on their resume.)",
        "  (They're also interviewing someone internal. Classic.)",
        "  (The other candidate knows the hiring manager from a previous job.)",
        "  (Your competition has 3 open source projects with 10k stars. Each.)",
        "  (Someone from a FAANG submitted yesterday. Just so you know.)",
        "  (The hiring manager's college roommate also applied.)",
    };

    std::cout << DIM << "\n" << pickRandom(msgs) << "\n" << RESET;
}

// ---- TECHNICAL ROUND ----------------------------------------------------------
// Leetcode, system design, whiteboard. Pass rate lower than phone screen.
// Returns true if passed.
bool runTechnical(Player& p, const std::string& co) {
    (void)co;
    static const std::vector<std::string> questions = {
        "  Q: Reverse a linked list. You blanked for 4 seconds.",
        "  Q: Two Sum. You did it in O(n^2). They watched the whole time.",
        "  Q: Design a URL shortener. You forgot about databases briefly.",
        "  Q: Binary search. You got it. They didn't look impressed.",
        "  Q: Implement a trie. In JavaScript. On a whiteboard. Timer running.",
        "  Q: 'Walk me through your system design approach.' You said 'it depends.' Twice.",
        "  Q: Dijkstra's algorithm. From scratch. No hints.",
        "  Q: Merge K sorted lists. You said you'd use a heap. They said 'go ahead.'",
        "  Q: LRU cache. You got the logic right. The syntax was a mess.",
        "  Q: 'How would you design Twitter?' You forgot about the fanout problem.",
    };
    static const std::vector<std::string> passMsgs = {
        "  Interviewer: 'Not bad.' That's it. That's all you get.",
        "  Passed. They said they'd 'follow up soon.' It took 11 days.",
        "  Solution was 'acceptable.' Frame it.",
        "  Got through it. The silence at the end was normal. Probably.",
        "  'We'll move you forward.' Said with the enthusiasm of reading a lease.",
    };
    static const std::vector<std::string> failMsgs = {
        "  Feedback: 'Didn't demonstrate strong problem solving skills.'",
        "  Feedback: 'Looking for a more optimal solution.' You had O(n log n).",
        "  Feedback: 'Communication could be clearer.' You explained everything.",
        "  No feedback. Portal updated to 'position filled.'",
        "  Feedback: 'Strong candidate but not the right fit.' No further detail.",
        "  They said feedback was coming. It never came.",
        "  The interviewer was 22. He didn't explain what was wrong. Just shook his head.",
    };

    printInboxTag("TECHNICAL INTERVIEW", YELLOW);
    std::cout << pickRandom(questions) << "\n";
    sleepMs(600);

    bool passed = (randRange(1, 100) <= p.effectiveRate(RATE_TECHNICAL));
    if (passed) {
        std::cout << GREEN << "  >> PASSED\n" << RESET;
        std::cout << pickRandom(passMsgs) << "\n";
        p.reputationUp(3);
    } else {
        std::cout << RED << "  >> FAILED\n" << RESET;
        std::cout << pickRandom(failMsgs) << "\n";
        p.hope = clamp(p.hope - 20, 0, 100);
    }
    return passed;
}

// ---- REFERENCE CHECK ----------------------------------------------------------
// Happens between final round and offer. Sometimes your reference says something weird.
// Returns false if the reference torpedoed the offer.
bool runReferenceCheck(Player& p) {
    static const std::vector<std::string> goodRefs = {
        "  Reference said you were 'one of the best they've worked with.'",
        "  Reference check clean. They said 'hire immediately.'",
        "  Reference was glowing. You have no idea what they said specifically.",
    };
    static const std::vector<std::string> weirdRefs = {
        "  Your reference paused for 3 seconds before answering 'would I hire them again?'",
        "  Reference said you were 'very... passionate.' Long pause before 'passionate.'",
        "  Reference mentioned 'a situation in Q3' and then said they couldn't elaborate.",
        "  Reference was enthusiastic but mentioned you 'work better independently.'",
        "  Reference called you 'a real character.' Hiring manager's tone shifted.",
    };
    static const std::vector<std::string> badRefs = {
        "  Reference accidentally mentioned you left on bad terms. Offer withdrawn.",
        "  Reference said 'I'd rather not comment on that.' Offer withdrawn.",
        "  Reference check failed. Company wouldn't say why. You can guess.",
    };

    std::cout << DIM << "\n  [Reference check in progress...]\n" << RESET;
    sleepMs(700);

    int roll = randRange(1, 100);
    if (roll <= 60) {
        // Good
        std::cout << GREEN << pickRandom(goodRefs) << RESET << "\n";
        return true;
    } else if (roll <= 85) {
        // Weird but survivable
        std::cout << YELLOW << pickRandom(weirdRefs) << RESET << "\n";
        p.hope = clamp(p.hope - 5, 0, 100);
        return true;
    } else {
        // Bad
        std::cout << RED << pickRandom(badRefs) << RESET << "\n";
        p.hope = clamp(p.hope - 30, 0, 100);
        p.reputationDown(10);
        return false;
    }
}

// ---- FINAL ROUND --------------------------------------------------------------
// Four interviewers, two hours, behavioral questions you've answered 200 times.
// Returns true if passed.
bool runFinalRound(Player& p, const std::string& co) {
    static const std::vector<std::string> questions = {
        "  'Tell me about a time you failed.' You picked the wrong story.",
        "  'Where do you see yourself in 5 years?' You said 'here.' Too eager.",
        "  'What's your biggest weakness?' You said 'perfectionism.' They wrote something down.",
        "  'Why do you want to work here?' You mentioned the mission. They nodded vaguely.",
        "  'How do you handle conflict?' Your answer was too honest.",
        "  Four interviewers back to back. Last one asked the same question as the first.",
        "  'Tell me about a time you disagreed with your manager.' You told the truth.",
        "  'What does success look like to you?' You said 'shipping.' They said 'interesting.'",
    };
    static const std::vector<std::string> passMsgs = {
        "  Panel says you were 'impressive.' Decision coming 'end of week.'",
        "  They said 'we'll be in touch very soon.' Meant nothing. Still passed.",
        "  Hiring manager shook your hand twice. Means nothing. But you passed.",
        "  One panelist said 'I'd work with you.' The other said nothing. You passed.",
    };
    static const std::vector<std::string> failMsgs = {
        "  'Went with someone whose experience was a slightly better match.'",
        "  'Strong candidate pool. We had a tough decision.'",
        "  'Decided to go in a different direction.'",
        "  'Role put on hold due to budget.' (Reposted 3 weeks later.)",
        "  'After deliberation, another candidate.' No further details.",
        "  Made it to the final round. Lost to someone's college roommate.",
        "  'Culture fit wasn't quite right.' Four rounds for that.",
    };

    printInboxTag("FINAL ROUND", MAGENTA);
    std::cout << "  " << co << " brings you in. Four people. Two hours.\n";
    std::cout << pickRandom(questions) << "\n";
    sleepMs(800);

    maybeShowCompetingApplicant();

    bool passed = (randRange(1, 100) <= p.effectiveRate(RATE_FINAL));
    if (passed) {
        std::cout << "\n" << GREEN << "  >> PASSED\n" << RESET;
        std::cout << pickRandom(passMsgs) << "\n";
        p.reputationUp(5);
    } else {
        std::cout << "\n" << RED << "  >> FAILED\n" << RESET;
        std::cout << pickRandom(failMsgs) << "\n";
        std::cout << DIM << "  Made it to the final round and still lost. "
                  << "That's a special kind of pain.\n" << RESET;
        p.hope = clamp(p.hope - 25, 0, 100);
    }
    return passed;
}
