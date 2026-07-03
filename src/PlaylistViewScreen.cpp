#include "PlaylistViewScreen.h"
#include "Application.h"
#include "FilterScreen.h"
#include "StringUtils.h"
#include <algorithm>
#include <sstream>
#include <iomanip>

PlaylistViewScreen::PlaylistViewScreen(Application& app)
    : app_(app), sortField_(SortField::NONE), sortDescending_(false),
      searchQuery_(""), filterActive_(false), filterResultSongs_(), filterDescription_("") {
}

PlaylistViewScreen::FavouriteToggleResult PlaylistViewScreen::tryHandleFavouriteToggle(
    const std::string& raw, const std::vector<Song*>& displayList) {
    if (raw.size() < 2) return FavouriteToggleResult::NotMatched;
    char first = raw[0];
    if (first != 'v' && first != 'V') return FavouriteToggleResult::NotMatched;

    std::string numberPart = raw.substr(1);
    int value = 0;
    if (!StringUtils::tryParseInt(numberPart, value)) return FavouriteToggleResult::NotMatched;

    if (value < 1 || static_cast<size_t>(value) > displayList.size())
        return FavouriteToggleResult::IndexOutOfRange;
    

    Song* song = displayList[static_cast<size_t>(value - 1)];
    app_.getFavouritesManager().toggle(song, app_.getLibrary());
    app_.refreshVirtualPlaylists();
    return FavouriteToggleResult::Success;
}

std::vector<Song*> PlaylistViewScreen::computeDisplayList() const {
    std::vector<Song*> base;

    if (filterActive_) 
        base = filterResultSongs_;
    else {
        const Playlist* active = app_.getPlayer().getActivePlaylist();
        if (active != nullptr)
            base = active->getSongs();
    }

    std::vector<Song*> afterSearch;
    if (searchQuery_.empty()) 
        afterSearch = base;
    else {
        for (Song* s : base) {
            bool match = StringUtils::containsCaseInsensitive(s->getTitle(), searchQuery_) ||
                         StringUtils::containsCaseInsensitive(s->getArtist(), searchQuery_) ||
                         StringUtils::containsCaseInsensitive(s->getAlbum(), searchQuery_);
            if (match) afterSearch.push_back(s);
        }
    }

    std::vector<Song*> result = afterSearch;
    if (sortField_ != SortField::NONE) {
        std::sort(result.begin(), result.end(), [this](Song* a, Song* b) {
            bool less;
            switch (sortField_) {
                case SortField::TITLE:
                    less = StringUtils::toLower(a->getTitle()) < StringUtils::toLower(b->getTitle());
                    break;
                case SortField::ARTIST:
                    less = StringUtils::toLower(a->getArtist()) < StringUtils::toLower(b->getArtist());
                    break;
                case SortField::ALBUM:
                    less = StringUtils::toLower(a->getAlbum()) < StringUtils::toLower(b->getAlbum());
                    break;
                case SortField::YEAR:
                    less = a->getYear() < b->getYear();
                    break;
                case SortField::DURATION:
                    less = a->getDurationSec() < b->getDurationSec();
                    break;
                default:
                    less = false;
            }
            return sortDescending_ ? !less : less;
        });
    }

    return result;
}

int PlaylistViewScreen::mapDisplayIndexToPlaylistIndex(Song* song) const {
    const Playlist* active = app_.getPlayer().getActivePlaylist();
    if (active == nullptr) return -1;
    const auto& songs = active->getSongs();
    for (size_t i = 0; i < songs.size(); i++) {
        if (songs[i] == song) return static_cast<int>(i);
    }
    return -1;
}

