#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <fstream>

#include "colors.h"
#include "utils.h"
#include "careers.h"
#include "difficulty.h"
#include "player.h"
#include "skills.h"
#include "burnout.h"
#include "streaks.h"
#include "events.h"
#include "coverletter.h"
#include "prep.h"
#include "tracker.h"
#include "layoff.h"
#include "achievements.h"
#include "storymode.h"
#include "vent.h"
#include "networking.h"
#include "interviews.h"
#include "offer.h"
#include "saveload.h"

// ---- DISPLAY ------------------------------------------------------------------



void printHUD(const Player& p, const StoryState& story) {
    std::cout << "\n";
    std::cout << BOLD << "  REJECTION SIMULATOR 3000" << RESET
              << DIM  << "  Day " << p.day << RESET
              << "  [" << tierName(p.tier) << "]"
              << "  [" << trackName(p.track) << "]";
    if (p.prestige>0) std::cout << BMAGENTA << "  Prestige " << p.prestige << RESET;
    if (story.active)  std::cout << RED << "  Debt $" << story.debt << "k" << RESET;
    std::cout << "\n";
    printSeparator();

    std::cout << "Applied:  " << BOLD  << std::setw(4) << p.applied    << RESET
              << "  Rejected: " << RED << BOLD << std::setw(4) << p.rejected << RESET
              << "  Ghosted: "  << DIM << std::setw(4) << p.ghosted    << RESET
              << "  ATS: "      << DIM << std::setw(3) << p.atsFailed  << RESET << "\n";
    std::cout << "Interviews:" << YELLOW << BOLD << std::setw(3) << p.interviews << RESET
              << "  Offers: "  << GREEN  << BOLD << std::setw(4) << p.offers     << RESET
              << "  Networked: "<< BCYAN << std::setw(4) << p.networked          << RESET << "\n";
    std::cout << "Rep: " << std::setw(3) << p.reputation << " (" << p.reputationLabel() << ")";
    if (p.hasReferral) std::cout << "  " << BGREEN << "[REFERRAL READY]" << RESET;
    if (p.prep.active) std::cout << "  " << BBLUE  << "[PREP ACTIVE +" << p.prep.bonus << "%]" << RESET;
    std::cout << "\n";

    // Market + event
    std::cout << "Market: " << DIM << seasonName(p.season) << RESET;
    if (p.eventSlot.isActive()) {
        const char* ec = (p.eventSlot.current.modifier>=0)?GREEN:RED;
        std::cout << "  " << ec << "[Event: "
                  << (p.eventSlot.current.modifier>=0?"+":"")
                  << p.eventSlot.current.modifier << "% "
                  << p.eventSlot.current.daysLeft << "d]" << RESET;
    }
    std::cout << "\n";

    // Burnout
    if (p.burnout.level != BurnoutLevel::Fresh)
        std::cout << burnoutColor(p.burnout.level) << "Burnout: "
                  << burnoutLabel(p.burnout.level)
                  << " (penalty " << burnoutPenalty(p.burnout.level) << "%)"
                  << RESET << "\n";

    // Skills compact line
    std::cout << "Skills: "
              << "Leet Lv" << p.skills.leetcodeLevel << " [" << skillLevelLabel(p.skills.leetcodeLevel) << "]  "
              << "Portfolio Lv" << p.skills.portfolioLevel << " [" << skillLevelLabel(p.skills.portfolioLevel) << "]  "
              << "Clout Lv" << p.skills.cloutLevel << " [" << skillLevelLabel(p.skills.cloutLevel) << "]\n";

    // Streak warning
    if (p.streak.current >= 10)
        std::cout << RED << DIM << "  Streak: " << p.streak.current << " rejections in a row\n" << RESET;

    // Cover letter
    std::cout << "Cover:  " << coverLetterName(p.coverLetter)
              << DIM << "  (bonus " << (coverLetterBonus(p.coverLetter)>=0?"+":"")
              << coverLetterBonus(p.coverLetter) << "% pipeline)\n" << RESET;

    if (story.active) printDebtBar(story);
    printHopeBar(p.hope);
    printEnergyBar(p.energy, p.maxEnergy);
    printSeparator();
}


// ---- APPLY --------------------------------------------------------------------

