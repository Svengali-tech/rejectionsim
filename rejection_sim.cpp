#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <thread>
#include <chrono>

#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN    "\033[36m"
#define BOLD    "\033[1m"
#define DIM     "\033[2m"

void sleep_ms(int ms) {
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

void printSeparator() {
    std::cout << DIM << std::string(60, '-') << RESET << "\n";
}

std::string pickRandom(const std::vector<std::string>& v) {
    return v[rand() % v.size()];
}

void printHopeBar(int hope) {
    std::cout << "Hope:  [";
    int filled = hope / 5;
    int empty  = 20 - filled;
    if (hope > 60)       std::cout << GREEN;
    else if (hope > 30)  std::cout << YELLOW;
    else                 std::cout << RED;
    std::cout << std::string(filled, '#') << RESET
              << std::string(empty, '.') << "] "
              << std::setw(3) << hope << "%\n";
}

void printStats(int applied, int rejected, int ghosted, int interviews, int offers, int hope) {
    std::cout << "\n";
    std::cout << BOLD << "  REJECTION SIMULATOR 3000" << RESET << "\n";
    printSeparator();
    std::cout << "Applied: "     << BOLD    << std::setw(4) << applied    << RESET
              << "   Rejected: " << RED     << BOLD << std::setw(4) << rejected   << RESET
              << "   Ghosted: "  << DIM     << std::setw(4) << ghosted    << RESET << "\n";
    std::cout << "Interviews: "  << YELLOW  << BOLD << std::setw(3) << interviews << RESET
              << "   Offers: "   << GREEN   << BOLD << std::setw(5) << offers     << RESET << "\n";
    printHopeBar(hope);
    printSeparator();
}

bool runPhoneScreen(const std::string& co) {
    const std::vector<std::string> passMsgs = {
        "  Recruiter says you seem 'great'. You will never hear that word again.",
        "  You passed. They said you were 'a strong communicator.' Low bar.",
        "  You made it through. The recruiter laughed at your joke. Once.",
        "  They liked you enough to waste more of your time. Congrats.",
    };
    const std::vector<std::string> failMsgs = {
        "  The recruiter said 'we'll be in touch.' They were not.",
        "  You mentioned salary expectations. Call ended shortly after.",
        "  You said you knew their stack. They asked a follow-up. You didn't.",
        "  The recruiter had another call. You were not rescheduled.",
        "  They said the role 'might be changing.' It wasn't. You just failed.",
    };

    std::cout << YELLOW << "[PHONE SCREEN]\n" << RESET;
    std::cout << DIM << "  " << co << " wants 30 minutes of your life.\n" << RESET;
    sleep_ms(600);

    bool passed = (rand() % 100) < 55;
    if (passed) {
        std::cout << GREEN << "  >> PASSED\n" << RESET;
        std::cout << pickRandom(passMsgs) << "\n";
    } else {
        std::cout << RED << "  >> FAILED\n" << RESET;
        std::cout << pickRandom(failMsgs) << "\n";
    }
    return passed;
}

bool runTechnical(const std::string& co) {
    (void)co;
    const std::vector<std::string> questions = {
        "  Q: Reverse a linked list. You blanked for 4 seconds.",
        "  Q: Two Sum. You did it in O(n^2). They watched.",
        "  Q: Design a URL shortener. You forgot about databases briefly.",
        "  Q: Binary search. You got it. They didn't look impressed.",
        "  Q: Implement a trie. In JavaScript. On a whiteboard. Go.",
        "  Q: 'What's your approach to system design?' You said 'it depends.' Twice.",
        "  Q: Dijkstra's algorithm. From scratch. No hints. Timer running.",
    };
    const std::vector<std::string> passMsgs = {
        "  Interviewer: 'Not bad.' That's it. That's all you get.",
        "  You passed. They said they'd 'follow up soon.' It took 11 days.",
        "  They said your solution was 'acceptable.' Frame that.",
        "  You got through it. The silence at the end was normal. Probably.",
    };
    const std::vector<std::string> failMsgs = {
        "  Feedback: 'Didn't demonstrate strong problem solving skills.'",
        "  Feedback: 'We were looking for a more optimal solution.' You had O(n log n).",
        "  Feedback: 'Communication could be clearer.' You explained everything.",
        "  No feedback. Portal updated to 'position filled.'",
        "  Feedback: 'Strong candidate but not the right fit at this time.'",
        "  They said they'd send feedback. The feedback never came.",
    };

    std::cout << YELLOW << "[TECHNICAL INTERVIEW]\n" << RESET;
    std::cout << pickRandom(questions) << "\n";
    sleep_ms(700);

    bool passed = (rand() % 100) < 45;
    if (passed) {
        std::cout << GREEN << "  >> PASSED\n" << RESET;
        std::cout << pickRandom(passMsgs) << "\n";
    } else {
        std::cout << RED << "  >> FAILED\n" << RESET;
        std::cout << pickRandom(failMsgs) << "\n";
    }
    return passed;
}

bool runFinalRound(const std::string& co) {
    const std::vector<std::string> questions = {
        "  'Tell me about a time you failed.' You picked the wrong story.",
        "  'Where do you see yourself in 5 years?' You said 'here.' Too eager.",
        "  'What's your biggest weakness?' You said 'perfectionism.' They wrote something down.",
        "  'Why do you want to work here?' You mentioned the mission. They nodded vaguely.",
        "  'How do you handle conflict?' Your answer was too honest.",
        "  Four interviewers back to back. Last one asked the same question as the first.",
    };
    const std::vector<std::string> passMsgs = {
        "  Panel says you were 'impressive.' Decision coming 'end of week.'",
        "  They loved you. You can tell because they said 'we'll be in touch very soon.'",
        "  Final round done. The hiring manager shook your hand twice. Means nothing.",
        "  Strong final round. One panelist said 'I'd work with you.' The other said nothing.",
    };
    const std::vector<std::string> failMsgs = {
        "  'We went with someone whose experience was a slightly better match.'",
        "  'The team felt you were great but we had a very strong candidate pool.'",
        "  'We've decided to go in a different direction with this role.'",
        "  'The role has been put on hold due to budget.' (Reposted 3 weeks later.)",
        "  'After deliberation, we've chosen another candidate.' No further details.",
        "  You made it to the final round and lost to someone's college roommate.",
    };

    std::cout << MAGENTA << "[FINAL ROUND]\n" << RESET;
    std::cout << "  " << co << " brings you in. Four people. Two hours.\n";
    std::cout << pickRandom(questions) << "\n";
    sleep_ms(800);

    bool passed = (rand() % 100) < 35;
    if (passed) {
        std::cout << GREEN << "  >> PASSED\n" << RESET;
        std::cout << pickRandom(passMsgs) << "\n";
    } else {
        std::cout << RED << "  >> FAILED\n" << RESET;
        std::cout << pickRandom(failMsgs) << "\n";
    }
    return passed;
}

void runOffer(const std::string& co, const std::string& role, int& offers, int& hope) {
    const std::vector<std::string> offerRemarks = {
        "  They say it's 'competitive for the market.' It isn't.",
        "  The equity 'could be worth a lot someday.' Could be.",
        "  Benefits include 'unlimited PTO.' You will never take any.",
        "  Sign-on bonus included. Claws back if you leave within 12 months.",
        "  They say 'we move fast here.' You will find out what that means on day one.",
    };

    int base   = 120 + (rand() % 80);
    int equity = 10  + (rand() % 90);
    int bonus  = 5   + (rand() % 20);

    offers++;
    hope = std::min(100, hope + 40);

    std::cout << "\n";
    std::cout << GREEN << BOLD
              << "  ╔══════════════════════════════╗\n"
              << "  ║         OFFER RECEIVED       ║\n"
              << "  ╚══════════════════════════════╝\n" << RESET;
    std::cout << "\n";
    std::cout << BOLD << "  " << co << " -- " << role << "\n\n" << RESET;
    std::cout << "  Base salary:   " << GREEN << BOLD << "$" << base << "k" << RESET << "\n";
    std::cout << "  Equity (est.): " << YELLOW << "$" << equity << "k" << RESET
              << DIM << "  (4yr vest, 1yr cliff)" << RESET << "\n";
    std::cout << "  Sign-on:       $" << bonus << "k\n";
    std::cout << "\n";
    std::cout << DIM << pickRandom(offerRemarks) << "\n" << RESET;
    std::cout << "\n";

    std::cout << "  Negotiate? [y/n] > ";
    std::string choice;
    std::cin >> choice;

    if (choice == "y" || choice == "Y") {
        int roll = rand() % 100;
        if (roll < 40) {
            int bump = 5 + rand() % 15;
            std::cout << GREEN << "\n  They came up $" << bump << "k. "
                      << "'Best they could do.'\n" << RESET;
            std::cout << DIM << "  It wasn't.\n" << RESET;
        } else if (roll < 70) {
            std::cout << YELLOW << "\n  'No flexibility on base but we can add equity.'\n" << RESET;
            std::cout << DIM << "  The equity vests in 4 years. You'll see.\n" << RESET;
        } else {
            std::cout << RED << "\n  They rescinded the offer.\n" << RESET;
            std::cout << DIM << "  'Felt your expectations weren't aligned with the role.'\n" << RESET;
            offers--;
            hope = std::max(0, hope - 50);
            return;
        }
    } else {
        std::cout << DIM << "\n  Smart. Probably.\n" << RESET;
    }

    std::cout << "\n" << BOLD << GREEN
              << "  You accepted. Start date TBD.\n"
              << "  They said paperwork coming 'this week.'\n" << RESET;
    std::cout << DIM << "  It arrived 12 days later.\n\n" << RESET;
}

int main() {
    srand(static_cast<unsigned>(time(nullptr)));

    const std::vector<std::string> companies = {
        "Rippling", "Linear", "Retool", "Vercel", "Supabase", "Clerk",
        "Replit", "Descript", "Notion", "Figma", "Superhuman", "Scale AI",
        "Brex", "Ramp", "Mercury", "Plaid", "Deel", "Gusto", "Lattice",
        "Alchemy", "Warpcast", "Modal", "Fly.io", "PlanetScale", "Loom"
    };

    const std::vector<std::string> roles = {
        "Founding Engineer", "Full Stack Engineer", "iOS Engineer",
        "Backend Engineer", "Staff Engineer", "Senior Software Engineer",
        "Platform Engineer", "Growth Engineer", "API Developer",
        "Solutions Engineer", "ML Engineer", "Developer Advocate"
    };

    const std::vector<std::string> rejections = {
        "After careful consideration, we've decided to move forward\n  with other candidates.",
        "We were impressed, but we're looking for someone with more\n  experience in our specific stack.",
        "Unfortunately, we don't think this is the right fit at this time.",
        "We've decided to pause this search indefinitely. Best of luck!",
        "We received an overwhelming number of applications.",
        "We're looking for someone local. No relocation budget.",
        "We've decided to go with an internal candidate.",
        "We're restructuring this role and putting the search on hold.",
        "Your experience doesn't quite align with our current needs.",
        "After further review, we will not be moving forward."
    };

    const std::vector<std::string> ghostMessages = {
        "No response. Read receipts: off.",
        "Left on read. Classic.",
        "Your follow-up email was also ignored.",
        "The job posting was quietly deleted.",
        "They reposted the same role the following week.",
        "Opened at 11pm. Never replied.",
        "ATS auto-acknowledged. Human: never."
    };

    const std::vector<std::string> recruiterSpam = {
        "Hi! I came across your profile. We have a GREAT opportunity --\n  Junior Dev role, mostly Excel, pays $52k. LMK!",
        "Hey! Are you open to roles? We need a Principal Architect\n  (12-15 YOE), $58k base, fully onsite in Tulsa.",
        "Quick question -- interested in a 3-month contract?\n  No benefits. Hybrid. Parsippany, NJ.",
        "Your background is impressive! We're hiring a manual QA tester.\n  Interested?"
    };

    const std::vector<std::pair<int,std::string>> milestones = {
        {5,   "5 shots. Just warming up."},
        {10,  "10 deep. Easy Apply is muscle memory now."},
        {20,  "20 apps. Cover letter is basically a template."},
        {30,  "30. You have more data on hiring cycles than most analysts."},
        {50,  "50. Applied to the same company twice by accident."},
        {75,  "75. You check the portal before breakfast."},
        {100, "100 applications. A century of unanswered effort. LEGENDARY."}
    };

    int applied = 0, rejected = 0, ghosted = 0, interviews = 0, offers = 0, hope = 100;
    std::string input;

    std::cout << "\n" << BOLD << CYAN
              << "  ===================================\n"
              << "   REJECTION SIMULATOR 3000 (C++ ed.)\n"
              << "  ===================================\n" << RESET
              << "  A game based on a true story.\n\n"
              << "  Commands:  [a] apply   [q] quit   [r] reset\n\n";

    while (true) {
        printStats(applied, rejected, ghosted, interviews, offers, hope);
        std::cout << "\n> ";
        std::cin >> input;

        if (input == "q" || input == "quit") {
            std::cout << "\n" << BOLD << "Final stats:\n" << RESET;
            std::cout << "  Applied:    " << applied    << "\n";
            std::cout << "  Rejected:   " << rejected   << "\n";
            std::cout << "  Ghosted:    " << ghosted    << "\n";
            std::cout << "  Interviews: " << interviews << "\n";
            std::cout << "  Offers:     " << offers     << "\n";
            std::cout << "  Hope left:  " << hope       << "%\n\n";
            if (offers > 0)
                std::cout << GREEN << BOLD << "You got an offer. The grind was worth it. Maybe.\n\n" << RESET;
            else if (applied == 0)
                std::cout << "You didn't even apply. Bold strategy.\n\n";
            else if (hope <= 0)
                std::cout << RED << "Hope at zero. The market won. It usually does.\n\n" << RESET;
            else
                std::cout << "Still got " << hope << "% hope. That's more than most.\n\n";
            break;

        } else if (input == "r" || input == "reset") {
            applied = rejected = ghosted = interviews = offers = 0;
            hope = 100;
            std::cout << GREEN << "\nReset. Fresh eyes. New you. Same market.\n\n" << RESET;
            continue;

        } else if (input != "a" && input != "apply") {
            std::cout << RED << "Unknown command. [a]pply, [r]eset, [q]uit.\n" << RESET;
            continue;
        }

        applied++;
        std::string co   = pickRandom(companies);
        std::string role = pickRandom(roles);
        double roll = (double)rand() / RAND_MAX;

        std::cout << "\n";
        std::cout << BOLD << "To: " << co << " -- " << role << RESET << "\n";
        sleep_ms(400);

        bool do_milestone = true;

        if (roll < 0.05) {
            std::cout << BLUE << "[RECRUITER SPAM]\n" << RESET;
            std::cout << "  " << pickRandom(recruiterSpam) << "\n";
            hope = std::min(100, hope + 2);
            std::cout << DIM << "  (At least it's technically a response.)\n" << RESET;

        } else if (roll < 0.35) {
            ghosted++;
            hope = std::max(0, hope - 8);
            std::cout << DIM << "[GHOSTED]\n" << RESET;
            std::cout << "  " << pickRandom(ghostMessages) << "\n";

        } else if (roll < 0.80) {
            rejected++;
            hope = std::max(0, hope - 12);
            std::cout << RED << "[REJECTED]\n" << RESET;
            std::cout << "  " << pickRandom(rejections) << "\n";

        } else {
            interviews++;
            hope = std::min(100, hope + 5);
            std::cout << CYAN << "[INTERVIEW PIPELINE]\n" << RESET;
            std::cout << DIM << "  They actually responded. Suspicious.\n\n" << RESET;
            sleep_ms(300);

            if (!runPhoneScreen(co)) {
                rejected++;
                hope = std::max(0, hope - 15);
                std::cout << "\n";
            } else {
                std::cout << "\n";
                sleep_ms(400);
                if (!runTechnical(co)) {
                    rejected++;
                    hope = std::max(0, hope - 20);
                    std::cout << "\n";
                } else {
                    std::cout << "\n";
                    sleep_ms(400);
                    if (!runFinalRound(co)) {
                        rejected++;
                        hope = std::max(0, hope - 25);
                        std::cout << DIM << "\n  Made it to the final round and still lost."
                                  << " That's a special kind of pain.\n" << RESET;
                        std::cout << "\n";
                    } else {
                        std::cout << "\n";
                        sleep_ms(500);
                        runOffer(co, role, offers, hope);
                    }
                }
            }
        }

        if (do_milestone) {
            for (const auto& [n, msg] : milestones) {
                if (applied == n)
                    std::cout << "\n" << YELLOW << BOLD << "  >> " << msg << RESET << "\n";
            }
        }

        if (hope <= 0) {
            std::cout << "\n" << RED << BOLD
                      << "  !! HOPE DEPLETED. The market has won.\n"
                      << "     But you're still here. That counts.\n" << RESET;
        }

        std::cout << "\n";
    }

    return 0;
}
