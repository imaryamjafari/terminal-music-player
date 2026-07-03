#include "NowPlayingScreen.h"
#include "Application.h"
#include <iomanip>
#include <sstream>

namespace {

std::string formatSeconds(float totalSeconds) {
    if (totalSeconds < 0.0f)
        totalSeconds = 0.0f;

    int total = static_cast<int>(totalSeconds);
    int mins = total / 60;
    int secs = total % 60;

    std::ostringstream oss;
    oss << std::setw(2) << std::setfill('0') << mins
        << ":"
        << std::setw(2) << std::setfill('0') << secs;

    return oss.str();
}

}

NowPlayingScreen::NowPlayingScreen(Application& app)
    : app_(app) {
}

void NowPlayingScreen::render() {
    UIRenderer& ui = app_.getRenderer();
    Player& player = app_.getPlayer();

    Player::Snapshot snap = player.getSnapshot();

    ui.clearScreen();

    ui.printLine("=====================================================================");
    ui.printLine("                    ~ Terminal Music Player ~");
    ui.printLine("=====================================================================");
    ui.printLine("Now Playing");

    Song* song = snap.currentSong;

    if (song == nullptr) {
        ui.printLine("  (No song selected)");
    } else {
        std::string favMarker = song->isFavourite() ? " (favourite)" : "";

        ui.printLine("  Title  : " + song->getTitle() + favMarker);
        ui.printLine("  Artist : " + song->getArtist());
        ui.printLine("  Album  : " + song->getAlbum() + " [" + std::to_string(song->getYear()) + "]");
        ui.printLine("  Genre  : " + song->getGenre());
    }

    ui.printLine("---------------------------------------------------------------------");

    std::string stateStr;

    switch (snap.state) {
        case PlayerState::PLAYING:
            stateStr = "> PLAYING";
            break;
        case PlayerState::PAUSED:
            stateStr = "|| PAUSED";
            break;
        case PlayerState::STOPPED:
            stateStr = "[] STOPPED";
            break;
    }

    ui.printLine("  " + stateStr + "    Playlist: " +
                 (snap.playlistName.empty() ? "(none)" : snap.playlistName));

    std::string timeStr = "00:00/00:00";

    if (song != nullptr) {
        float pos = snap.cursorSeconds;
        float len = snap.lengthSeconds;

        if (len <= 0.0f)
            len = static_cast<float>(song->getDurationSec());

        timeStr = formatSeconds(pos) + "/" + formatSeconds(len);
    }

    ui.printLine("  Mode: " + playbackModeToString(snap.mode) +
                 "    Time: " + timeStr);

    ui.printLine("---------------------------------------------------------------------");
    ui.printLine("[p] pause/resume [n] next [b] prev [s] stop [v] favourite [q] menu");
    ui.printLine("=====================================================================");
}

bool NowPlayingScreen::toggleFavouriteIfApplicable() {
    Song* song = app_.getPlayer().getCurrentSong();

    if (song == nullptr)
        return false;

    app_.getFavouritesManager().toggle(song, app_.getLibrary());
    app_.refreshVirtualPlaylists();

    return true;
}

bool NowPlayingScreen::handleKey(char key) {
    Player& player = app_.getPlayer();
    UIRenderer& ui = app_.getRenderer();

    switch (key) {
        case 'p':
            if (player.getState() == PlayerState::PLAYING)
                player.pause();
            else if (player.getState() == PlayerState::PAUSED)
                player.resume();
            else if (!player.play())
                ui.printError("Playlist is empty.");
            break;

        case 'n':
            player.next();
            break;

        case 'b':
            player.previous();
            break;

        case 's':
            player.stop();
            break;

        case 'v':
            toggleFavouriteIfApplicable();
            break;

        case 'q':
            return false;
    }
    return true;
}

bool NowPlayingScreen::handleInput() {
    char key = app_.getInput().readKey("Choice: ");

    if (key >= 'A' && key <= 'Z')
        key = static_cast<char>(key - 'A' + 'a');

    if(!handleKey(key)) return false;

    render();
    return true;
}

void NowPlayingScreen::run() {
    app_.getInput().setTerminalRawMode(true);

    while (true) {
        render();
        char c;
        if (app_.getInput().pollKey(c)) {
            if (c >= 'A' && c <= 'Z')
                c += 'a' - 'A';

            if (!handleKey(c))
                break;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }

    app_.getInput().setTerminalRawMode(false);
}