void runApply(Player& p, JobBoard& board) {
    if (p.energy < 1) {
        std::cout << RED << "  No energy. Sleep ([s]) first.\n" << RESET;
        return;
    }

    // Tailored cover letter costs extra energy
    if (p.coverLetter == CoverLetterStyle::Tailored && p.energy < 2) {
        std::cout << RED << "  Tailored cover letter needs 2 energy. Sleep first.\n" << RESET;
        return;
    }

    int energyCost = (p.coverLetter == CoverLetterStyle::Tailored) ? 2 : 1;
    p.energy -= energyCost;
    p.applied++;

    // Track coverage for achievements
    if (p.track == CareerTrack::SoftwareEng) p.appliedSWE = true;
    if (p.track == CareerTrack::Finance)     p.appliedFin = true;
    if (p.track == CareerTrack::Consulting)  p.appliedCon = true;

    std::string company = pickCompany(p.track, p.prestige);
    std::string role    = pickRole(p.track);
    std::string tname   = (p.track==CareerTrack::Finance?"Finance":
                           p.track==CareerTrack::Consulting?"Consulting":"SWE");

    auto& app = board.add(company, role, tname, p.day);

    std::cout << "\n" << BOLD << "To: " << company << " -- " << role << RESET << "\n";

    // Cover letter flavor
    if (p.coverLetter != CoverLetterStyle::None)
        printCoverLetterFlavor(p.coverLetter);

    // Burnout typo warning
    maybePrintTypoWarning(p.burnout.level);

    sleepMs(350);

    // Burnout grind day tick
    if (p.burnout.onGrindDay())
        printBurnoutWarning(p.burnout.level);

    const auto& cfg = getDifficultyConfig(p.difficulty);
    double roll = randDouble();

    // Pipeline entry threshold adjusted by cover letter bonus and difficulty ghost rate
    double pipelineThreshold = 0.20 + (coverLetterBonus(p.coverLetter) / 100.0);
    double ghostThreshold    = cfg.ghostChance / 100.0;
    double atsThreshold      = cfg.atsRate;

    // Recruiter spam (5%)
    if (roll < 0.05) {
        static const std::vector<std::string> spam = {
            "Hi! Great opportunity -- Junior Dev role. Mostly spreadsheets. $48k. LMK!",
            "Hey, we need a Principal Architect (15 YOE). $55k. Onsite. Tulsa.",
            "Interested in a 3-month contract? No benefits. Hybrid. Parsippany, NJ.",
            "Your background is impressive! We need a manual QA tester.",
            "Exciting stealth startup. Can't share name, stack, or comp. Interested?",
        };
        std::cout << BLUE << BOLD << "[RECRUITER SPAM]\n" << RESET;
        std::cout << "  " << pickRandom(spam) << "\n";
        p.hope = clamp(p.hope+2, 0, 100);
        board.update(app.id, AppStatus::Rejected, p.day, "Recruiter spam.");
        maybeLinkedInPing();
        return;
    }

    // ATS filter
    if (randRange(1,100) > atsThreshold) {
        p.atsFailed++;
        p.hope = clamp(p.hope-5, 0, 100);
        p.streak.onRejection();
        printf("\a");
        static const std::vector<std::string> atsMsgs = {
            "  Auto-rejected. A human never saw your name.",
            "  ATS filtered: insufficient keyword density. You had all the keywords.",
            "  Resume score: 62/100. Cutoff: 63.",
            "  Filtered for not having 5 years of a 3-year-old framework.",
        };
        std::cout << DIM << BOLD << "[ATS FILTERED]\n" << RESET;
        std::cout << pickRandom(atsMsgs) << "\n";
        board.update(app.id, AppStatus::ATSFiltered, p.day);
        printStreakCommentary(p.streak.current);
        maybeLinkedInPing();
        award("ats_10"); // checked inside -- only fires at 10
        return;
    }

    // Ghost
    if (roll < ghostThreshold) {
        p.ghosted++;
        p.hope = clamp(p.hope-8, 0, 100);
        p.streak.onRejection();
        static const std::vector<std::string> ghosts = {
            "No response. Read receipts: off.",
            "Left on read. Classic.",
            "Your follow-up was also ignored.",
            "Job posting quietly deleted.",
            "They reposted the same role the following week.",
            "ATS acknowledged. Human: never.",
            "Status in portal: 'Under Review' for 6 weeks.",
        };
        std::cout << DIM << BOLD << "[GHOSTED]\n" << RESET;
        std::cout << "  " << pickRandom(ghosts) << "  "
                  << DIM << pickSignOff(0) << RESET << "\n";
        board.update(app.id, AppStatus::Ghosted, p.day);
        printStreakCommentary(p.streak.current);
        maybeLinkedInPing();
        return;
    }

    const TrackConfig& tc = getTrackConfig(p.track);

    // Flat rejection
    if (roll < (1.0 - pipelineThreshold)) {
        p.rejected++;
        p.hope = clamp(p.hope-12, 0, 100);
        p.streak.onRejection();
        printf("\a");
        std::cout << RED << BOLD << "[REJECTED]\n" << RESET;
        std::cout << "  " << pickRandom(getTrackRejections(p.track))
                  << "\n  " << DIM << pickSignOff(0) << RESET << "\n";
        board.update(app.id, AppStatus::Rejected, p.day);
        printStreakCommentary(p.streak.current);
        maybeLinkedInPing();
        return;
    }

    // ---- INTERVIEW PIPELINE ----
    p.interviews++;
    p.streak.onProgress();
    p.hope = clamp(p.hope+5, 0, 100);
    bool hadReferral = p.hasReferral;
    p.hasReferral = false;

    // Consume prep bonus
    p.prep.consume();

    std::cout << CYAN << BOLD << "[INTERVIEW PIPELINE]\n" << RESET;
    std::cout << DIM << "  They actually responded. Suspicious.\n\n" << RESET;
    board.update(app.id, AppStatus::PhoneScreen, p.day);
    sleepMs(300);

    // Phone screen
    std::cout << YELLOW << BOLD << "[" << tc.phoneLabel << "]\n" << RESET;
    std::cout << DIM << "  " << company << " wants 30 minutes of your life.\n" << RESET;
    sleepMs(500);
    if (randRange(1,100) > p.effectiveRate(tc.phoneRate)) {
        p.rejected++;
        p.hope = clamp(p.hope-15, 0, 100);
        printf("\a");
        static const std::vector<std::string> pf = {
            "  They said 'we'll be in touch.' They were not.",
            "  You mentioned salary. Call ended shortly after.",
            "  They had another call. You were not rescheduled.",
            "  'Role might be changing.' It wasn't. You just failed.",
        };
        std::cout << RED << "  >> FAILED\n" << RESET << pickRandom(pf) << "\n";
        std::cout << DIM << "  " << pickSignOff(1) << RESET << "\n";
        board.update(app.id, AppStatus::Rejected, p.day);
        printFeedback(getFeedbackQuality(p.reputation));
        goto achievement_check;
    }
    std::cout << GREEN << "  >> PASSED\n" << RESET;
    p.reputationUp(2);
    std::cout << DIM << "  'Strong communicator.' Low bar.\n\n" << RESET;
    board.update(app.id, AppStatus::Technical, p.day);
    sleepMs(400);

    // Technical / case / fit round
    {
        // Pick question pool based on track
        static const std::vector<std::string> sweQ = {
            "  Q: Reverse a linked list. You blanked for 4 seconds.",
            "  Q: Two Sum. You did it in O(n^2). They watched.",
            "  Q: Design a URL shortener. You forgot databases briefly.",
            "  Q: Dijkstra's. From scratch. No hints.",
            "  Q: LRU cache. Logic right. Syntax: a mess.",
        };
        const std::vector<std::string>* qpool =
            (p.track==CareerTrack::Finance)    ? &FINANCE_TECH_QUESTIONS :
            (p.track==CareerTrack::Consulting) ? &CONSULTING_CASE_QUESTIONS : &sweQ;

        std::cout << YELLOW << BOLD << "[" << tc.techLabel << "]\n" << RESET;
        std::cout << pickRandom(*qpool) << "\n";
        sleepMs(600);

        if (randRange(1,100) > p.effectiveRate(tc.techRate)) {
            p.rejected++;
            p.hope = clamp(p.hope-20, 0, 100);
            printf("\a");
            static const std::vector<std::string> tf = {
                "  Feedback: 'Didn't demonstrate strong problem solving.'",
                "  Feedback: 'More optimal solution expected.' You had O(n log n).",
                "  No feedback. Portal: 'position filled.'",
                "  They said feedback was coming. It never came.",
            };
            std::cout << RED << "  >> FAILED\n" << RESET << pickRandom(tf) << "\n";
            std::cout << DIM << "  " << pickSignOff(2) << RESET << "\n";
            board.update(app.id, AppStatus::Rejected, p.day);
            printFeedback(getFeedbackQuality(p.reputation));
            goto achievement_check;
        }
        std::cout << GREEN << "  >> PASSED\n" << RESET;
        p.reputationUp(3);
        std::cout << DIM << "  'Acceptable.' Frame that.\n\n" << RESET;
        board.update(app.id, AppStatus::FinalRound, p.day);
        sleepMs(400);
    }

    // Competing applicant flavor
    maybeShowCompetingApplicant();

    // Final round
    {
        const std::vector<std::string>* fpool =
            (p.track==CareerTrack::Finance)    ? &FINANCE_FINAL_QUESTIONS :
            (p.track==CareerTrack::Consulting) ? &CONSULTING_FINAL_QUESTIONS : nullptr;

        std::cout << MAGENTA << BOLD << "[" << tc.finalLabel << "]\n" << RESET;
        std::cout << "  " << company << ". Four people. Two hours.\n";

        if (fpool) std::cout << pickRandom(*fpool) << "\n";
        else {
            static const std::vector<std::string> sweFinal = {
                "  'Tell me about a time you failed.' You picked the wrong story.",
                "  'Where do you see yourself in 5 years?' You said 'here.' Too eager.",
                "  Four interviewers. Last one asked the same question as the first.",
            };
            std::cout << pickRandom(sweFinal) << "\n";
        }
        sleepMs(800);

        // Cover letter AI detection at final round
        int finalRate = p.effectiveRate(tc.finalRate) + coverLetterFinalRoundPenalty(p.coverLetter);
        if (randRange(1,100) > clamp(finalRate,5,95)) {
            p.rejected++;
            p.hope = clamp(p.hope-25, 0, 100);
            printf("\a");
            static const std::vector<std::string> ff = {
                "  'Went with a slightly stronger match.'",
                "  'Strong candidate pool. Tough decision.'",
                "  'Different direction.' No further detail.",
                "  Made it to the final round and lost to someone's college roommate.",
                "  'Role put on hold.' (Reposted 3 weeks later.)",
            };
            std::cout << RED << "  >> FAILED\n" << RESET << pickRandom(ff) << "\n";
            std::cout << DIM << "  " << pickSignOff(3) << "\n"
                      << "  Made it to the final round and still lost. Special kind of pain.\n" << RESET;
            board.update(app.id, AppStatus::Rejected, p.day);
            printFeedback(getFeedbackQuality(p.reputation));
            goto achievement_check;
        }
        std::cout << GREEN << "  >> PASSED\n" << RESET;
        p.reputationUp(5);
    }

    // Reference check
    {
        std::cout << DIM << "\n  [Reference check...]\n" << RESET;
        sleepMs(600);
        int rr = randRange(1,100);
        if (rr <= 60) {
            std::cout << GREEN << "  Reference was clean.\n" << RESET;
        } else if (rr <= 85) {
            static const std::vector<std::string> weird = {
                "  Reference paused for 3 seconds before answering.",
                "  Reference mentioned 'a situation in Q3' and stopped.",
                "  Reference said you were 'very... passionate.'",
            };
            std::cout << YELLOW << pickRandom(weird) << RESET << "\n";
            p.hope = clamp(p.hope-5,0,100);
        } else {
            std::cout << RED << "  Reference torpedoed it. Offer withdrawn.\n" << RESET;
            p.rejected++;
            p.hope = clamp(p.hope-30,0,100);
            p.reputationDown(10);
            board.update(app.id, AppStatus::Rejected, p.day);
            goto achievement_check;
        }
    }

    // Offer
    board.update(app.id, AppStatus::OfferReceived, p.day, "Offer received.");
    {
        printf("\a");
        // Salary by tier
        int base = randRange(100 + static_cast<int>(p.tier)*30,
                             150 + static_cast<int>(p.tier)*40);
        int eq   = randRange(10 + static_cast<int>(p.tier)*15, 80 + static_cast<int>(p.tier)*30);
        int bon  = randRange(5, 25);

        p.offers++;
        p.hope = clamp(p.hope+40, 0, 100);
        if (p.fastestOffer < 0) p.fastestOffer = p.day;

        std::cout << "\n" << GREEN << BOLD
                  << "  +--------------------------------------------------+\n"
                  << "  |                  OFFER RECEIVED                  |\n"
                  << "  +--------------------------------------------------+\n\n" << RESET;
        std::cout << BOLD << "  " << company << " -- " << role << "\n\n" << RESET;
        std::cout << "  Base:   " << GREEN << BOLD << "$" << base << "k" << RESET << "\n";
        std::cout << "  Equity: " << YELLOW << "$" << eq << "k" << RESET << DIM << " (4yr vest, 1yr cliff)" << RESET << "\n";
        std::cout << "  Bonus:  $" << bon << "k\n\n";

        static const std::vector<std::string> remarks = {
            "  'Competitive for the market.' It isn't.",
            "  'Unlimited PTO.' You will never take any.",
            "  'We move fast here.' You will find out.",
            "  Equity vests in 4 years. A lot can happen in 4 years.",
        };
        std::cout << DIM << pickRandom(remarks) << "\n\n" << RESET;

        // Negotiate?
        if (askYesNo("Negotiate?")) {
            int nr = randRange(1,100);
            if (nr<=40) {
                int bump = randRange(5,15);
                std::cout << GREEN << "\n  Up $" << bump << "k. 'Best we can do.'\n" << RESET;
                std::cout << DIM << "  It wasn't.\n" << RESET;
            } else if (nr<=70) {
                std::cout << YELLOW << "\n  'No base flexibility. We can add equity.'\n" << RESET;
            } else {
                std::cout << RED << "\n  Rescinded.\n" << RESET;
                std::cout << DIM << "  'Expectations not aligned.' Both offers gone.\n" << RESET;
                printf("\a");
                p.offers--;
                p.hope = clamp(p.hope-50,0,100);
                p.reputationDown(5);
                board.update(app.id, AppStatus::Withdrawn, p.day);
                if (unlockAchievement("offer_rescinded")) printAchievementUnlock("offer_rescinded");
                goto achievement_check;
            }
        }

        if (askYesNo("Accept?")) {
            std::cout << GREEN << "\n  Accepted. Start date TBD. Paperwork in 12 days.\n\n" << RESET;
            p.prestige_up();
            // Story mode employment
        } else {
            std::cout << DIM << "\n  Declined. Recruiter said 'no worries' and moved on.\n" << RESET;
            p.reputationUp(2);
            board.update(app.id, AppStatus::Withdrawn, p.day);
        }

        // Achievement checks for offer
        if (unlockAchievement("first_offer")) printAchievementUnlock("first_offer");
        if (hadReferral && unlockAchievement("referral_offer")) printAchievementUnlock("referral_offer");
    }

achievement_check:
    // Standard achievement checks after every apply
    if (p.applied==1   && unlockAchievement("first_app"))        printAchievementUnlock("first_app");
    if (p.interviews==1 && unlockAchievement("first_interview"))  printAchievementUnlock("first_interview");
    if (p.applied==100 && unlockAchievement("apps_100"))          printAchievementUnlock("apps_100");
    if (p.applied==200 && unlockAchievement("apps_200"))          printAchievementUnlock("apps_200");
    if (p.streak.current>=30 && unlockAchievement("streak_30"))   printAchievementUnlock("streak_30");
    if (p.streak.current>=50 && unlockAchievement("streak_50"))   printAchievementUnlock("streak_50");
    if (p.ghosted>=20  && unlockAchievement("ghost_20"))          printAchievementUnlock("ghost_20");
    if (p.atsFailed>=10 && unlockAchievement("ats_10"))           printAchievementUnlock("ats_10");
    if (p.hope<=0      && unlockAchievement("hope_zero"))         printAchievementUnlock("hope_zero");
    if (p.appliedSWE && p.appliedFin && p.appliedCon
        && unlockAchievement("all_3_tracks"))                      printAchievementUnlock("all_3_tracks");
    if (p.burnout.level==BurnoutLevel::Crispy && unlockAchievement("burnout_crispy"))
        printAchievementUnlock("burnout_crispy");
    if (p.season==Season::Freezing && unlockAchievement("hiring_freeze"))
        printAchievementUnlock("hiring_freeze");
    if (p.season==Season::BullMarket && p.offers>0 && unlockAchievement("bull_offer"))
        printAchievementUnlock("bull_offer");
    if ((p.season==Season::BearMarket||p.season==Season::Freezing) && p.offers>0
        && unlockAchievement("survived_bear"))                    printAchievementUnlock("survived_bear");
    if (p.difficulty==Difficulty::Nightmare && p.offers>0 && unlockAchievement("nightmare_offer"))
        printAchievementUnlock("nightmare_offer");
    if (p.skills.leetcodeLevel==5 && p.skills.portfolioLevel==5 && p.skills.cloutLevel==5
        && unlockAchievement("skills_maxed"))                      printAchievementUnlock("skills_maxed");
    if (p.networked>=20 && unlockAchievement("networked_20"))      printAchievementUnlock("networked_20");
    if (p.prestige>=1  && unlockAchievement("prestige_1"))         printAchievementUnlock("prestige_1");
    if (p.prestige>=3  && unlockAchievement("prestige_3"))         printAchievementUnlock("prestige_3");
    if (p.tier==ResumeTier::Principal && unlockAchievement("principal")) printAchievementUnlock("principal");
    if (p.coverLetter==CoverLetterStyle::Tailored && unlockAchievement("cover_tailored"))
        printAchievementUnlock("cover_tailored");

    // Milestones
    static const std::vector<std::pair<int,std::string>> milestones = {
        {5,"5 shots. Warming up."},
        {10,"10 deep. Easy Apply is muscle memory."},
        {20,"20 apps. Cover letter is a template."},
        {30,"30. You have more hiring data than most analysts."},
        {50,"50. Applied to the same company twice by accident."},
        {75,"75. You check the portal before breakfast."},
        {100,"100 applications. A century of effort. LEGENDARY."},
        {150,"150. The number stopped surprising you. That's the scariest part."},
        {200,"200. You could write a book. Nobody would read it."},
    };
    for (const auto& [n,msg] : milestones)
        if (p.applied==n)
            std::cout << "\n" << BYELLOW << BOLD << "  >> " << msg << RESET << "\n";

    if (p.hope<=0)
        std::cout << "\n" << RED << BOLD
                  << "  !! HOPE DEPLETED.\n     But you're still here. That counts.\n" << RESET;

    maybeLinkedInPing();
    std::cout << "\n";
}

