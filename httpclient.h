#pragma once

// Global leaderboard HTTP submission.
//
// On Windows this uses WinHTTP which ships with Windows -- no extra libraries needed.
// On macOS/Linux it uses a curl subprocess call so the binary has zero link-time
// dependencies. If curl isn't installed the submission silently skips.
//
// To point at your own deployed backend, change LEADERBOARD_HOST and LEADERBOARD_PORT.
// To disable global submission entirely, set GLOBAL_LB_ENABLED to 0.

#include <string>
#include <iostream>
#include "colors.h"
#include "player.h"

static const std::string LEADERBOARD_HOST = "localhost";
static const int         LEADERBOARD_PORT = 5000;
static const std::string LEADERBOARD_PATH = "/scores";
static const int         GLOBAL_LB_ENABLED = 1;  // set to 0 to disable

// Builds the JSON payload from player state.
// Matches the ScoreSubmission record in Program.cs exactly.
inline std::string buildScoreJson(const Player& p, const std::string& alias) {
    auto esc = [](const std::string& s) {
        // Simple quote escape -- alias is already sanitized but be safe
        std::string out;
        for (char c : s) {
            if (c == '"') out += "\\\"";
            else          out += c;
        }
        return out;
    };

    std::string track =
        (p.track == CareerTrack::Finance)     ? "Finance"    :
        (p.track == CareerTrack::Consulting)  ? "Consulting" : "SWE";

    std::string diff =
        (p.difficulty == Difficulty::Easy)      ? "Easy"      :
        (p.difficulty == Difficulty::Hard)      ? "Hard"      :
        (p.difficulty == Difficulty::Nightmare) ? "Nightmare" : "Normal";

    return std::string("{") +
        "\"Alias\":"        + "\"" + esc(alias)          + "\"," +
        "\"Applied\":"      + std::to_string(p.applied)      + "," +
        "\"Rejected\":"     + std::to_string(p.rejected)     + "," +
        "\"Ghosted\":"      + std::to_string(p.ghosted)      + "," +
        "\"Interviews\":"   + std::to_string(p.interviews)   + "," +
        "\"Offers\":"       + std::to_string(p.offers)       + "," +
        "\"Prestige\":"     + std::to_string(p.prestige)     + "," +
        "\"FastestOffer\":" + std::to_string(p.fastestOffer) + "," +
        "\"DaysPlayed\":"   + std::to_string(p.day)          + "," +
        "\"Track\":"        + "\"" + track                   + "\"," +
        "\"Difficulty\":"   + "\"" + diff                    + "\"," +
        "\"HopeLeft\":"     + std::to_string(p.hope)         +
        "}";
}

// ---- WINDOWS (WinHTTP) --------------------------------------------------------
#ifdef _WIN32
#include <windows.h>
#include <winhttp.h>
#pragma comment(lib, "winhttp.lib")

