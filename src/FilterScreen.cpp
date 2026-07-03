#include "FilterScreen.h"
#include "Application.h"
#include <algorithm>
#include <sstream>

FilterScreen::FilterScreen(Application& app)
    : app_(app), subState_(SubState::CHOOSE_TYPE), isArtist_(true),
      resultReady_(false), exitToFullView_(false) {
}

void FilterScreen::buildDistinctValues() {
    distinctValues_.clear();
    countsPerValue_.clear();

    const Playlist* active = app_.getPlayer().getActivePlaylist();
    if (active == nullptr) return;

    std::vector<std::string> values;
    for (Song* s : active->getSongs()) {
        values.push_back(isArtist_ ? s->getArtist() : s->getAlbum());
    }

    std::vector<std::string> unique = values;
    std::sort(unique.begin(), unique.end());
    unique.erase(std::unique(unique.begin(), unique.end()), unique.end());

    for (const auto& v : unique) {
        int count = static_cast<int>(std::count(values.begin(), values.end(), v));
        distinctValues_.push_back(v);
        countsPerValue_.push_back(count);
    }
}

void FilterScreen::render() {
    UIRenderer& ui = app_.getRenderer();
    const Playlist* active = app_.getPlayer().getActivePlaylist();
    std::string plName = (active != nullptr) ? active->getName() : "(none)";

    ui.clearScreen();
    ui.printLine("=====================================================================");

    if (subState_ == SubState::CHOOSE_TYPE) {
        ui.printLine("Filter songs in: " + plName);
        ui.printLine("---------------------------------------------------------------------");
        ui.printLine("Filter by:");
        ui.printLine("  1. Artist");
        ui.printLine("  2. Album");
        ui.printLine("---------------------------------------------------------------------");
        ui.printLine("  0. Back");
    }
    else {
        std::string typeLabel = isArtist_ ? "Artists" : "Albums";
        ui.printLine(typeLabel + " in " + plName);
        ui.printLine("---------------------------------------------------------------------");
        for (size_t i = 0; i < distinctValues_.size(); i++) {
            std::ostringstream row;
            row << "  " << (i + 1) << ". " << distinctValues_[i]
                << " (" << countsPerValue_[i] << " songs)";
            ui.printLine(row.str());
        }
        ui.printLine("---------------------------------------------------------------------");
        ui.printLine("  0. Back");
    }
    ui.printLine("=====================================================================");
}

bool FilterScreen::handleInput() {
    UIRenderer& ui = app_.getRenderer();

    if (subState_ == SubState::CHOOSE_TYPE) {
        int choice = app_.getInput().readInt(0, 2, "Choice: ");
        if (choice == 0) {
            exitToFullView_ = true;
            return false;
        }
        isArtist_ = (choice == 1);
        buildDistinctValues();
        if (distinctValues_.empty()) {
            ui.printError("No values available to filter.");
            return true;
        }
        subState_ = SubState::CHOOSE_VALUE;
        return true;
    }
    else {
        int maxChoice = static_cast<int>(distinctValues_.size());
        int choice = app_.getInput().readInt(0, maxChoice, "Choice: ");
        if (choice == 0) {
            exitToFullView_ = true;
            resultReady_ = false;
            return false;
        }

        std::string selectedValue = distinctValues_[static_cast<size_t>(choice - 1)];
        const Playlist* active = app_.getPlayer().getActivePlaylist();

        filteredSongs_.clear();
        if (active != nullptr) {
            for (Song* s : active->getSongs()) {
                const std::string& field = isArtist_ ? s->getArtist() : s->getAlbum();
                if (field == selectedValue) {
                    filteredSongs_.push_back(s);
                }
            }
        }

        filterDescription_ = (isArtist_ ? "Artist: " : "Album: ") + selectedValue;
        resultReady_ = true;
        exitToFullView_ = false;
        return false;
    }
}

bool FilterScreen::hasResult() const { return resultReady_; }
const std::vector<Song*>& FilterScreen::getFilteredSongs() const { return filteredSongs_; }
const std::string& FilterScreen::getFilterDescription() const { return filterDescription_; }