void PlaylistViewScreen::render() {
    UIRenderer& ui = app_.getRenderer();
    const Playlist* active = app_.getPlayer().getActivePlaylist();
    std::string plName = (active != nullptr) ? active->getName() : "(none)";

    std::vector<Song*> displayList = computeDisplayList();

    ui.clearScreen();
    ui.printLine("=====================================================================");

    if (!searchQuery_.empty()) {
        ui.printLine(plName + " (" + std::to_string(active ? active->size() : 0) +
                     " songs)    Search: \"" + searchQuery_ + "\"");
    } else if (filterActive_) {
        ui.printLine(plName + " - Filtered (" + filterDescription_ + ")");
    } else {
        std::string sortLabel = "Sort: ";
        switch (sortField_) {
            case SortField::NONE: sortLabel += "(none)"; break;
            case SortField::TITLE: sortLabel += "Title"; break;
            case SortField::ARTIST: sortLabel += "Artist"; break;
            case SortField::ALBUM: sortLabel += "Album"; break;
            case SortField::YEAR: sortLabel += "Year"; break;
            case SortField::DURATION: sortLabel += "Duration"; break;
        }
        if (sortField_ != SortField::NONE) {
            sortLabel += sortDescending_ ? " (desc)" : " (asc)";
        }
        ui.printLine(plName + " (" + std::to_string(active ? active->size() : 0) +
                     " songs)    " + sortLabel);
    }

    ui.printLine("---------------------------------------------------------------------");

    std::ostringstream header;
    header << std::left << std::setw(4) << "#" << std::setw(33) << "Title"
           << std::setw(21) << "Artist" << "Dur";
    ui.printLine(header.str());
    ui.printLine("---------------------------------------------------------------------");

    if (displayList.empty()) {
        ui.printLine("  (No songs to display)");
    }

    int currentPlaylistIdx = app_.getPlayer().getCurrentIndex();
    Song* currentSong = app_.getPlayer().getCurrentSong();

    for (size_t i = 0; i < displayList.size(); i++) {
        Song* s = displayList[i];
        std::ostringstream row;
        std::string marker = (s == currentSong) ? "> " : "  ";
        std::string favMark = s->isFavourite() ? "* " : "  ";
        std::string title = s->getTitle();
        if (title.size() > 32) title = title.substr(0, 29) + "...";
        std::string artist = s->getArtist();
        if (artist.size() > 20) artist = artist.substr(0, 17) + "...";

        row << std::left << std::setw(4) << (i + 1)
            << marker << favMark << std::setw(29) << title
            << std::setw(21) << artist
            << s->getDurationFormatted();
        ui.printLine(row.str());
    }

    ui.printLine("---------------------------------------------------------------------");

    if (!searchQuery_.empty()) {
        ui.printLine(std::to_string(displayList.size()) +
                     " result(s). [num] play  [v<num>] favourite  [/] new search  [0] clear search");
    } else if (filterActive_) {
        ui.printLine("[num] play  [v<num>] favourite  [0] back to full playlist  [f] new filter");
    } else {
        ui.printLine("[num] play song  [v<num>] favourite  [s] sort  [f] filter  [/] search");
        ui.printLine("[0] back");
    }
    ui.printLine("=====================================================================");
    (void)currentPlaylistIdx;
}

void PlaylistViewScreen::renderSortSubmenu() {
    UIRenderer& ui = app_.getRenderer();
    ui.printLine("---------------------------------------------------------------------");
    ui.printLine("Sort by: 1.Title  2.Artist  3.Album  4.Year  5.Duration");
    ui.printLine("Add + for descending (e.g. 4+ for Year desc)");
    ui.printLine("=====================================================================");
}

void PlaylistViewScreen::handleSortSubmenu() {
    UIRenderer& ui = app_.getRenderer();
    renderSortSubmenu();

    std::string raw = app_.getInput().readLine();
    while (true) {
        std::string trimmed = StringUtils::trim(raw);
        bool descending = false;
        std::string numberPart = trimmed;

        if (!trimmed.empty() && trimmed.back() == '+') {
            descending = true;
            numberPart = trimmed.substr(0, trimmed.size() - 1);
        }

        int value = 0;
        if (StringUtils::tryParseInt(numberPart, value) && value >= 1 && value <= 5) {
            switch (value) {
                case 1: sortField_ = SortField::TITLE; break;
                case 2: sortField_ = SortField::ARTIST; break;
                case 3: sortField_ = SortField::ALBUM; break;
                case 4: sortField_ = SortField::YEAR; break;
                case 5: sortField_ = SortField::DURATION; break;
            }
            sortDescending_ = descending;
            return;
        }

        ui.printError("Invalid choice. Please try again.");
        ui.print("Sort choice: ");
        raw = app_.getInput().readLine();
    }
}

void PlaylistViewScreen::handleSearchPrompt() {
    UIRenderer& ui = app_.getRenderer();
    ui.print("Search: ");
    std::string query = app_.getInput().readLine();
    searchQuery_ = query;
    if (!searchQuery_.empty()) {
        filterActive_ = false;
    }
}

