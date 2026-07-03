#pragma once

#include <string>
#include <vector>
#include <map>
#include "Song.h"

class MusicLibrary {
public:
    MusicLibrary();
    ~MusicLibrary();
    MusicLibrary(const MusicLibrary&) = delete;
    MusicLibrary& operator=(const MusicLibrary&) = delete;

    Song* addSong(const std::string& title,
                  const std::string& artist,
                  const std::string& album,
                  const std::string& genre,
                  int year,
                  int durationSec,
                  const std::string& filePath);

    Song* findByFilePath(const std::string& filePath) const;

    size_t size() const;

    const std::vector<Song*>& getAllSongs() const;

    std::vector<Song*> filterByArtist(const std::string& artist) const;
    std::vector<Song*> filterByAlbum(const std::string& album) const;

private:
    std::vector<Song*> songs_;              
    std::map<std::string, Song*> pathIndex_;  
};
