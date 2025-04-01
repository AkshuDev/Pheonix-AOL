#include "errorHandler.h"

// Debug Settings
bool Debug = true;
// Debug Out is where the debug should show at.
// 0 - No Debug Out.
// 1 - Console Window.
// 2 - GUI Window.
// 3 - Event Manager (Windows) OR log (Other than Windows OS).
// 4 - LOG (Windows) OR None.
// Default -> Console Window.
int DebugOut = 1;

namespace ColorCodes {
    // Regular text colors
    const std::string RESET = "\033[0m";
    const std::string BLACK = "\033[30m";
    const std::string RED = "\033[31m";
    const std::string GREEN = "\033[32m";
    const std::string YELLOW = "\033[33m";
    const std::string BLUE = "\033[34m";
    const std::string MAGENTA = "\033[35m";
    const std::string CYAN = "\033[36m";
    const std::string WHITE = "\033[37m";

    // Bright text colors
    const std::string BRIGHT_BLACK = "\033[90m";
    const std::string BRIGHT_RED = "\033[91m";
    const std::string BRIGHT_GREEN = "\033[92m";
    const std::string BRIGHT_YELLOW = "\033[93m";
    const std::string BRIGHT_BLUE = "\033[94m";
    const std::string BRIGHT_MAGENTA = "\033[95m";
    const std::string BRIGHT_CYAN = "\033[96m";
    const std::string BRIGHT_WHITE = "\033[97m";

    // Background colors
    const std::string BG_BLACK = "\033[40m";
    const std::string BG_RED = "\033[41m";
    const std::string BG_GREEN = "\033[42m";
    const std::string BG_YELLOW = "\033[43m";
    const std::string BG_BLUE = "\033[44m";
    const std::string BG_MAGENTA = "\033[45m";
    const std::string BG_CYAN = "\033[46m";
    const std::string BG_WHITE = "\033[47m";

    // Bright background colors
    const std::string BG_BRIGHT_BLACK = "\033[100m";
    const std::string BG_BRIGHT_RED = "\033[101m";
    const std::string BG_BRIGHT_GREEN = "\033[102m";
    const std::string BG_BRIGHT_YELLOW = "\033[103m";
    const std::string BG_BRIGHT_BLUE = "\033[104m";
    const std::string BG_BRIGHT_MAGENTA = "\033[105m";
    const std::string BG_BRIGHT_CYAN = "\033[106m";
    const std::string BG_BRIGHT_WHITE = "\033[107m";

    // Bold, underline, etc.
    const std::string BOLD = "\033[1m";
    const std::string UNDERLINE = "\033[4m";
    const std::string REVERSED = "\033[7m";
    const std::string CROSSED_OUT = "\033[9m";
    const std::string ITALIC = "\033[3m";

    // Extended colors (256 colors)
    std::string color256(int colorCode) {
        return "\033[38;5;" + std::to_string(colorCode) + "m";
    }

    // Extended background colors (256 colors)
    std::string bgColor256(int colorCode) {
        return "\033[48;5;" + std::to_string(colorCode) + "m";
    }

    // Example of 50 different color codes
    const std::string LIGHT_GRAY = color256(246);
    const std::string DARK_GRAY = color256(238);
    const std::string LIGHT_BLUE = color256(117);
    const std::string DARK_BLUE = color256(25);
    const std::string LIGHT_GREEN_2 = color256(119);
    const std::string DARK_GREEN_2 = color256(28);
    const std::string LIGHT_RED_2 = color256(203);
    const std::string DARK_RED_2 = color256(88);
    const std::string ORANGE = color256(208);
    const std::string PURPLE = color256(93);
    const std::string PINK = color256(212);
    const std::string TEAL = color256(51);
    const std::string OLIVE = color256(142);
    const std::string VIOLET = color256(128);
    const std::string GOLD = color256(226);
    const std::string SILVER = color256(244);
    const std::string BRONZE = color256(130);
    const std::string NAVY = color256(17);
    const std::string LIME = color256(118);
    const std::string MAROON = color256(88);
    const std::string TURQUOISE = color256(45);
    const std::string CORAL = color256(209);
    const std::string LAVENDER = color256(183);
    const std::string MUSTARD = color256(178);
    const std::string SLATE_GRAY = color256(102);
    const std::string STEEL_BLUE = color256(67);
    const std::string FOREST_GREEN = color256(34);
    const std::string CRIMSON = color256(196);
    const std::string TAN = color256(137);
    const std::string BEIGE = color256(230);
    const std::string PLUM = color256(139);
    const std::string SALMON = color256(210);
    const std::string KHAKI = color256(180);
    const std::string INDIGO = color256(54);
    const std::string AMBER = color256(214);
    const std::string SIENNA = color256(131);
    const std::string UMBER = color256(94);
    const std::string AZURE = color256(123);
    const std::string FUCHSIA = color256(201);
    const std::string CHARTREUSE = color256(118);
    const std::string AQUAMARINE = color256(122);
    const std::string BURNT_ORANGE = color256(202);
    const std::string DARK_TURQUOISE = color256(37);
    const std::string FIREBRICK = color256(88);
    const std::string HONEYDEW = color256(229);
    const std::string LEMON_CHIFFON = color256(228);
    const std::string MEDIUM_PURPLE = color256(92);
    const std::string POWDER_BLUE = color256(153);
    const std::string SPRING_GREEN = color256(46);
};


