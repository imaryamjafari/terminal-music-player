#include "FavouritesManager.h"
#include "StringUtils.h"
#include <fstream>

FavouritesManager::FavouritesManager(const std::string& filePath) : filePath_(filePath) {
}

void FavouritesManager::load(MusicLibrary& library) {
    std::ifstream file(filePath_);
    if (!file.is_open()) {
        return;
    }

    try {
        std::string line;
        while (std::getline(file, line)) {
            std::string trimmed = StringUtils::trim(line);
            if (trimmed.empty() || trimmed[0] == '#') continue;

            Song* song = library.findByFilePath(trimmed);
            if (song != nullptr) {
                song->setFavourite(true);
            }
        }
    }
    catch (const std::exception&) {}

    file.close();
}

bool FavouritesManager::save(const MusicLibrary& library) const {
    try {
        std::ofstream file(filePath_);
        if (!file.is_open()) return false;

        for (Song* s : library.getAllSongs()) {
            if (s->isFavourite()) {
                file << s->getFilePath() << "\n";
            }
        }
        file.close();
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

void FavouritesManager::toggle(Song* song, const MusicLibrary& library) {
    if (song == nullptr) return;
    song->setFavourite(!song->isFavourite());
    save(library);
}

Playlist FavouritesManager::buildFavouritesPlaylist(const MusicLibrary& library) const {
    Playlist playlist("Favourites");
    for (Song* s : library.getAllSongs()) {
        if (s->isFavourite()) {
            playlist.addSong(s);
        }
    }
    return playlist;
}