// ---- PROFILE ------------------------------------------------------------------

void printProfile(const Player& p) {
    std::cout << "\n" << BOLD << "  -- Profile --\n\n" << RESET;
    std::cout << "  Track:       " << trackName(p.track) << "\n";
    std::cout << "  Tier:        " << tierName(p.tier) << "\n";
    std::cout << "  Prestige:    " << p.prestige << "\n";
    std::cout << "  Reputation:  " << p.reputation << " (" << p.reputationLabel() << ")\n";
    std::cout << "  Difficulty:  " << difficultyName(p.difficulty) << "\n";
    std::cout << "  Cover:       " << coverLetterName(p.coverLetter) << "\n";
    std::cout << "  Connections: " << p.connections << "\n\n";

    std::cout << "  Applied:     " << p.applied    << "\n";
    std::cout << "  ATS fails:   " << p.atsFailed  << "\n";
    std::cout << "  Rejected:    " << p.rejected   << "\n";
    std::cout << "  Ghosted:     " << p.ghosted    << "\n";
    std::cout << "  Interviews:  " << p.interviews << "\n";
    std::cout << "  Offers:      " << p.offers     << "\n";
    std::cout << "  Networked:   " << p.networked  << "\n";
    std::cout << "  Streak:      " << p.streak.current << " (best: " << p.streak.allTime << ")\n\n";

    if (p.applied>0) {
        std::cout << std::fixed << std::setprecision(1);
        std::cout << "  Interview rate: " << 100.f*p.interviews/p.applied << "%\n";
        std::cout << "  Offer rate:     " << 100.f*p.offers/p.applied     << "%\n\n";
    }

    std::cout << BOLD << "  -- Skills --\n\n" << RESET;
    printSkillRow("Leetcode Grind",     p.skills.leetcodeLevel,  p.skills.leetcodeXP);
    printSkillRow("Portfolio Strength", p.skills.portfolioLevel, p.skills.portfolioXP);
    printSkillRow("Network Clout",      p.skills.cloutLevel,     p.skills.cloutXP);
    std::cout << "\n" << DIM
              << "  Pass rate bonuses right now:\n"
              << "    ATS/Phone:   +" << skillBonus(p.skills.portfolioLevel) << "%\n"
              << "    Technical:   +" << skillBonus(p.skills.leetcodeLevel)  << "%\n"
              << "    Final round: +" << skillBonus(p.skills.cloutLevel)     << "%\n"
              << RESET << "\n";
}

