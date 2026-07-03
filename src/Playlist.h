#pragma once

#include <string>
#include <vector>
#include "Song.h"

class Playlist {
public:
    explicit Playlist(const std::string& name = "");

    const std::string& getName() const;
    void setName(const std::string& name);

    void addSong(Song* song);

    size_t size() const;
    bool isEmpty() const;

    Song* getSong(size_t index) const;

    const std::vector<Song*>& getSongs() const;

private:
    std::string name_;
    std::vector<Song*> songs_;
};