// Submits the score to the backend via WinHTTP.
// Returns true on HTTP 200/201, false on any error.
bool submitScoreGlobal(const Player& p, const std::string& alias) {
    if (!GLOBAL_LB_ENABLED) return false;

    std::string body = buildScoreJson(p, alias);

    // Convert host and path to wide strings for WinHTTP
    std::wstring wHost(LEADERBOARD_HOST.begin(), LEADERBOARD_HOST.end());
    std::wstring wPath(LEADERBOARD_PATH.begin(), LEADERBOARD_PATH.end());

    HINTERNET hSession = WinHttpOpen(
        L"RejectionSim/1.0",
        WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
        WINHTTP_NO_PROXY_NAME,
        WINHTTP_NO_PROXY_BYPASS, 0);
    if (!hSession) return false;

    HINTERNET hConnect = WinHttpConnect(
        hSession, wHost.c_str(),
        static_cast<INTERNET_PORT>(LEADERBOARD_PORT), 0);
    if (!hConnect) { WinHttpCloseHandle(hSession); return false; }

    HINTERNET hRequest = WinHttpOpenRequest(
        hConnect, L"POST", wPath.c_str(),
        NULL, WINHTTP_NO_REFERER,
        WINHTTP_DEFAULT_ACCEPT_TYPES, 0);   // 0 = HTTP (not HTTPS); use WINHTTP_FLAG_SECURE for prod
    if (!hRequest) {
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);
        return false;
    }

    // Set Content-Type header
    WinHttpAddRequestHeaders(hRequest,
        L"Content-Type: application/json",
        (ULONG)-1L, WINHTTP_ADDREQ_FLAG_ADD);

    // Send
    bool ok = WinHttpSendRequest(
        hRequest,
        WINHTTP_NO_ADDITIONAL_HEADERS, 0,
        (LPVOID)body.c_str(),
        (DWORD)body.size(),
        (DWORD)body.size(), 0) &&
        WinHttpReceiveResponse(hRequest, NULL);

    DWORD statusCode = 0;
    DWORD statusCodeSize = sizeof(statusCode);
    if (ok) {
        WinHttpQueryHeaders(hRequest,
            WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER,
            WINHTTP_HEADER_NAME_BY_INDEX,
            &statusCode, &statusCodeSize,
            WINHTTP_NO_HEADER_INDEX);
    }

    WinHttpCloseHandle(hRequest);
    WinHttpCloseHandle(hConnect);
    WinHttpCloseHandle(hSession);

    return (statusCode == 200 || statusCode == 201);
}

// ---- macOS / Linux (curl subprocess) -----------------------------------------
#else
#include <cstdlib>
#include <sstream>

// Submits via a curl shell command. No link-time dependency.
// If curl is not installed this returns false silently.
bool submitScoreGlobal(const Player& p, const std::string& alias) {
    if (!GLOBAL_LB_ENABLED) return false;

    std::string body = buildScoreJson(p, alias);
    std::string url  = "http://" + LEADERBOARD_HOST + ":" +
                       std::to_string(LEADERBOARD_PORT) + LEADERBOARD_PATH;

    // Build the curl command. Redirect stdout/stderr to /dev/null so it's silent.
    // --max-time 5 so a dead backend doesn't hang the game on quit.
    std::string cmd =
        "curl -s -o /dev/null --max-time 5 "
        "-X POST "
        "-H 'Content-Type: application/json' "
        "-d '" + body + "' "
        "'" + url + "' 2>/dev/null";

    int ret = system(cmd.c_str());
    return (ret == 0);
}
#endif

// ---- Public interface ---------------------------------------------------------

// Asks the player for an alias, then submits to the global leaderboard.
// Called on [q]uit after local leaderboard is already saved.
// Fails silently if the backend is unreachable -- never block the quit flow.
void runGlobalSubmit(const Player& p) {
    if (!GLOBAL_LB_ENABLED) return;

    std::cout << "\n  Submit to global leaderboard? [y/n] > ";
    std::string choice;
    std::cin >> choice;
    if (choice != "y" && choice != "Y") {
        std::cout << DIM << "  Skipped.\n" << RESET;
        return;
    }

    std::cout << "  Alias (max 20 chars, shown publicly) > ";
    std::string alias;
    std::cin >> alias;
    if (alias.empty()) alias = "Anonymous";
    if (alias.size() > 20) alias = alias.substr(0, 20);

    std::cout << DIM << "  Submitting..." << RESET;
    std::cout.flush();

    bool ok = submitScoreGlobal(p, alias);

    if (ok) {
        std::cout << GREEN << " done.\n" << RESET;
        std::cout << DIM << "  Score posted. Check the leaderboard at "
                  << LEADERBOARD_HOST << ":" << LEADERBOARD_PORT << "/scores\n" << RESET;
    } else {
        std::cout << DIM << " backend unreachable. Score not submitted.\n"
                  << "  Run the backend (cd backend && dotnet run) to enable this.\n" << RESET;
    }
    std::cout << "\n";
}