// ---- SLEEP --------------------------------------------------------------------

void handleSleep(Player& p, StoryState& story) {
    bool seasonChanged = p.advanceDay();
    maybeTriggerEvent(p.eventSlot);

    std::cout << "\n" << DIM << "  Day " << p.day-1 << " done. Energy refilled.\n" << RESET;

    if (p.burnout.level != BurnoutLevel::Fresh)
        printBurnoutRecovery(p.burnout.level);

    if (seasonChanged) {
        std::cout << "\n" << BYELLOW << BOLD << "  -- Market Update --\n" << RESET;
        std::cout << "  " << seasonName(p.season) << "\n";
        switch(p.season) {
            case Season::BullMarket:
                std::cout << DIM << "  VCs are throwing money at everything.\n" << RESET; break;
            case Season::BearMarket:
                std::cout << DIM << "  Layoff announcements trending. Everyone's posting 'next chapter.'\n" << RESET; break;
            case Season::Freezing:
                std::cout << DIM << "  Hiring freeze memos going out. Roles being pulled mid-process.\n" << RESET; break;
            default: break;
        }
    }

    // Layoff check
    if (shouldLayoff(p)) runLayoffEvent(p);

    // Story mode debt tick
    if (story.active) {
        if (storyTick(story)) {
            printStoryWin(story);
            story.active = false;
        }
    }

    std::cout << "\n  Day " << p.day << " begins.\n\n";
}