// Function to get the current time as a string
std::string GetFormattedTime() {
    time_t now = time(0);
    char buffer[20];
    strftime(buffer, sizeof(buffer), "%H:%M:%S", localtime(&now));
    return std::string(buffer);
}

// Function to log data to a file.
bool LogToFile(const std::string &message, bool endl) {
    std::ofstream logFile("debug.log", std::ios::app);
    if (logFile.is_open()) {
        if (endl) {
            logFile << message << std::endl;
        } else {
            logFile << message;
        }

        logFile.close();
    } else {
        return false;
    }

    return true;
}

#ifdef _WIN32
// Function to log to windows event manager
bool LogToEvent(const std::string &message){
    HANDLE hEventLog = RegisterEventSource(NULL, "Pheonix Engine");
    if (hEventLog) {
        const char* messages[1] = { message.c_str() };
        ReportEvent(hEventLog, EVENTLOG_INFORMATION_TYPE, 0, 0, NULL, 1, 0, messages, NULL);
        DeregisterEventSource(hEventLog);

        return true;
    }

    return false;
}

// Debugs the content to the specified Debug Out and using the specified format.
// Formats:
// 0 -> Just message
// 1 -> [Time]: [message]\n\t[description]\n\tLevel: [category]\n\tCode: [DebugCode]\n
// 2 -> Debug: [message]\n\t[description]\n\tLevel: [category]\n\tCode: [DebugCode]\n
// 3 -> [message]\n\t[description]\n\tLevel: [category]\n\tCode: [DebugCode] (No time)
// 4 -> [Time]: [message]\n\tLevel: [category]\n\tCode: [DebugCode] (No description)
// 5 -> [Time]\n\t[description]\n\tLevel: [category]\n\tCode: [DebugCode] (No message)
// 6 -> [Time]: [message] [description] Level: [category] Code: [DebugCode] (Single-line format, no endline)
bool Debug_(const std::string message, const std::string description, const std::string category, const std::string DebugCode, const bool Time, int Format) {
    if (!Debug || DebugOut == 0) return false;

    std::string output;
    std::string timeStr = (Time) ? GetFormattedTime() : std::string("");

    switch (Format) {
        case 0:
            output = message;
            break;
        case 1:
            output = "[" + timeStr + "]: " + message + "\n\t" + description + "\n\tLevel: " + category + "\n\tCode: " + DebugCode + "\n";
            break;
        case 2:
            output = "Debug: " + message + "\n\t" + description + "\n\tLevel: " + category + "\n\tCode: " + DebugCode + "\n";
            break;
        case 3:
            output = message + "\n\t" + description + "\n\tLevel: " + category + "\n\tCode: " + DebugCode + "\n";
            break;
        case 4:
            output = "[" + timeStr + "]: " + message + "\n\tLevel: " + category + "\n\tCode: " + DebugCode + "\n";
            break;
        case 5:
            output = "[" + timeStr + "]\n\t" + description + "\n\tLevel: " + category + "\n\tCode: " + DebugCode + "\n";
            break;
        case 6:
            output = "[" + timeStr + "]: " + message + " " + description + " Level: " + category + " Code: " + DebugCode;
            break;
        default:
            output = "Invalid debug format.";
            break;
    }

    switch (DebugOut) {
        case 1:
            std::cout << output;
            break;
        case 2:
            MessageBox(NULL, output.c_str(), "Debug Message: PheonixEngine", MB_OK | MB_ICONINFORMATION);
            break;
        case 3:
            LogToEvent(output);
            break;
        case 4:
            LogToFile(output, false);
            break;
        default:
            std::cout << output;
            break;
    }
    return true;
}


#elif defined(__linux__)

