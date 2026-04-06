#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <thread>
#include <chrono>

// ANSI color codes
#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define CYAN    "\033[36m"
#define BOLD    "\033[1m"
#define DIM     "\033[2m"

void sleep_ms(int ms) {
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

void printSeparator() {
    std::cout << DIM << std::string(60, '-') << RESET << "\n";
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

void printStats(int applied, int rejected, int ghosted, int hope) {
    std::cout << "\n";
    std::cout << BOLD << "  REJECTION SIMULATOR 3000" << RESET << "\n";
    printSeparator();
    std::cout << "Applied: " << BOLD << std::setw(4) << applied  << RESET
              << "   Rejected: " << RED << BOLD << std::setw(4) << rejected << RESET
              << "   Ghosted: "  << DIM << std::setw(4) << ghosted << RESET << "\n";
    printHopeBar(hope);
    printSeparator();
}

std::string pickRandom(const std::vector<std::string>& v) {
    return v[rand() % v.size()];
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

    int applied = 0, rejected = 0, ghosted = 0, hope = 100;
    std::string input;

    std::cout << "\n" << BOLD << CYAN
              << "  ===================================\n"
              << "   REJECTION SIMULATOR 3000 (C++ ed.)\n"
              << "  ===================================\n" << RESET
              << "  A game based on a true story.\n\n"
              << "  Commands:  [a] apply   [q] quit   [r] reset\n\n";

    while (true) {
        printStats(applied, rejected, ghosted, hope);
        std::cout << "\n> ";
        std::cin >> input;

        if (input == "q" || input == "quit") {
            std::cout << "\n" << BOLD << "Final stats:\n" << RESET;
            std::cout << "  Applied:  " << applied  << "\n";
            std::cout << "  Rejected: " << rejected << "\n";
            std::cout << "  Ghosted:  " << ghosted  << "\n";
            std::cout << "  Hope remaining: " << hope << "%\n\n";
            if (applied == 0)
                std::cout << "You didn't even try. Respect.\n\n";
            else if (hope <= 0)
                std::cout << "Hope at zero. The market broke you. It breaks everyone.\n\n";
            else
                std::cout << "Still got " << hope << "% hope. That's more than most.\n\n";
            break;

        } else if (input == "r" || input == "reset") {
            applied = rejected = ghosted = 0;
            hope = 100;
            std::cout << GREEN << "\nReset. Fresh eyes. New you.\n\n" << RESET;
            continue;

        } else if (input != "a" && input != "apply") {
            std::cout << RED << "Unknown command. Use [a]pply, [r]eset, [q]uit.\n" << RESET;
            continue;
        }

        // Apply
        applied++;
        std::string co   = pickRandom(companies);
        std::string role = pickRandom(roles);
        double roll = (double)rand() / RAND_MAX;

        std::cout << "\n";
        std::cout << BOLD << "To: " << co << " -- " << role << RESET << "\n";
        sleep_ms(400);

        if (roll < 0.05) {
            // Recruiter spam
            std::cout << BLUE << "[RECRUITER SPAM]\n" << RESET;
            std::cout << "  " << pickRandom(recruiterSpam) << "\n";
            hope = std::min(100, hope + 2);
            std::cout << DIM << "  (At least it's technically a response.)\n" << RESET;

        } else if (roll < 0.40) {
            // Ghosted
            ghosted++;
            hope = std::max(0, hope - 8);
            std::cout << DIM << "[GHOSTED]\n" << RESET;
            std::cout << "  " << pickRandom(ghostMessages) << "\n";

        } else {
            // Rejected
            rejected++;
            hope = std::max(0, hope - 12);
            std::cout << RED << "[REJECTED]\n" << RESET;
            std::cout << "  " << pickRandom(rejections) << "\n";
        }

        // Check milestones
        for (const auto& [n, msg] : milestones) {
            if (applied == n) {
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
