#include "Song.h"
#include <sstream>
#include <iomanip>

Song::Song()
    : title_(""), artist_(""), album_(""), genre_(""),
      year_(0), durationSec_(0), filePath_(""),
      playCount_(0), favourite_(false) {}

Song::Song(const std::string& title,
           const std::string& artist,
           const std::string& album,
           const std::string& genre,
           int year,
           int durationSec,
           const std::string& filePath)
    : title_(title), artist_(artist), album_(album), genre_(genre),
      year_(year), durationSec_(durationSec), filePath_(filePath),
      playCount_(0), favourite_(false) {}

const std::string& Song::getTitle() const { return title_; }
const std::string& Song::getArtist() const { return artist_; }
const std::string& Song::getAlbum() const { return album_; }
const std::string& Song::getGenre() const { return genre_; }
int Song::getYear() const { return year_; }
int Song::getDurationSec() const { return durationSec_; }
const std::string& Song::getFilePath() const { return filePath_; }
long Song::getPlayCount() const { return playCount_; }

void Song::setPlayCount(long count) { playCount_ = count; }
void Song::incrementPlayCount() { playCount_++; }
void Song::setFavourite(bool fav) { favourite_ = fav; }
bool Song::isFavourite() const { return favourite_; }

std::string Song::getDurationFormatted() const {
    int mins = durationSec_ / 60;
    int secs = durationSec_ % 60;
    std::ostringstream oss;
    oss << std::setw(2) << std::setfill('0') << mins << ":"
        << std::setw(2) << std::setfill('0') << secs;
    return oss.str();
}