// ---- SAVE/LOAD WRAPPERS -------------------------------------------------------
// saveload.h handles the file I/O. These wrap it with extra fields.


void doSave(const Player& p) {
    std::ofstream f(SAVE_FILE);
    if (!f.is_open()) return;
    f<<"prestige="<<p.prestige<<"\n"<<"tier="<<(int)p.tier<<"\n"
     <<"reputation="<<p.reputation<<"\n"<<"connections="<<p.connections<<"\n"
     <<"hasReferral="<<p.hasReferral<<"\n"<<"hope="<<p.hope<<"\n"
     <<"energy="<<p.energy<<"\n"<<"maxEnergy="<<p.maxEnergy<<"\n"
     <<"day="<<p.day<<"\n"<<"applied="<<p.applied<<"\n"
     <<"rejected="<<p.rejected<<"\n"<<"ghosted="<<p.ghosted<<"\n"
     <<"interviews="<<p.interviews<<"\n"<<"offers="<<p.offers<<"\n"
     <<"offersAccepted="<<p.offersAccepted<<"\n"<<"networked="<<p.networked<<"\n"
     <<"atsFailed="<<p.atsFailed<<"\n"<<"ventCount="<<p.ventCount<<"\n"
     <<"season="<<(int)p.season<<"\n"<<"daysInSeason="<<p.daysInSeason<<"\n"
     <<"fastestOffer="<<p.fastestOffer<<"\n"
     <<"leetcodeLevel="<<p.skills.leetcodeLevel<<"\n"
     <<"leetcodeXP="<<p.skills.leetcodeXP<<"\n"
     <<"portfolioLevel="<<p.skills.portfolioLevel<<"\n"
     <<"portfolioXP="<<p.skills.portfolioXP<<"\n"
     <<"cloutLevel="<<p.skills.cloutLevel<<"\n"
     <<"cloutXP="<<p.skills.cloutXP<<"\n"
     <<"burnoutLevel="<<(int)p.burnout.level<<"\n"
     <<"burnoutDays="<<p.burnout.consecutiveGrindDays<<"\n"
     <<"streakCurrent="<<p.streak.current<<"\n"
     <<"streakAllTime="<<p.streak.allTime<<"\n"
     <<"difficulty="<<(int)p.difficulty<<"\n"
     <<"coverLetter="<<(int)p.coverLetter<<"\n"
     <<"track="<<(int)p.track<<"\n"
     <<"appliedSWE="<<p.appliedSWE<<"\n"
     <<"appliedFin="<<p.appliedFin<<"\n"
     <<"appliedCon="<<p.appliedCon<<"\n";
    f.close();
}

