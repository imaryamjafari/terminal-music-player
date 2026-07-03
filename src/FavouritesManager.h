#pragma once

#include <string>
#include <set>
#include "MusicLibrary.h"
#include "Playlist.h"


class FavouritesManager {
public:
    explicit FavouritesManager(const std::string& filePath);

    void load(MusicLibrary& library);
    bool save(const MusicLibrary& library) const;
    void toggle(Song* song, const MusicLibrary& library);
    Playlist buildFavouritesPlaylist(const MusicLibrary& library) const;

private:
    std::string filePath_;
};