void ShowMessage(const char* message) {
    Display *display;
    Window window;
    XEvent event;
    int screen;
    Atom wm_delete;

    // Open connection to the X server
    display = XOpenDisplay(NULL);
    if (display == NULL) {
        fprintf(stderr, "Cannot open display\n");
        exit(1);
    }

    screen = DefaultScreen(display);
    window = XCreateSimpleWindow(display, RootWindow(display, screen), 10, 10, 300, 100, 1,
                                  BlackPixel(display, screen), WhitePixel(display, screen));

    // Set window title
    XStoreName(display, window, "Message Box");

    // Set up the delete window protocol
    wm_delete = XInternAtom(display, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(display, window, &wm_delete, 1);

    // Map (show) the window
    XMapWindow(display, window);

    // Create a graphics context
    GC gc = XCreateGC(display, window, 0, NULL);
    XSetForeground(display, gc, BlackPixel(display, screen));

    // Draw the message
    XDrawString(display, window, gc, 10, 50, message, strlen(message));

    // Event loop
    while (1) {
        XNextEvent(display, &event);
        if (event.type == Expose) {
            XDrawString(display, window, gc, 10, 50, message, strlen(message));
        }
        if (event.type == ClientMessage && (Atom)event.xclient.data.l[0] == wm_delete) {
            break; // Exit on window close
        }
    }

    // Clean up
    XDestroyWindow(display, window);
    XCloseDisplay(display);
}

// Debugs the content to the specified Debug Out and using the specified format.
// Formats:
// 0 -> Just message
// 1 -> [Time]: [message]\n\t[description]\n\tLevel: [category]\n\tCode: [DebugCode]\n
// 2 -> Debug: [message]\n\t[description]\n\tLevel: [category]\n\tCode: [DebugCode]\n
// 3 -> [message]\n\t[description]\n\tLevel: [category]\n\tCode: [DebugCode] (No time)
// 4 -> [Time]: [message]\n\tLevel: [category]\n\tCode: [DebugCode] (No description)
// 5 -> [Time]\n\t[description]\n\tLevel: [category]\n\tCode: [DebugCode] (No message)
// 6 -> [Time]: [message] [description] Level: [category] Code: [DebugCode] (Single-line format, no endline)
bool Debug_(const std::string message, const std::string description, const std::string category, const std::string DebugCode, const bool Time, int Format) {
    if (!Debug || DebugOut == 0) return false;

    std::string output;
    std::string timeStr = (Time) ? GetFormattedTime() : std::string("");

    switch (Format) {
        case 0:
            output = message;
            break;
        case 1:
            output = "[" + timeStr + "]: " + message + "\n\t" + description + "\n\tLevel: " + category + "\n\tCode: " + DebugCode + "\n";
            break;
        case 2:
            output = "Debug: " + message + "\n\t" + description + "\n\tLevel: " + category + "\n\tCode: " + DebugCode + "\n";
            break;
        case 3:
            output = message + "\n\t" + description + "\n\tLevel: " + category + "\n\tCode: " + DebugCode + "\n";
            break;
        case 4:
            output = "[" + timeStr + "]: " + message + "\n\tLevel: " + category + "\n\tCode: " + DebugCode + "\n";
            break;
        case 5:
            output = "[" + timeStr + "]\n\t" + description + "\n\tLevel: " + category + "\n\tCode: " + DebugCode + "\n";
            break;
        case 6:
            output = "[" + timeStr + "]: " + message + " " + description + " Level: " + category + " Code: " + DebugCode;
            break;
        default:
            output = "Invalid debug format.";
            break;
    }

    switch (DebugOut) {
        case 1:
            std::cout << output;
            break;
        case 2:
            ShowMessage(output.c_str());
            break;
        case 3:
            LogToFile(output, false);
            break;
        default:
            std::cout << output;
            break;
    }
    return true;
}
#endif

void throwError(const std::string message, const std::string description, const std::string category, const std::string ErrorCode, const bool Time, int Format, const std::string& color) {
    if (!Debug) return;

    std::string output;
    std::string timeStr = (Time) ? GetFormattedTime() : std::string("");

    switch (Format) {
        case 0:
            output = message;
            break;
        case 1:
            output = "[" + timeStr + "]: " + message + "\n\t" + description + "\n\tLevel: " + category + "\n\tCode: " + ErrorCode + "\n";
            break;
        case 2:
            output = "Error: " + message + "\n\t" + description + "\n\tLevel: " + category + "\n\tCode: " + ErrorCode + "\n";
            break;
        case 3:
            output = message + "\n\t" + description + "\n\tLevel: " + category + "\n\tCode: " + ErrorCode + "\n";
            break;
        case 4:
            output = "[" + timeStr + "]: " + message + "\n\tLevel: " + category + "\n\tCode: " + ErrorCode + "\n";
            break;
        case 5:
            output = "[" + timeStr + "]\n\t" + description + "\n\tLevel: " + category + "\n\tCode: " + ErrorCode + "\n";
            break;
        case 6:
            output = "[" + timeStr + "]: " + message + " " + description + " Level: " + category + " Code: " + ErrorCode;
            break;
        default:
            output = "Invalid error format.";
            break;
    }

    switch (DebugOut) {
        case 1:
            std::cout << color << output << ColorCodes::RESET;
            break;
        case 2:
#ifdef _WIN32
            MessageBox(NULL, output.c_str(), "Error Message: PheonixEngine", MB_OK | MB_ICONERROR);
#elif defined(__linux__)
            ShowMessage(output.c_str());
#endif
            break;
        case 3:
#ifdef _WIN32
            LogToEvent(output);
#else
            LogToFile(output, false);
#endif
            break;
        case 4:
            LogToFile(output, false);
            break;
        default:
            std::cout << color << output << ColorCodes::RESET;
            break;
    }

    if (ErrorCode != "") {
        exit(999);
    } else {
        exit(std::stoi(ErrorCode));
    }
}