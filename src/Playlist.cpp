#include "Playlist.h"

Playlist::Playlist(const std::string& name) : name_(name) {
}

const std::string& Playlist::getName() const { return name_; }

void Playlist::setName(const std::string& name) { name_ = name; }

void Playlist::addSong(Song* song) {
    if (song != nullptr) {
        songs_.push_back(song);
    }
}

size_t Playlist::size() const { return songs_.size(); }

bool Playlist::isEmpty() const { return songs_.empty(); }

Song* Playlist::getSong(size_t index) const {
    if (index >= songs_.size()) return nullptr;
    return songs_[index];
}

const std::vector<Song*>& Playlist::getSongs() const { return songs_; }

