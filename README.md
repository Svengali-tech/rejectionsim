# Rejection Simulator 3000

A terminal-based job search simulator built in C++. Apply to companies, get rejected, watch your hope bar drain, and try to land an offer before the market breaks you.

Based on a true story.

---

## Quick Start

```bash
# Windows (MSYS2 UCRT64 terminal)
g++ -std=c++17 -O2 -o rejection_sim.exe main.cpp -lwinhttp
./rejection_sim.exe

# macOS / Linux (If your code uses libcurl functions, add -lcurl at link time:)
g++ -std=c++17 -O2 -o rejection_sim main.cpp -lcurl

#(Better (portable) — use pkg-config so compiler uses the correct flags:)
g++ -std=c++17 -O2 -o rejection_sim main.cpp $(pkg-config --cflags --libs libcurl)
./rejection_sim

#If you split compile and link:
g++ -std=c++17 -O2 -c main.cpp -o main.o
g++ -o rejection_sim main.o -lcurl
./rejection_sim

# or using pkg-config
g++ -o rejection_sim main.o $(pkg-config --libs libcurl)
./rejection_sim
```

All source files must be in the same directory. Only `main.cpp` is passed to the compiler -- it pulls in everything else through the headers.

---

## Gameplay

### Career Tracks

Choose between three tracks at the start of each run. Each has its own companies, roles, interview style, and rejection language.

| Track | Interview Stages | Companies |
|---|---|---|
| Software Engineering | Phone screen, technical interview, final round | Startups through FAANG |
| Finance | HR screen, technical/fit, superday | Boutiques through Goldman/Blackstone |
| Consulting | Fit screen, case interview, partner round | Regional firms through MBB |

Switch tracks mid-run with `[x]`.

### Commands

| Command | Action | Energy Cost |
|---|---|---|
| `a` | Apply to a job | 1 (2 if using tailored cover letter) |
| `n` | Network -- builds connections toward a referral | 2 |
| `t leet` | Grind leetcode -- improves technical round | 1 |
| `t portfolio` | Work on projects -- improves ATS and phone screen | 1 |
| `t clout` | Build presence -- improves final round | 1 |
| `i` | Interview prep -- one-time +20% to next pipeline | 2 |
| `s` | Sleep -- ends the day, refills energy, resolves pending stages | 0 |
| `c` | Change cover letter strategy | 0 |
| `x` | Switch career track | 0 |
| `v` | View job tracking board and pending pipeline stages | 0 |
| `o` | Negotiate with a competing offer (requires 2+ live offers) | 0 |
| `p` | Full profile with skills, stats, and conversion rates | 0 |
| `z` | Achievements | 0 |
| `l` | Local leaderboard | 0 |
| `vent` | +5 hope. No judgment. | 0 |
| `q` | Quit and save | 0 |
| `r` | Reset -- wipes save, submits run to local leaderboard | 0 |

### The Pipeline

Applications don't resolve instantly. When you get into a pipeline, each stage is queued with a wait time (1-5 days depending on track and stage). Sleep to advance the clock and see results arrive in your inbox.

```
Apply -> ATS filter -> Phone screen -> Technical/Case -> Final round -> Reference check -> Offer
```

Finance and consulting pipelines run slower than SWE. Referrals triple your pass rates.

### Stats That Affect Your Odds

| Stat | What it does |
|---|---|
| Resume Tier | +5% to all pass rates per tier (Junior -> Mid -> Senior -> Staff -> Principal) |
| Leetcode Grind | +3% per level to technical/case round |
| Portfolio Strength | +3% per level to ATS and phone screen |
| Network Clout | +3% per level to final round |
| Interview Prep | One-time +20% to next pipeline, consumed on entry |
| Referral | 3x base pass rate on next application |
| Burnout | -5% (Tired) / -15% (Burned Out) / -25% (Crispy) |
| Season | +20% (Bull) to -40% (Hiring Freeze) |
| Market Event | Temporary modifier from one-off market news |
| Difficulty | -30% (Nightmare) to +15% (Easy) across everything |

### Progression

Accepting an offer triggers prestige. Your resume tier upgrades, max energy increases, and new companies unlock. FAANG requires prestige 2+ and reputation 60+.

Tiers: Junior -> Mid-Level -> Senior -> Staff -> Principal

### Burnout

Apply or train multiple days in a row without sleeping or networking and burnout builds up. At Burned Out your cover letter develops typos. At Crispy your pass rates drop 25% and the game starts making fun of your applications in real time. Sleep or network to recover.

### Cover Letter Strategies

