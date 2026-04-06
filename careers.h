#pragma once

#include <string>
#include <vector>
#include "colors.h"
#include "utils.h"

// The three career tracks available in the game.
// Each track has different interview styles, role names, companies,
// and stage names. The pass rates are tuned per track.
enum class CareerTrack {
    SoftwareEng = 0,
    Finance     = 1,
    Consulting  = 2
};

inline std::string trackName(CareerTrack t) {
    switch (t) {
        case CareerTrack::SoftwareEng: return "Software Engineering";
        case CareerTrack::Finance:     return "Finance";
        case CareerTrack::Consulting:  return "Consulting";
    }
    return "Unknown";
}

// ---- COMPANIES BY TRACK -------------------------------------------------------

inline std::vector<std::string> SWE_COMPANIES_T1 = {
    "Supabase", "Clerk", "Replit", "Descript", "Loom",
    "Warpcast", "Modal", "Fly.io", "PlanetScale", "Railway"
};
inline std::vector<std::string> SWE_COMPANIES_T2 = {
    "Rippling", "Linear", "Retool", "Vercel", "Notion",
    "Superhuman", "Brex", "Ramp", "Mercury", "Plaid"
};
inline std::vector<std::string> SWE_COMPANIES_T3 = {
    "Figma", "Scale AI", "Stripe", "Coinbase", "OpenAI"
};
inline std::vector<std::string> SWE_FAANG = {
    "Google", "Meta", "Apple", "Amazon", "Microsoft", "Netflix"
};

inline std::vector<std::string> FINANCE_COMPANIES_T1 = {
    "Baird", "Piper Sandler", "Houlihan Lokey", "Lazard", "Moelis",
    "Jefferies", "Cowen", "Imperial Capital", "Stifel", "Raymond James"
};
inline std::vector<std::string> FINANCE_COMPANIES_T2 = {
    "Deutsche Bank", "Barclays", "UBS", "Credit Suisse", "Nomura",
    "Mizuho", "HSBC", "BNP Paribas", "Societe Generale", "RBC"
};
inline std::vector<std::string> FINANCE_COMPANIES_T3 = {
    "Goldman Sachs", "Morgan Stanley", "JP Morgan", "Bank of America",
    "Citigroup", "Wells Fargo", "Blackstone", "KKR", "Apollo", "Carlyle"
};

inline std::vector<std::string> CONSULTING_COMPANIES_T1 = {
    "West Monroe", "Protiviti", "FTI Consulting", "Navigant", "A.T. Kearney",
    "Oliver Wyman", "L.E.K.", "Analysis Group", "Cornerstone Research", "NERA"
};
inline std::vector<std::string> CONSULTING_COMPANIES_T2 = {
    "Deloitte", "PwC", "EY", "KPMG", "Accenture",
    "IBM Consulting", "Capgemini", "Booz Allen", "SAIC", "Leidos"
};
inline std::vector<std::string> CONSULTING_COMPANIES_T3 = {
    "McKinsey", "Bain", "BCG", "Roland Berger", "Strategy&"
};

// ---- ROLES BY TRACK -----------------------------------------------------------

inline std::vector<std::string> SWE_ROLES = {
    "Founding Engineer", "Full Stack Engineer", "iOS Engineer",
    "Backend Engineer",  "Staff Engineer",      "Senior Software Engineer",
    "Platform Engineer", "ML Engineer",         "API Developer",
    "Infrastructure Engineer", "Security Engineer", "DevEx Engineer"
};

inline std::vector<std::string> FINANCE_ROLES = {
    "Investment Banking Analyst", "IB Associate", "Private Equity Analyst",
    "Hedge Fund Analyst",         "Equity Research Associate",
    "Credit Analyst",             "FP&A Analyst",
    "Corporate Finance Associate","Risk Analyst",
    "Quantitative Analyst",       "Portfolio Manager",
    "M&A Associate",              "CFAO Analyst",
    "Treasury Analyst",           "Leveraged Finance Analyst"
};

inline std::vector<std::string> CONSULTING_ROLES = {
    "Business Analyst",          "Associate Consultant",
    "Consultant",                "Senior Consultant",
    "Engagement Manager",        "Strategy Analyst",
    "Management Consultant",     "Operations Analyst",
    "Technology Consultant",     "Financial Advisory Associate",
    "Transformation Consultant", "Due Diligence Analyst"
};

// ---- INTERVIEW STAGE NAMES BY TRACK -------------------------------------------
// Each track has different names for the same underlying stages so it feels authentic.

struct TrackConfig {
    std::string phoneLabel;     // what they call the first screen
    std::string techLabel;      // what they call the skills round
    std::string finalLabel;     // what they call the final
    int         phoneRate;      // base pass rate: phone screen
    int         techRate;       // base pass rate: skills round
    int         finalRate;      // base pass rate: final
};

inline TrackConfig getTrackConfig(CareerTrack t) {
    switch (t) {
        case CareerTrack::SoftwareEng:
            return { "Phone Screen", "Technical Interview", "Final Round", 55, 45, 35 };
        case CareerTrack::Finance:
            // Finance is brutal: superday is the final and pass rates are lower
            return { "HR Screen", "Technical/Fit Interview", "Superday", 50, 40, 30 };
        case CareerTrack::Consulting:
            // Consulting is all case interviews, slightly more structured
            return { "Fit Screen", "Case Interview", "Partner Round", 52, 38, 32 };
    }
    return { "Phone Screen", "Interview", "Final", 55, 45, 35 };
}

// ---- TRACK-SPECIFIC INTERVIEW QUESTIONS ---------------------------------------

