#pragma once

// ANSI escape codes for terminal colors and styles.
// Include this wherever you need colored output.
// Call resetColor() at the end of any colored line to avoid bleed.

#define RESET     "\033[0m"
#define BOLD      "\033[1m"
#define DIM       "\033[2m"
#define ITALIC    "\033[3m"

// Foreground colors
#define RED       "\033[31m"
#define GREEN     "\033[32m"
#define YELLOW    "\033[33m"
#define BLUE      "\033[34m"
#define MAGENTA   "\033[35m"
#define CYAN      "\033[36m"
#define WHITE     "\033[37m"

// Bright foreground colors
#define BRED      "\033[91m"
#define BGREEN    "\033[92m"
#define BYELLOW   "\033[93m"
#define BBLUE     "\033[94m"
#define BMAGENTA  "\033[95m"
#define BCYAN     "\033[96m"

inline void resetColor() {
    // Prints the reset sequence without a newline.
    // Use at end of colored output blocks.
    printf("%s", RESET);
}
