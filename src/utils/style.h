#ifndef STYLE_H
#define STYLE_H

#include <string>

/**
 * Kid: "Mom, can we have Tailwind?"
 * Mom: "No, we have Tailwind at home."
 * Tailwind at home: style.h
 *
 * @warning Requires ANSI-compatible terminal support
 *
 * @author y4nked/kiwz
 * @notes This code doesn't include the full capabilities of ANSI escapes,
 * please feel free to add it by your self from this source:
 * https://gist.github.com/fnky/458719343aabd01cfb17a3a4f7296797 or any other
 * sources.
 *
 * Example usage:
 * #include "style.h"   // Adjust your path
 * // Basic text coloring
 * std::cout << style::BLUE << "This text is blue\n" << style::RESET;
 *
 * // Combining styles
 * std::cout << style::BOLD << style::RED << "Bold red text"
 *           << style::BLUE << " this is bold blue\n" << style::RESET;
 *
 * // Background and text color
 * std::cout << style::BG_GREEN << style::BLACK << "Green background with black
 * text"
 *           << style::RESET << "\n";
 *
 * // Cursor manipulation
 * std::cout << style::HIDE_CURSOR;  // Hide the cursor
 * // ... do something ...
 * std::cout << style::SHOW_CURSOR;  // Show the cursor again
 *
 * // Terminal effects
 * std::cout << style::CLEAR_SCREEN;  // Clear the screen
 * std::cout << style::SOUND;         // Trigger a beep
 */

// Terminal effects
inline const std::string CLEAR_SCREEN =
    "\033c"; // Only clear the shown screen (not really)
inline const std::string SOUND = "\a";

// Cursor manipulation
inline const std::string HIDE_CURSOR = "\033[?25l";
inline const std::string SHOW_CURSOR = "\033[?25h";
inline const std::string BLOCK_CURSOR = "\033[2 q";        // Blinking block
inline const std::string STEADY_BLOCK_CURSOR = "\033[0 q"; // Steady block
inline const std::string UNDERLINE_CURSOR = "\033[4 q";    // Blinking underline
inline const std::string STEADY_UNDERLINE_CURSOR =
    "\033[3 q";                                          // Steady underline
inline const std::string BAR_CURSOR = "\033[6 q";        // Blinking bar
inline const std::string STEADY_BAR_CURSOR = "\033[5 q"; // Steady bar

// Text formatting
inline const std::string RESET = "\033[0m";         // Reset all styles
inline const std::string BOLD = "\033[1m";          // Bold text
inline const std::string DIM = "\033[2m";           // Dim text
inline const std::string ITALIC = "\033[3m";        // Italic text
inline const std::string UNDERLINE = "\033[4m";     // Underline text
inline const std::string BLINK = "\033[5m";         // Blink text (COOL)
inline const std::string REVERSE = "\033[7m";       // Reverse colors
inline const std::string HIDDEN = "\033[8m";        // Hidden text
inline const std::string STRIKETHROUGH = "\033[9m"; // Strikethrough text

// Text colors
inline const std::string BLACK = "\033[30m";
inline const std::string RED = "\033[31m";
inline const std::string GREEN = "\033[32m";
inline const std::string YELLOW = "\033[33m";
inline const std::string BLUE = "\033[34m";
inline const std::string MAGENTA = "\033[35m";
inline const std::string CYAN = "\033[36m";
inline const std::string WHITE = "\033[37m";

// Bright text colors
inline const std::string BRIGHT_BLACK = "\033[90m";
inline const std::string BRIGHT_RED = "\033[91m";
inline const std::string BRIGHT_GREEN = "\033[92m";
inline const std::string BRIGHT_YELLOW = "\033[93m";
inline const std::string BRIGHT_BLUE = "\033[94m";
inline const std::string BRIGHT_MAGENTA = "\033[95m";
inline const std::string BRIGHT_CYAN = "\033[96m";
inline const std::string BRIGHT_WHITE = "\033[97m";

// Background colors
inline const std::string BG_BLACK = "\033[40m";
inline const std::string BG_RED = "\033[41m";
inline const std::string BG_GREEN = "\033[42m";
inline const std::string BG_YELLOW = "\033[43m";
inline const std::string BG_BLUE = "\033[44m";
inline const std::string BG_MAGENTA = "\033[45m";
inline const std::string BG_CYAN = "\033[46m";
inline const std::string BG_WHITE = "\033[47m";

// Bright background colors
inline const std::string BG_BRIGHT_BLACK = "\033[100m";
inline const std::string BG_BRIGHT_RED = "\033[101m";
inline const std::string BG_BRIGHT_GREEN = "\033[102m";
inline const std::string BG_BRIGHT_YELLOW = "\033[103m";
inline const std::string BG_BRIGHT_BLUE = "\033[104m";
inline const std::string BG_BRIGHT_MAGENTA = "\033[105m";
inline const std::string BG_BRIGHT_CYAN = "\033[106m";
inline const std::string BG_BRIGHT_WHITE = "\033[107m";

#endif // STYLE_H

