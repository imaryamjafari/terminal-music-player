#include "M3uLoader.h"
#include "StringUtils.h"
#include <fstream>
#include <algorithm>
#include <filesystem>

namespace fs = std::filesystem;

std::vector<Playlist> M3uLoader::loadAll(const std::string& dir,
                                          const MusicLibrary& library) {
    std::vector<Playlist> playlists;

    if (!fs::exists(dir) || !fs::is_directory(dir)) {
        return playlists;
    }

    std::vector<fs::path> m3uFiles;
    try {
        for (const auto& entry : fs::directory_iterator(dir)) {
            if (!entry.is_regular_file()) continue;
            std::string ext = entry.path().extension().string();
            std::string extLower = StringUtils::toLower(ext);
            if (extLower == ".m3u") 
                m3uFiles.push_back(entry.path());
        }
    } catch (const std::exception&) {
        return playlists;
    }

    std::sort(m3uFiles.begin(), m3uFiles.end());

    for (const auto& p : m3uFiles) {
        std::string playlistName = p.stem().string();
        Playlist pl = parseFile(p.string(), playlistName, library);
        playlists.push_back(pl);
    }

    return playlists;
}

Playlist M3uLoader::parseFile(const std::string& filePath,
                               const std::string& playlistName,
                               const MusicLibrary& library) {
    Playlist playlist(playlistName);

    std::ifstream file(filePath);
    if (!file.is_open()) {
        return playlist;
    }

    std::string line;
    while (std::getline(file, line)) {
        std::string trimmed = StringUtils::trim(line);
        if (trimmed.empty()) continue;
        if (trimmed[0] == '#') continue;

        Song* matched = library.findByFilePath(trimmed);
        if (matched == nullptr) continue;
        playlist.addSong(matched);
    }

    file.close();
    return playlist;
}
