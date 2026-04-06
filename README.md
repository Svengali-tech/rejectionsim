# Rejection Simulator 3000 (C++ ed.)

> A game based on a true story.

A terminal-based game that simulates the modern tech job search experience. Apply to startups, collect rejections, watch your hope bar drain in real time.

---

## Requirements

- A C++ compiler supporting C++17
- **Windows**: [MSYS2](https://www.msys2.org/) with `mingw-w64-ucrt-x86_64-gcc`
- **macOS/Linux**: g++ (usually pre-installed or via `brew install gcc` / `apt install g++`)

---

## Installation

### Windows (MSYS2)

1. Download and install [MSYS2](https://www.msys2.org/)
2. Open the **MSYS2 UCRT64** terminal
3. Install g++:
   ```bash
   pacman -S mingw-w64-ucrt-x86_64-gcc
   ```
4. Navigate to the project folder:
   ```bash
   cd /c/Users/yourname/rejectionsim
   ```
5. Compile:
   ```bash
   g++ -std=c++17 -O2 -o rejection_sim.exe rejection_sim.cpp
   ```
6. Run:
   ```bash
   ./rejection_sim.exe
   ```

### macOS / Linux

```bash
g++ -std=c++17 -O2 -o rejection_sim rejection_sim.cpp
./rejection_sim
```

---

## How to Play

| Command | Action |
|---------|--------|
| `a` | Apply to a job |
| `r` | Reset all stats |
| `q` | Quit and see final stats |

Each application results in one of three outcomes:

- **Rejected** — a polished, HR-approved way of saying no (-12% hope)
- **Ghosted** — silence. just silence. (-8% hope)
- **Recruiter spam** — technically a response (+2% hope, barely)

---

## Milestones

Survive long enough and the game acknowledges your suffering:

| Applications | Milestone |
|---|---|
| 5 | Just warming up |
| 10 | Easy Apply is muscle memory |
| 20 | Cover letter is basically a template |
| 30 | More data on hiring cycles than most analysts |
| 50 | Applied to the same company twice by accident |
| 75 | Checking the portal before breakfast |
| 100 | A century of unanswered effort. Legendary. |

---

## Hope Bar

Starts at 100%. Drains with every rejection and ghost. Color indicates current status:

- **Green** — still believe
- **Yellow** — starting to question things
- **Red** — the market is winning

Hits zero and the game tells you so. You can still keep playing.

---

## Notes

- If you're on Windows and see `Exec format error`, you have a Linux binary in your folder. Delete it with `rm rejection_sim` and recompile using the steps above.
- The 5% recruiter spam rate is probably generous compared to real life.

---

*Built for laughs. Inspired by real events.*
