#pragma once

#include <string>
#include <vector>
#include "Playlist.h"
#include "MusicLibrary.h"

class M3uLoader {
public:
    static std::vector<Playlist> loadAll(const std::string& dir,
                                          const MusicLibrary& library);

private:
    static Playlist parseFile(const std::string& filePath,
                               const std::string& playlistName,
                               const MusicLibrary& library);
};
