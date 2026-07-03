#pragma once

#include "Screen.h"

class Application;
class MainMenuScreen : public Screen {
public:
    explicit MainMenuScreen(Application& app);

    void render() override;
    bool handleInput() override;
    bool shouldQuit() const;

private:
    Application& app_;
    bool quitRequested_;
};
