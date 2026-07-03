#pragma once

#include <string>
#include <vector>

class UIRenderer {
public:
    UIRenderer(){};

    void clearScreen() const;
    void printLine(const std::string& text = "") const;
    void print(const std::string& text) const;
    void printError(const std::string& message) const;
    void printInfo(const std::string& message) const;
    void drawSeparator(int width = 56) const;
    void drawBox(const std::string& title, int width = 56) const;
    void printLines(const std::vector<std::string>& lines) const;
};