| Strategy | Pipeline Rate Bonus | Notes |
|---|---|---|
| None | 0% | Fine for most SWE roles |
| Generic | -5% | Actually hurts. HR can tell. |
| Tailored | +10% | Costs 2 energy per app instead of 1 |
| AI-Generated | +3% | -8% at final round. They've seen it before. |

### Story Mode

Start with a named character and one of three fixed scenarios, each with a debt counter and time pressure:

- **Broke and Brilliant** -- $82k student loans, Normal difficulty
- **The Lateral Move** -- $40k underpaid, Hard difficulty
- **Post-Layoff** -- $24k savings, 8 months of runway, Nightmare difficulty

Win condition: clear the debt by accepting an offer and staying employed.

### Difficulty

| Difficulty | ATS Rate | Ghost Rate | Referral Threshold | Market Bias |
|---|---|---|---|---|
| Easy | 90% | 20% | 3 connections | Bull market bias |
| Normal | 80% | 35% | 5 connections | None |
| Hard | 65% | 50% | 8 connections | Bear market bias |
| Nightmare | 50% | 60% | 10 connections | Bear market bias |

---

## Achievements

30 achievements tracked persistently in `rsim_achievements.txt` across all runs. A few examples:

- **Sent It** -- First application sent
- **Century** -- 100 applications
- **30 Straight** -- Rejection streak of 30
- **Cooked** -- Reached Crispy burnout
- **Restructured** -- Got laid off after accepting an offer
- **It's Who You Know** -- Got an offer with a referral active
- **The Dream** -- FAANG offer
- **Nightmare Cleared** -- Got an offer on Nightmare difficulty

---

## Save Files

The game auto-saves after every action to `rsim_save.txt` in the same directory as the executable. Achievements persist separately in `rsim_achievements.txt`. The local leaderboard is in `rsim_leaderboard.txt`.

All three are plain text and human-readable.

---

## Global Leaderboard Backend

A .NET 8 minimal API lives in `backend/`. It accepts score submissions on quit and serves a ranked leaderboard.

### Running the backend

```bash
cd backend
dotnet run
```

Serves on `http://localhost:5000` by default.

### Endpoints

```
POST /scores          -- submit a run
GET  /scores          -- top runs (paginated, filterable by track and difficulty)
GET  /scores/stats    -- aggregate stats across all submissions
GET  /scores/{id}     -- single run by ID
```

### Deploying

The backend uses SQLite by default. To switch to Postgres for a production deploy, change the connection string in `appsettings.json`:

```json
"Default": "Host=yourhost;Database=rsim;Username=user;Password=pass"
```

And swap the SQLite package for Npgsql in `RejectionSimBackend.csproj`:

```xml
<PackageReference Include="Npgsql.EntityFrameworkCore.PostgreSQL" Version="8.0.0" />
```

---

## Project Structure

```
rejection_sim/
├── main.cpp              -- game loop, command dispatch, HUD
├── player.h              -- Player struct, ResumeTier, Season, effectiveRate()
├── careers.h             -- SWE / Finance / Consulting tracks, companies, roles, interview questions
├── tracker.h             -- JobBoard, AppStatus, printJobBoard()
├── waiting.h             -- WaitQueue, async pipeline stage resolution
├── burnout.h             -- BurnoutLevel, grind tracking, recovery
├── streaks.h             -- rejection streak tracker and commentary
├── layoff.h              -- post-offer layoff events
├── coverletter.h         -- cover letter strategies and bonuses
├── prep.h                -- one-time interview prep boost
├── events.h              -- one-off market events
├── achievements.h        -- 30 achievements, persistent across resets
├── difficulty.h          -- Easy / Normal / Hard / Nightmare config
├── storymode.h           -- named character, debt counter, story scenarios
├── vent.h                -- vent command, feedback tiers, email sign-offs, competing offers
├── skills.h              -- Leetcode / Portfolio / Clout with XP and leveling
├── networking.h          -- networking action, referral unlock
├── interviews.h          -- ATS filter, phone screen, technical, final round, reference check
├── offer.h               -- offer display, salary ranges, negotiation
├── layoff.h              -- layoff event
├── saveload.h            -- local leaderboard file
├── httpclient.h          -- HTTP POST to global leaderboard (WinHTTP on Windows, curl on Unix)
├── display.h             -- printHelp(), printHopeBar(), printEnergyBar(), color helpers
├── utils.h               -- pickRandom(), sleepMs(), clamp(), randRange(), askYesNo()
├── colors.h              -- ANSI color macros
└── backend/
    ├── Program.cs                  -- .NET 8 minimal API, all endpoints
    ├── RejectionSimBackend.csproj  -- EF Core + SQLite
    └── appsettings.json            -- connection string, port config
```

---

## License

MIT License. See `LICENSE`.

---

*The rejection emails are fictional. The feeling is not.*