inline std::vector<std::string> FINANCE_TECH_QUESTIONS = {
    "  Q: Walk me through a DCF. You forgot to mention terminal value until prompted.",
    "  Q: How do you value a company with negative earnings? You said 'EV/Revenue.' They wrote something.",
    "  Q: What's the difference between enterprise value and equity value? You got it. They nodded once.",
    "  Q: Walk me through the three financial statements. You linked them correctly. Barely.",
    "  Q: If PP&E goes up by $10, walk me through the impact. You blanked on deferred tax.",
    "  Q: What's a leveraged buyout? You explained it. They asked you to model one. In your head.",
    "  Q: Why investment banking? You said 'the deals.' They've heard that before.",
    "  Q: Pitch me a stock. You pitched one you read about this morning.",
};

inline std::vector<std::string> CONSULTING_CASE_QUESTIONS = {
    "  Case: Your client is a regional grocery chain losing market share. Profitability or growth?",
    "  Case: A pharma company wants to enter the OTC market. Market sizing. Go.",
    "  Case: Operating margins dropped 5pts YoY. Interviewer is staring at you. Frameworks.",
    "  Case: Client is considering acquiring a competitor. You forgot to ask about synergies.",
    "  Case: A hospital system wants to reduce costs by 15%. You structured it. Sort of.",
    "  Case: New product launch in Southeast Asia. You sized the market wrong. They told you.",
    "  Case: Interviewer asks you to estimate the number of gas stations in the US.",
    "  Case: Client's revenue is flat but costs are rising. You said 'benchmark competitors' first.",
};

inline std::vector<std::string> FINANCE_FINAL_QUESTIONS = {
    "  Superday: Five back-to-back interviews. The fourth one asked the same DCF question as the first.",
    "  Superday: An MD asked where you see yourself in 10 years. You said 'buyside.' They smiled vaguely.",
    "  Superday: They asked about a deal in the news. You picked one from last week. It was fine.",
    "  Superday: VP asked why not consulting. You answered. They moved on without reacting.",
    "  Superday: They asked you to walk through your resume. You have done this 200 times. It showed.",
};

inline std::vector<std::string> CONSULTING_FINAL_QUESTIONS = {
    "  Partner round: They gave you a case with no structure. You made one. It was okay.",
    "  Partner round: Partner asked 'why McKinsey.' You said 'impact at scale.' They've heard that.",
    "  Partner round: Fit questions followed by a market sizing. You nailed the sizing.",
    "  Partner round: They asked about a time you led through ambiguity. You told the truth.",
    "  Partner round: Partner interrupted your case mid-way to pivot the question. You adapted.",
};

// ---- TRACK-SPECIFIC REJECTION MESSAGES ----------------------------------------

inline std::vector<std::string> FINANCE_REJECTIONS = {
    "We've decided to move forward with candidates whose backgrounds more closely\n  align with our group's focus.",
    "After careful review, we won't be extending an offer at this time.",
    "The class is now full. We encourage you to apply again next cycle.",
    "We had an exceptionally competitive pool this season.",
    "After the superday, we've decided not to extend an offer. Best of luck.",
    "We're moving forward with candidates with more relevant deal experience.",
};

inline std::vector<std::string> CONSULTING_REJECTIONS = {
    "After reviewing your case performance, we've decided not to move forward.",
    "We were impressed but had an unusually strong candidate pool this round.",
    "We're unable to extend an offer at this time. We encourage you to reapply.",
    "After deliberation, we've selected candidates whose skills better match our current needs.",
    "Your fit interviews were strong but we had concerns about the case performance.",
    "We've filled our analyst class for this cycle.",
};

// Returns the appropriate rejection pool for the given track.
inline const std::vector<std::string>& getTrackRejections(CareerTrack t) {
    static std::vector<std::string> swe = {
        "After careful consideration, we've decided to move forward with other candidates.",
        "We were impressed but looking for someone with more experience in our specific stack.",
        "Unfortunately, we don't think this is the right fit at this time.",
        "We've decided to go with an internal candidate.",
        "Your experience doesn't quite align with our current needs.",
        "After further review, we will not be moving forward.",
    };
    switch (t) {
        case CareerTrack::Finance:     return FINANCE_REJECTIONS;
        case CareerTrack::Consulting:  return CONSULTING_REJECTIONS;
        default:                       return swe;
    }
}

// Returns a random company name for the given track and prestige level.
inline std::string pickCompany(CareerTrack track, int prestige) {
    switch (track) {
        case CareerTrack::SoftwareEng:
            if (prestige >= 2) {
                int r = randRange(0, 2);
                if (r == 0) return pickRandom(SWE_FAANG);
                if (r == 1) return pickRandom(SWE_COMPANIES_T3);
            }
            if (prestige >= 1) return pickRandom(SWE_COMPANIES_T2);
            return pickRandom(SWE_COMPANIES_T1);

        case CareerTrack::Finance:
            if (prestige >= 2) return pickRandom(FINANCE_COMPANIES_T3);
            if (prestige >= 1) return pickRandom(FINANCE_COMPANIES_T2);
            return pickRandom(FINANCE_COMPANIES_T1);

        case CareerTrack::Consulting:
            if (prestige >= 2) return pickRandom(CONSULTING_COMPANIES_T3);
            if (prestige >= 1) return pickRandom(CONSULTING_COMPANIES_T2);
            return pickRandom(CONSULTING_COMPANIES_T1);
    }
    return "Unknown Corp";
}

inline std::string pickRole(CareerTrack track) {
    switch (track) {
        case CareerTrack::Finance:    return pickRandom(FINANCE_ROLES);
        case CareerTrack::Consulting: return pickRandom(CONSULTING_ROLES);
        default:                      return pickRandom(SWE_ROLES);
    }
}
