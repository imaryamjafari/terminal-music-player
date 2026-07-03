#pragma once

#include "Screen.h"

class Application;

class PlaylistListScreen : public Screen {
public:
    explicit PlaylistListScreen(Application& app);

    void render() override;
    bool handleInput() override;

private:
    Application& app_;
};
