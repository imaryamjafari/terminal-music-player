#pragma once

#include "Screen.h"

class Application;

class SettingsScreen : public Screen {
public:
    explicit SettingsScreen(Application& app);

    void render() override;
    bool handleInput() override;

private:
    Application& app_;
};
