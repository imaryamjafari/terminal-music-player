#include "PlayHistoryManager.h"
#include "StringUtils.h"
#include <fstream>
#include <algorithm>
#include <vector>

PlayHistoryManager::PlayHistoryManager(const std::string& filePath) : filePath_(filePath) {
}

void PlayHistoryManager::load(MusicLibrary& library) {
    std::ifstream file(filePath_);
    if (!file.is_open()) 
        return;

    try {
        std::string line;
        while (std::getline(file, line)) {
            std::string trimmed = StringUtils::trim(line);
            if (trimmed.empty() || trimmed[0] == '#') continue;

            size_t eqPos = trimmed.rfind('=');
            if (eqPos == std::string::npos) continue;

            std::string path = StringUtils::trim(trimmed.substr(0, eqPos));
            std::string countStr = StringUtils::trim(trimmed.substr(eqPos + 1));

            int count = 0;
            if (!StringUtils::tryParseInt(countStr, count) || count < 0) continue;

            Song* song = library.findByFilePath(path);
            if (song != nullptr) {
                song->setPlayCount(count);
            }
        }
    }
    catch (const std::exception&) {}

    file.close();
}

bool PlayHistoryManager::save(const MusicLibrary& library) const {
    try {
        std::ofstream file(filePath_);
        if (!file.is_open()) return false;

        for (Song* s : library.getAllSongs()) 
            if (s->getPlayCount() > 0) 
                file << s->getFilePath() << "=" << s->getPlayCount() << "\n";

        file.close();
        return true;
    }
    catch (const std::exception&) {
        return false;
    }
}

Playlist PlayHistoryManager::buildMostPlayedPlaylist(const MusicLibrary& library, size_t maxItems) const {
    Playlist playlist("Most Played");

    std::vector<Song*> candidates;
    for (Song* s : library.getAllSongs())
        if (s->getPlayCount() > 0) candidates.push_back(s);
    
    std::sort(candidates.begin(), candidates.end(), [](Song* a, Song* b) {
        return a->getPlayCount() > b->getPlayCount();
    });

    size_t limit = std::min(maxItems, candidates.size());
    for (size_t i = 0; i < limit; i++) {
        playlist.addSong(candidates[i]);
    }

    return playlist;
}
