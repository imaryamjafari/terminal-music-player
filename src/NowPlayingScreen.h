#pragma once

#include "Screen.h"

class Application;

class NowPlayingScreen : public Screen {
public:
    explicit NowPlayingScreen(Application& app);

    void render() override;
    bool handleInput() override;
    void run();

private:
    Application& app_;

    bool toggleFavouriteIfApplicable();
    bool handleKey(char key);
};
