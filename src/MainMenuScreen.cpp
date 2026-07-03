#include "MainMenuScreen.h"
#include "Application.h"
#include "NowPlayingScreen.h"
#include "PlaylistListScreen.h"
#include "PlaylistViewScreen.h"
#include "SettingsScreen.h"

MainMenuScreen::MainMenuScreen(Application& app) : app_(app), quitRequested_(false) {
}

void MainMenuScreen::render() {
    UIRenderer& ui = app_.getRenderer();
    ui.clearScreen();

    ui.printLine("=====================================================================");
    ui.printLine("                    ~ Terminal Music Player ~");
    ui.printLine("=====================================================================");

    std::string lastSongPath = app_.getConfig().get("last_song", "");
    if (!lastSongPath.empty()) {
        Song* lastSong = app_.getLibrary().findByFilePath(lastSongPath);
        if (lastSong != nullptr) {
            ui.printLine("Last played: " + lastSong->getTitle() + " - " + lastSong->getArtist());
            ui.printLine("---------------------------------------------------------------------");
        }
    }

    ui.printLine("  1. Now Playing");
    ui.printLine("  2. Playlists");
    ui.printLine("  3. Browse Playlist");
    ui.printLine("  4. Settings");
    ui.printLine("---------------------------------------------------------------------");
    ui.printLine("  0. Quit (saves state)");
    ui.printLine("=====================================================================");
}

bool MainMenuScreen::handleInput() {
    int choice = app_.getInput().readInt(0, 4, "Enter choice: ");

    switch (choice) {
        case 0: {
            quitRequested_ = true;
            return false;
        }
        case 1: {
            NowPlayingScreen screen(app_);
            screen.run();
            return true;
        }
        case 2: {
            PlaylistListScreen screen(app_);
            screen.render();
            while (true) {
                app_.getPlayer().tick();
                screen.render();
                if (!screen.handleInput()) break;
            }
            return true;
        }
        case 3: {
            const Playlist* active = app_.getPlayer().getActivePlaylist();
            if (active == nullptr && !app_.getPlaylists().empty()) {
                app_.getPlayer().setActivePlaylist(&app_.getPlaylists()[0]);
                active = app_.getPlayer().getActivePlaylist();
            }
            if (active == nullptr) {
                app_.getRenderer().printError("No playlists available.");
                return true;
            }
            PlaylistViewScreen screen(app_);
            screen.render();
            while (true) {
                app_.getPlayer().tick();
                screen.render();
                if (!screen.handleInput()) break;
            }
            if (screen.wantsNowPlaying()) {
                NowPlayingScreen npScreen(app_);
                npScreen.run();
            }
            return true;
        }
        case 4: {
            SettingsScreen screen(app_);
            screen.render();
            while (true) {
                app_.getPlayer().tick();
                screen.render();
                if (!screen.handleInput()) break;
            }
            return true;
        }
        default:
            return true;
    }
}

bool MainMenuScreen::shouldQuit() const { return quitRequested_; }
