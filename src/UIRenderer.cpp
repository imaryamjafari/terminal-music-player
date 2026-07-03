#include "UIRenderer.h"
#include <iostream>
#include <cstdlib>

void UIRenderer::clearScreen() const {

#ifdef _WIN32
    int result = std::system("cls");
#else
    int result = std::system("clear");
#endif
    (void)result;
}

void UIRenderer::printLine(const std::string& text) const {
    std::cout << text << "\n";
}

void UIRenderer::print(const std::string& text) const {
    std::cout << text;
    std::cout.flush();
}

void UIRenderer::printError(const std::string& message) const {
    std::cout << message << "\n";
}

void UIRenderer::printInfo(const std::string& message) const {
    std::cout << message << "\n";
}

void UIRenderer::drawSeparator(int width) const {
    std::cout << std::string(static_cast<size_t>(width), '-') << "\n";
}

void UIRenderer::drawBox(const std::string& title, int width) const {
    drawSeparator(width);
    std::cout << title << "\n";
    drawSeparator(width);
}

void UIRenderer::printLines(const std::vector<std::string>& lines) const {
    for (const auto& l : lines)
        printLine(l);
}