bool PlaylistViewScreen::handleInput() {
    UIRenderer& ui = app_.getRenderer();
    const Playlist* active = app_.getPlayer().getActivePlaylist();

    std::vector<Song*> displayList = computeDisplayList();

    if (!searchQuery_.empty()) {
        ui.print("Choice: ");
        while (true) {
            std::string raw = app_.getInput().readLine();
            if (raw == "0") {
                searchQuery_.clear();
                return true;
            }
            if (raw == "/") {
                handleSearchPrompt();
                return true;
            }
            {
                auto favResult = tryHandleFavouriteToggle(raw, displayList);
                if (favResult == FavouriteToggleResult::Success) {
                    return true;
                }
                if (favResult == FavouriteToggleResult::IndexOutOfRange) {
                    ui.printError("No song at this index.");
                    ui.print("Choice: ");
                    continue;
                }
            }
            int value = 0;
            if (!StringUtils::tryParseInt(raw, value)) {
                ui.printError("Invalid choice. Please try again.");
                ui.print("Choice: ");
                continue;
            }
            if (value < 1 || static_cast<size_t>(value) > displayList.size()) {
                ui.printError("No song at this index.");
                ui.print("Choice: ");
                continue;
            }
            Song* chosen = displayList[static_cast<size_t>(value - 1)];
            int plIdx = mapDisplayIndexToPlaylistIndex(chosen);
            if (plIdx >= 0) {
                app_.getPlayer().setCurrentIndex(static_cast<size_t>(plIdx));
                app_.getPlayer().play();
            }
            wantsNowPlaying_ = true;
            return false;
        }
    }

    if (filterActive_) {
        ui.print("Choice: ");
        while (true) {
            std::string raw = app_.getInput().readLine();
            if (raw == "0") {
                filterActive_ = false;
                return true;
            }
            if (raw == "f" || raw == "F") {
                FilterScreen filterScreen(app_);
                filterScreen.render();
                while (filterScreen.handleInput()) {
                    filterScreen.render();
                }
                if (filterScreen.hasResult()) {
                    filterResultSongs_ = filterScreen.getFilteredSongs();
                    filterDescription_ = filterScreen.getFilterDescription();
                    filterActive_ = true;
                } else {
                    filterActive_ = false;
                }
                return true;
            }
            {
                auto favResult = tryHandleFavouriteToggle(raw, displayList);
                if (favResult == FavouriteToggleResult::Success) {
                    return true;
                }
                if (favResult == FavouriteToggleResult::IndexOutOfRange) {
                    ui.printError("No song at this index.");
                    ui.print("Choice: ");
                    continue;
                }
            }
            int value = 0;
            if (!StringUtils::tryParseInt(raw, value)) {
                ui.printError("Invalid choice. Please try again.");
                ui.print("Choice: ");
                continue;
            }
            if (value < 1 || static_cast<size_t>(value) > displayList.size()) {
                ui.printError("No song at this index.");
                ui.print("Choice: ");
                continue;
            }
            Song* chosen = displayList[static_cast<size_t>(value - 1)];
            int plIdx = mapDisplayIndexToPlaylistIndex(chosen);
            if (plIdx >= 0) {
                app_.getPlayer().setCurrentIndex(static_cast<size_t>(plIdx));
                app_.getPlayer().play();
            }
            wantsNowPlaying_ = true;
            return false;
        }
    }

    if (active == nullptr || active->isEmpty()) {
        ui.print("Choice: ");
        std::string raw = app_.getInput().readLine();
        if (raw == "0") return false;
        ui.printError("Playlist is empty.");
        return true;
    }

    ui.print("Choice: ");
    while (true) {
        std::string raw = app_.getInput().readLine();
        std::string lower = StringUtils::toLower(raw);

        if (raw == "0") {
            return false;
        }
        if (lower == "s") {
            handleSortSubmenu();
            return true;
        }
        if (lower == "f") {
            FilterScreen filterScreen(app_);
            filterScreen.render();
            while (filterScreen.handleInput()) {
                filterScreen.render();
            }
            if (filterScreen.hasResult()) {
                filterResultSongs_ = filterScreen.getFilteredSongs();
                filterDescription_ = filterScreen.getFilterDescription();
                filterActive_ = true;
            }
            return true;
        }
        if (raw == "/") {
            handleSearchPrompt();
            return true;
        }
        {
            auto favResult = tryHandleFavouriteToggle(raw, displayList);
            if (favResult == FavouriteToggleResult::Success) {
                return true;
            }
            if (favResult == FavouriteToggleResult::IndexOutOfRange) {
                ui.printError("No song at this index.");
                ui.print("Choice: ");
                continue;
            }
        }

        int value = 0;
        if (!StringUtils::tryParseInt(raw, value)) {
            ui.printError("Invalid choice. Please try again.");
            ui.print("Choice: ");
            continue;
        }
        if (value < 1 || static_cast<size_t>(value) > displayList.size()) {
            ui.printError("No song at this index.");
            ui.print("Choice: ");
            continue;
        }

        Song* chosen = displayList[static_cast<size_t>(value - 1)];
        int plIdx = mapDisplayIndexToPlaylistIndex(chosen);
        if (plIdx >= 0) {
            app_.getPlayer().setCurrentIndex(static_cast<size_t>(plIdx));
            app_.getPlayer().play();
        }
        wantsNowPlaying_ = true;
        return false;
    }
}

bool PlaylistViewScreen::wantsNowPlaying() const { return wantsNowPlaying_; }