bool doLoad(Player& p) {
    std::ifstream f(SAVE_FILE);
    if (!f.is_open()) return false;
    std::string line;
    while (std::getline(f, line)) {
        size_t eq = line.find('=');
        if (eq==std::string::npos) continue;
        std::string k=line.substr(0,eq), v=line.substr(eq+1);
        int iv=0;
        try { iv=std::stoi(v); } catch(...) { continue; }
        if      (k=="prestige")       p.prestige=iv;
        else if (k=="tier")           p.tier=(ResumeTier)iv;
        else if (k=="reputation")     p.reputation=iv;
        else if (k=="connections")    p.connections=iv;
        else if (k=="hasReferral")    p.hasReferral=iv;
        else if (k=="hope")           p.hope=iv;
        else if (k=="energy")         p.energy=iv;
        else if (k=="maxEnergy")      p.maxEnergy=iv;
        else if (k=="day")            p.day=iv;
        else if (k=="applied")        p.applied=iv;
        else if (k=="rejected")       p.rejected=iv;
        else if (k=="ghosted")        p.ghosted=iv;
        else if (k=="interviews")     p.interviews=iv;
        else if (k=="offers")         p.offers=iv;
        else if (k=="offersAccepted") p.offersAccepted=iv;
        else if (k=="networked")      p.networked=iv;
        else if (k=="atsFailed")      p.atsFailed=iv;
        else if (k=="ventCount")      p.ventCount=iv;
        else if (k=="season")         p.season=(Season)iv;
        else if (k=="daysInSeason")   p.daysInSeason=iv;
        else if (k=="fastestOffer")   p.fastestOffer=iv;
        else if (k=="leetcodeLevel")  p.skills.leetcodeLevel=iv;
        else if (k=="leetcodeXP")     p.skills.leetcodeXP=iv;
        else if (k=="portfolioLevel") p.skills.portfolioLevel=iv;
        else if (k=="portfolioXP")    p.skills.portfolioXP=iv;
        else if (k=="cloutLevel")     p.skills.cloutLevel=iv;
        else if (k=="cloutXP")        p.skills.cloutXP=iv;
        else if (k=="burnoutLevel")   p.burnout.level=(BurnoutLevel)iv;
        else if (k=="burnoutDays")    p.burnout.consecutiveGrindDays=iv;
        else if (k=="streakCurrent")  p.streak.current=iv;
        else if (k=="streakAllTime")  p.streak.allTime=iv;
        else if (k=="difficulty")     p.difficulty=(Difficulty)iv;
        else if (k=="coverLetter")    p.coverLetter=(CoverLetterStyle)iv;
        else if (k=="track")          p.track=(CareerTrack)iv;
        else if (k=="appliedSWE")     p.appliedSWE=iv;
        else if (k=="appliedFin")     p.appliedFin=iv;
        else if (k=="appliedCon")     p.appliedCon=iv;
    }
    f.close();
    return true;
}

