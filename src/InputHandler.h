#pragma once
#include <string>

class InputHandler {
public:
    InputHandler();

    std::string readLine() const;
    int readInt(int min, int max, const std::string& prompt) const;
    char readKey(const std::string& prompt) const;
    bool pollKey(char& key) const;
    void setTerminalRawMode(bool enable) const;

private:
    void clearCinAfterFailure() const;
};
