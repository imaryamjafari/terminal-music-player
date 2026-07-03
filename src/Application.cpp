#include "chrono"
#include "Application.h"
#include "CsvLoader.h"
#include "M3uLoader.h"
#include "MainMenuScreen.h"
#include "MiniaudioBackend.h"
#include <iostream>

const std::string Application::kLibraryCsvPath = "Data/library.csv";
const std::string Application::kSettingsCfgPath = "Data/settings.cfg";
const std::string Application::kPlaylistsDirPath = "Data/Playlists/";
const std::string Application::kFavouritesPath = "Data/favourites.txt";
const std::string Application::kPlayHistoryPath = "Data/play_history.txt";

Application::Application()
    : config_(kSettingsCfgPath),
      favouritesManager_(kFavouritesPath),
      playHistoryManager_(kPlayHistoryPath),
      running_(false) {
}


void Application::init() {
    CsvLoader::load(kLibraryCsvPath, library_);

    playlists_ = M3uLoader::loadAll(kPlaylistsDirPath, library_);

    favouritesManager_.load(library_);
    playHistoryManager_.load(library_);
    playlists_.push_back(favouritesManager_.buildFavouritesPlaylist(library_));
    playlists_.push_back(playHistoryManager_.buildMostPlayedPlaylist(library_));
    
    std::unique_ptr<IAudioBackend> backend = std::make_unique<MiniaudioBackend>();

    player_ = std::make_unique<Player>(std::move(backend));

    tickThreadRunning_ = true;
    tickThread_ = std::thread([this]() {
        while (tickThreadRunning_) {
            player_->tick();
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }
    });
    config_.load();

    std::string savedPlaylistName = config_.get("active_playlist", "");
    if (!savedPlaylistName.empty()) {
        int idx = findPlaylistIndexByName(savedPlaylistName);
        if (idx >= 0) {
            player_->setActivePlaylist(&playlists_[static_cast<size_t>(idx)]);
        }
    } else if (!playlists_.empty()) {
        player_->setActivePlaylist(&playlists_[0]);
    }

    std::string savedMode = config_.get("playback_mode", "NO_REPEAT");
    player_->setMode(playbackModeFromString(savedMode));
}

void Application::shutdown() {
    tickThreadRunning_ = false;

    if (tickThread_.joinable()) tickThread_.join();

    saveState();
    favouritesManager_.save(library_);
    playHistoryManager_.save(library_);
}

void Application::saveState() {
    const Playlist* active = player_->getActivePlaylist();
    if(active != nullptr) {
        config_.set("active_playlist", active->getName());
    }
    config_.set("playback_mode", playbackModeToString(player_->getMode()));

    Song* current = player_->getCurrentSong();
    if(current != nullptr) {
        config_.set("last_song", current->getFilePath());
    }

    config_.save();
}

void Application::run() {
    init();
    running_ = true;

    MainMenuScreen mainMenu(*this);
    while (running_) {
        mainMenu.render();
        bool stay = mainMenu.handleInput();
        if (!stay) {
            if (mainMenu.shouldQuit()) {
                running_ = false;
            }
        }
    }

    shutdown();
    renderer_.printLine("Goodbye!");
}

MusicLibrary& Application::getLibrary() { return library_; }
std::vector<Playlist>& Application::getPlaylists() { return playlists_; }
Player& Application::getPlayer() { return *player_; }
ConfigManager& Application::getConfig() { return config_; }
UIRenderer& Application::getRenderer() { return renderer_; }
InputHandler& Application::getInput() { return input_; }

int Application::findPlaylistIndexByName(const std::string& name) const {
    for (size_t i = 0; i < playlists_.size(); i++) {
        if (playlists_[i].getName() == name) return static_cast<int>(i);
    }
    return -1;
}

int Application::getActivePlaylistIndex() const {
    const Playlist* active = player_->getActivePlaylist();
    if (active == nullptr) return -1;
    for (size_t i = 0; i < playlists_.size(); i++) {
        if (&playlists_[i] == active) return static_cast<int>(i);
    }
    return -1;
}

FavouritesManager& Application::getFavouritesManager() { return favouritesManager_; }
PlayHistoryManager& Application::getPlayHistoryManager() { return playHistoryManager_; }

void Application::refreshVirtualPlaylists() {
    int favIdx = findPlaylistIndexByName("Favourites");
    if (favIdx >= 0) {
        playlists_[static_cast<size_t>(favIdx)] =
            favouritesManager_.buildFavouritesPlaylist(library_);
    }
    /*
    int historyIdx = findPlaylistIndexByName("Most Played");
    if (historyIdx >= 0) {
        playlists_[static_cast<size_t>(historyIdx)] =
            playHistoryManager_.buildMostPlayedPlaylist(library_);
    }
    */
}