// ---- MAIN ---------------------------------------------------------------------

int main() {
    srand(static_cast<unsigned>(time(nullptr)));
    loadAchievements();

    std::cout << "\n" << BOLD << CYAN
              << "  +-------------------------------------------+\n"
              << "  |      REJECTION SIMULATOR 3000 (C++ ed.)   |\n"
              << "  |           A game based on a true story.    |\n"
              << "  +-------------------------------------------+\n\n" << RESET;

    Player     p;
    JobBoard   board;
    StoryState story;

    // Check for save
    std::ifstream chk(SAVE_FILE);
    bool hasSave = chk.is_open();
    chk.close();

    if (hasSave) {
        std::cout << "  Save found. [c]ontinue  [n]ew game  [s]tory mode > ";
        std::string choice; std::cin >> choice;
        if (choice=="c") {
            doLoad(p);
            std::cout << GREEN << "  Loaded. Day " << p.day << ".\n\n" << RESET;
        } else if (choice=="s") {
            std::remove(SAVE_FILE.c_str());
            story = runStorySetup();
            p.difficulty = static_cast<Difficulty>(story.startDebt > 60 ? 3 : story.startDebt > 30 ? 2 : 1);
            printHelp();
        } else {
            std::remove(SAVE_FILE.c_str());
            p.difficulty = runDifficultySelect();
            p.track      = CareerTrack::SoftwareEng;
            p.coverLetter = runCoverLetterSetup();
            printHelp();
        }
    } else {
        std::cout << "  [n]ew game  [s]tory mode > ";
        std::string choice; std::cin >> choice;
        if (choice=="s") {
            story = runStorySetup();
            p.difficulty = static_cast<Difficulty>(story.startDebt > 60 ? 3 : story.startDebt > 30 ? 2 : 1);
        } else {
            p.difficulty = runDifficultySelect();
            p.track      = CareerTrack::SoftwareEng;
            p.coverLetter = runCoverLetterSetup();
        }
        printHelp();
    }

    std::string input;
    while (true) {
        printHUD(p, story);
        std::cout << "> ";
        std::cin >> input;

        if (input=="a" || input=="apply") {
            runApply(p, board);
            doSave(p);

        } else if (input=="n" || input=="network") {
            runNetworking(p);
            doSave(p);

        } else if (input=="t" || input=="train") {
            std::string sub; std::cin >> sub;
            if (p.energy<1) { std::cout<<RED<<"  No energy.\n"<<RESET; }
            else { p.energy--; runTraining(p.skills, sub); doSave(p); }

        } else if (input=="i" || input=="prep") {
            runInterviewPrep(p.prep, p.energy);
            doSave(p);

        } else if (input=="s" || input=="sleep") {
            handleSleep(p, story);
            doSave(p);

        } else if (input=="c" || input=="cover") {
            p.coverLetter = runCoverLetterSetup();
            doSave(p);

        } else if (input=="x" || input=="track") {
            std::cout << "  [1] Software Engineering  [2] Finance  [3] Consulting > ";
            std::string t; std::cin >> t;
            if      (t=="1") p.track=CareerTrack::SoftwareEng;
            else if (t=="2") p.track=CareerTrack::Finance;
            else if (t=="3") p.track=CareerTrack::Consulting;
            std::cout << DIM << "  Track: " << trackName(p.track) << "\n\n" << RESET;
            doSave(p);

        } else if (input=="v" || input=="view") {
            printJobBoard(board, p.day);

        } else if (input=="p" || input=="profile") {
            printProfile(p);

        } else if (input=="z" || input=="achievements") {
            printAchievements();

        } else if (input=="l" || input=="leaderboard") {
            printLeaderboard();

        } else if (input=="vent") {
            runVent(p, p.ventCount);
            if (p.ventCount>=10 && unlockAchievement("vent_10")) printAchievementUnlock("vent_10");
            doSave(p);

        } else if (input=="h" || input=="help") {
            printHelp();

        } else if (input=="q" || input=="quit") {
            submitToLeaderboard(p);
            doSave(p);
            // Final summary
            std::cout << "\n" << BOLD << "  -- Final Stats --\n" << RESET;
            std::cout << "  Days:        " << p.day        << "\n";
            std::cout << "  Applied:     " << p.applied    << "\n";
            std::cout << "  Rejected:    " << p.rejected   << "\n";
            std::cout << "  Ghosted:     " << p.ghosted    << "\n";
            std::cout << "  Interviews:  " << p.interviews << "\n";
            std::cout << "  Offers:      " << p.offers     << "\n";
            std::cout << "  Prestige:    " << p.prestige   << "\n";
            std::cout << "  Hope left:   " << p.hope       << "%\n\n";
            if (p.offers>0) std::cout<<GREEN<<BOLD<<"  You got an offer. The grind was worth it. Maybe.\n\n"<<RESET;
            else if (p.hope<=0) std::cout<<RED<<"  Hope at zero. The market won.\n\n"<<RESET;
            else std::cout<<"  Still got "<<p.hope<<"% hope. That's more than most.\n\n";
            std::cout<<DIM<<"  Progress saved.\n\n"<<RESET;
            break;

        } else if (input=="r" || input=="reset") {
            std::cout << "\n  Wipe save? [y/n] > ";
            std::string c; std::cin>>c;
            if (c=="y"||c=="Y") {
                submitToLeaderboard(p);
                std::remove(SAVE_FILE.c_str());
                p = Player(); board = JobBoard(); story = StoryState();
                p.difficulty = runDifficultySelect();
                p.coverLetter = runCoverLetterSetup();
                std::cout<<GREEN<<"\n  Reset. Same market. New you.\n\n"<<RESET;
                printHelp();
            }

        } else {
            std::cout << RED << "  Unknown command. [h] for help.\n" << RESET;
        }
    }

    return 0;
}
