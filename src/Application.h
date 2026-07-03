#pragma once

#include <memory>
#include <vector>
#include <string>
#include <thread>
#include <atomic>
#include "MusicLibrary.h"
#include "Playlist.h"
#include "Player.h"
#include "ConfigManager.h"
#include "UIRenderer.h"
#include "InputHandler.h"
#include "FavouritesManager.h"
#include "PlayHistoryManager.h"

class Application {
public:
    Application();
    ~Application(){};

    void run();

    MusicLibrary& getLibrary();
    std::vector<Playlist>& getPlaylists();
    Player& getPlayer();
    ConfigManager& getConfig();
    UIRenderer& getRenderer();
    InputHandler& getInput();

    int findPlaylistIndexByName(const std::string& name) const;

    int getActivePlaylistIndex() const;

    void saveState();

    FavouritesManager& getFavouritesManager();
    PlayHistoryManager& getPlayHistoryManager();

    void refreshVirtualPlaylists();

private:
    void init();
    void shutdown();

    MusicLibrary library_;
    std::vector<Playlist> playlists_;
    std::unique_ptr<Player> player_;
    ConfigManager config_;
    UIRenderer renderer_;
    InputHandler input_;

    FavouritesManager favouritesManager_;
    PlayHistoryManager playHistoryManager_;

    bool running_;

    std::atomic<bool> tickThreadRunning_{false};
    std::thread tickThread_;

    static const std::string kLibraryCsvPath;
    static const std::string kSettingsCfgPath;
    static const std::string kPlaylistsDirPath;
    static const std::string kFavouritesPath;
    static const std::string kPlayHistoryPath;
};