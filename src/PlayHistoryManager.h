#pragma once

#include <string>
#include "MusicLibrary.h"
#include "Playlist.h"

class PlayHistoryManager {
public:
    explicit PlayHistoryManager(const std::string& filePath);

    void load(MusicLibrary& library);
    bool save(const MusicLibrary& library) const;

    Playlist buildMostPlayedPlaylist(const MusicLibrary& library, size_t maxItems = 20) const;

private:
    std::string filePath_;
};
