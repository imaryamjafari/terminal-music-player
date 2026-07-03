#include "InputHandler.h"
#include "StringUtils.h"
#include <iostream>
#include <limits>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>

InputHandler::InputHandler() {}

void InputHandler::clearCinAfterFailure() const {
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

std::string InputHandler::readLine() const {
    std::string line;
    if (!std::getline(std::cin, line)) {
        clearCinAfterFailure();
        return "";
    }
    return StringUtils::trim(line);
}

int InputHandler::readInt(int min, int max, const std::string& prompt) const {
    while (true) {
        std::cout << prompt;
        std::cout.flush();

        std::string line;
        if (!std::getline(std::cin, line)) {
            clearCinAfterFailure();
            std::cout << "Invalid choice. Please try again.\n";
            continue;
        }

        std::string trimmed = StringUtils::trim(line);
        int value = 0;
        if (!StringUtils::tryParseInt(trimmed, value)) {
            std::cout << "Invalid choice. Please try again.\n";
            continue;
        }

        if (value < min || value > max) {
            std::cout << "Invalid choice. Please try again.\n";
            continue;
        }

        return value;
    }
}

char InputHandler::readKey(const std::string& prompt) const {
    std::cout << prompt;
    std::cout.flush();

    std::string line;
    if (!std::getline(std::cin, line)) {
        clearCinAfterFailure();
        return '\0';
    }

    std::string trimmed = StringUtils::trim(line);
    if (trimmed.empty()) return '\0';
    return trimmed[0];
}

void InputHandler::setTerminalRawMode(bool enable) const {
    static termios oldt;
    static bool saved = false;

    if (enable) {
        if (!saved) {
            tcgetattr(STDIN_FILENO, &oldt);
            saved = true;
        }

        termios newt = oldt;
        newt.c_lflag &= ~(ICANON | ECHO);

        tcsetattr(STDIN_FILENO, TCSANOW, &newt);

        int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
        fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK);
    }
    else {
        if (saved)
            tcsetattr(STDIN_FILENO, TCSANOW, &oldt);

        int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
        flags &= ~O_NONBLOCK;
        fcntl(STDIN_FILENO, F_SETFL, flags);
    }
}

bool InputHandler::pollKey(char& key) const {
    char c;
    if (read(STDIN_FILENO, &c, 1) == 1) {
        key = c;
        return true;
    }

    return false;
}
