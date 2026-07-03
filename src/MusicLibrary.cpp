#include "MusicLibrary.h"
#include <algorithm>

MusicLibrary::MusicLibrary() {
}

MusicLibrary::~MusicLibrary() {
    for (Song* s : songs_) {
        delete s;
    }
    songs_.clear();
    pathIndex_.clear();
}

Song* MusicLibrary::addSong(const std::string& title,
                             const std::string& artist,
                             const std::string& album,
                             const std::string& genre,
                             int year,
                             int durationSec,
                             const std::string& filePath) {
    Song* newSong = new Song(title, artist, album, genre, year, durationSec, filePath);
    songs_.push_back(newSong);
    pathIndex_[filePath] = newSong;
    return newSong;
}

Song* MusicLibrary::findByFilePath(const std::string& filePath) const {
    auto it = pathIndex_.find(filePath);
    if (it == pathIndex_.end()) return nullptr;
    return it->second;
}

size_t MusicLibrary::size() const { return songs_.size(); }

const std::vector<Song*>& MusicLibrary::getAllSongs() const { return songs_; }

std::vector<Song*> MusicLibrary::filterByArtist(const std::string& artist) const {
    std::vector<Song*> result;
    for (Song* s : songs_) {
        if (s->getArtist() == artist) result.push_back(s);
    }
    return result;
}

std::vector<Song*> MusicLibrary::filterByAlbum(const std::string& album) const {
    std::vector<Song*> result;
    for (Song* s : songs_) {
        if (s->getAlbum() == album) result.push_back(s);
    }
    return result;
}
