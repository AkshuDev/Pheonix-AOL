#ifndef ERRORHANDLER_H
#define ERRORHANDLER_H

#include <iostream>
#include <string>
#include <ctime>
#include <fstream>
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#elif defined(__linux__)
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#endif

// Debug Settings
extern bool Debug;
// Debug Out is where the debug should show at.
// 0 - No Debug Out.
// 1 - Console Window.
// 2 - GUI Window.
// 3 - Event Manager (Windows) OR log (Other than Windows OS).
// 4 - LOG (Windows) OR None.
// Default -> Console Window.
extern int DebugOut;

// Color Codes
namespace ColorCodes {
    extern const std::string RESET;
    extern const std::string BLACK;
    extern const std::string RED;
    extern const std::string GREEN;
    extern const std::string YELLOW;
    extern const std::string BLUE;
    extern const std::string MAGENTA;
    extern const std::string CYAN;
    extern const std::string WHITE;

    extern const std::string BRIGHT_BLACK;
    extern const std::string BRIGHT_RED;
    extern const std::string BRIGHT_GREEN;
    extern const std::string BRIGHT_YELLOW;
    extern const std::string BRIGHT_BLUE;
    extern const std::string BRIGHT_MAGENTA;
    extern const std::string BRIGHT_CYAN;
    extern const std::string BRIGHT_WHITE;

    extern const std::string BG_BLACK;
    extern const std::string BG_RED;
    extern const std::string BG_GREEN;
    extern const std::string BG_YELLOW;
    extern const std::string BG_BLUE;
    extern const std::string BG_MAGENTA;
    extern const std::string BG_CYAN;
    extern const std::string BG_WHITE;

    extern const std::string BG_BRIGHT_BLACK;
    extern const std::string BG_BRIGHT_RED;
    extern const std::string BG_BRIGHT_GREEN;
    extern const std::string BG_BRIGHT_YELLOW;
    extern const std::string BG_BRIGHT_BLUE;
    extern const std::string BG_BRIGHT_MAGENTA;
    extern const std::string BG_BRIGHT_CYAN;
    extern const std::string BG_BRIGHT_WHITE;

    extern const std::string BOLD;
    extern const std::string UNDERLINE;
    extern const std::string REVERSED;
    extern const std::string CROSSED_OUT;
    extern const std::string ITALIC;

    std::string color256(int colorCode);
    std::string bgColor256(int colorCode);

    extern const std::string LIGHT_GRAY;
    extern const std::string DARK_GRAY;
    extern const std::string LIGHT_BLUE;
    extern const std::string DARK_BLUE;
    extern const std::string LIGHT_GREEN_2;
    extern const std::string DARK_GREEN_2;
    extern const std::string LIGHT_RED_2;
    extern const std::string DARK_RED_2;
    extern const std::string ORANGE;
    extern const std::string PURPLE;
    extern const std::string PINK;
    extern const std::string TEAL;
    extern const std::string OLIVE;
    extern const std::string VIOLET;
    extern const std::string GOLD;
    extern const std::string SILVER;
    extern const std::string BRONZE;
    extern const std::string NAVY;
    extern const std::string LIME;
    extern const std::string MAROON;
    extern const std::string TURQUOISE;
    extern const std::string CORAL;
    extern const std::string LAVENDER;
    extern const std::string MUSTARD;
    extern const std::string SLATE_GRAY;
    extern const std::string STEEL_BLUE;
    extern const std::string FOREST_GREEN;
    extern const std::string CRIMSON;
    extern const std::string TAN;
    extern const std::string BEIGE;
    extern const std::string PLUM;
    extern const std::string SALMON;
    extern const std::string KHAKI;
    extern const std::string INDIGO;
    extern const std::string AMBER;
    extern const std::string SIENNA;
    extern const std::string UMBER;
    extern const std::string AZURE;
    extern const std::string FUCHSIA;
    extern const std::string CHARTREUSE;
    extern const std::string AQUAMARINE;
    extern const std::string BURNT_ORANGE;
    extern const std::string DARK_TURQUOISE;
    extern const std::string FIREBRICK;
    extern const std::string HONEYDEW;
    extern const std::string LEMON_CHIFFON;
    extern const std::string MEDIUM_PURPLE;
    extern const std::string POWDER_BLUE;
    extern const std::string SPRING_GREEN;
}

// Function to get the current time as a string
std::string GetFormattedTime();

// Function to log data to a file.
bool LogToFile(const std::string &message, bool endl);
#ifdef _WIN32
// Function to log to windows event manager
bool LogToEvent(const std::string &message);
// Debugs the content to the specified Debug Out and using the specified format.
// Formats:
// 0 -> Just message
// 1 -> [Time]: [message]\n\t[description]\n\tLevel: [category]\n\tCode: [DebugCode]\n
// 2 -> Debug: [message]\n\t[description]\n\tLevel: [category]\n\tCode: [DebugCode]\n
// 3 -> [message]\n\t[description]\n\tLevel: [category]\n\tCode: [DebugCode] (No time)
// 4 -> [Time]: [message]\n\tLevel: [category]\n\tCode: [DebugCode] (No description)
// 5 -> [Time]\n\t[description]\n\tLevel: [category]\n\tCode: [DebugCode] (No message)
// 6 -> [Time]: [message] [description] Level: [category] Code: [DebugCode] (Single-line format, no endline)
bool Debug_(const std::string message, const std::string description, const std::string category, const std::string DebugCode, const bool Time, int Format);

#elif defined(__linux__)

void ShowMessage(const char* message);
// Debugs the content to the specified Debug Out and using the specified format.
// Formats:
// 0 -> Just message
// 1 -> [Time]: [message]\n\t[description]\n\tLevel: [category]\n\tCode: [DebugCode]\n
// 2 -> Debug: [message]\n\t[description]\n\tLevel: [category]\n\tCode: [DebugCode]\n
// 3 -> [message]\n\t[description]\n\tLevel: [category]\n\tCode: [DebugCode] (No time)
// 4 -> [Time]: [message]\n\tLevel: [category]\n\tCode: [DebugCode] (No description)
// 5 -> [Time]\n\t[description]\n\tLevel: [category]\n\tCode: [DebugCode] (No message)
// 6 -> [Time]: [message] [description] Level: [category] Code: [DebugCode] (Single-line format, no endline)
bool Debug_(const std::string message, const std::string description, const std::string category, const std::string DebugCode, const bool Time, int Format);
#endif

void throwError(const std::string message, const std::string description, const std::string category, const std::string ErrorCode, const bool Time, int Format, const std::string& color);
#endif