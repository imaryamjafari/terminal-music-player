#include "SettingsScreen.h"
#include "Application.h"

SettingsScreen::SettingsScreen(Application& app) : app_(app) {
}

void SettingsScreen::render() {
    UIRenderer& ui = app_.getRenderer();
    PlaybackMode mode = app_.getPlayer().getMode();

    ui.clearScreen();
    ui.printLine("=====================================================================");
    ui.printLine("                          Settings");
    ui.printLine("=====================================================================");
    ui.printLine("Playback Mode (current: " + playbackModeToString(mode) + ")");
    ui.printLine("---------------------------------------------------------------------");

    auto marker = [&](PlaybackMode m) { return mode == m ? " <- (active)" : ""; };

    ui.printLine("  1. NO_REPEAT     play in order, stop at end" + std::string(marker(PlaybackMode::NO_REPEAT)));
    ui.printLine("  2. REPEAT_ONE    repeat current song forever" + std::string(marker(PlaybackMode::REPEAT_ONE)));
    ui.printLine("  3. REPEAT_ALL    loop whole playlist" + std::string(marker(PlaybackMode::REPEAT_ALL)));
    ui.printLine("  4. SHUFFLE       random order" + std::string(marker(PlaybackMode::SHUFFLE)));
    ui.printLine("---------------------------------------------------------------------");
    ui.printLine("  0. Back (changes saved automatically)");
    ui.printLine("=====================================================================");
}

bool SettingsScreen::handleInput() {
    int choice = app_.getInput().readInt(0, 4, "Choice: ");

    if (choice == 0) {
        return false;
    }

    PlaybackMode newMode;
    switch (choice) {
        case 1: newMode = PlaybackMode::NO_REPEAT; break;
        case 2: newMode = PlaybackMode::REPEAT_ONE; break;
        case 3: newMode = PlaybackMode::REPEAT_ALL; break;
        case 4: newMode = PlaybackMode::SHUFFLE; break;
        default: return true;
    }

    app_.getPlayer().setMode(newMode);

    app_.getConfig().set("playback_mode", playbackModeToString(newMode));
    app_.getConfig().save();

    app_.getRenderer().printInfo("Playback mode updated.");
    return true;
}
