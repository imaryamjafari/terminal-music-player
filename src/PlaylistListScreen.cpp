#include "PlaylistListScreen.h"
#include "Application.h"
#include <iomanip>
#include <sstream>

PlaylistListScreen::PlaylistListScreen(Application& app) : app_(app) {
}

void PlaylistListScreen::render() {
    UIRenderer& ui = app_.getRenderer();
    ui.clearScreen();

    ui.printLine("=====================================================================");
    ui.printLine("                          Playlists");
    ui.printLine("=====================================================================");

    std::ostringstream header;
    header << std::left << std::setw(4) << "#" << std::setw(29) << "Name" << "Songs";
    ui.printLine(header.str());
    ui.printLine("---------------------------------------------------------------------");

    int activeIdx = app_.getActivePlaylistIndex();
    auto& playlists = app_.getPlaylists();

    if (playlists.empty()) {
        ui.printLine("  (No playlists found in Data/Playlists/)");
    }

    for (size_t i = 0; i < playlists.size(); i++) {
        std::ostringstream row;
        std::string name = playlists[i].getName();
        if (name.size() > 28) name = name.substr(0, 25) + "...";
        row << std::left << std::setw(4) << (i + 1)
            << std::setw(29) << name
            << playlists[i].size();
        if (static_cast<int>(i) == activeIdx) {
            row << " [active]";
        }
        ui.printLine(row.str());
    }

    ui.printLine("---------------------------------------------------------------------");
    ui.printLine("Enter number to switch active playlist. [0] back");
    ui.printLine("=====================================================================");
}

bool PlaylistListScreen::handleInput() {
    auto& playlists = app_.getPlaylists();
    int maxChoice = static_cast<int>(playlists.size());

    int choice = app_.getInput().readInt(0, maxChoice, "Choice: ");

    if (choice == 0) {
        return false;
    }

    size_t idx = static_cast<size_t>(choice - 1);
    int currentActive = app_.getActivePlaylistIndex();

    if (static_cast<int>(idx) != currentActive) {
        app_.getPlayer().setActivePlaylist(&playlists[idx]);
    }

    return true;
}
