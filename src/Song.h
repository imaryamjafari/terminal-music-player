#pragma once

#include <string>

class Song {
public:
    Song();
    Song(const std::string& title,
         const std::string& artist,
         const std::string& album,
         const std::string& genre,
         int year,
         int durationSec,
         const std::string& filePath);

    const std::string& getTitle() const;
    const std::string& getArtist() const;
    const std::string& getAlbum() const;
    const std::string& getGenre() const;
    int getYear() const;
    int getDurationSec() const;
    const std::string& getFilePath() const;
    long getPlayCount() const;

    void setPlayCount(long count);
    void incrementPlayCount();
    void setFavourite(bool fav);
    bool isFavourite() const;

    std::string getDurationFormatted() const;

private:
    std::string title_;
    std::string artist_;
    std::string album_;
    std::string genre_;
    int year_;
    int durationSec_;
    std::string filePath_;

    long playCount_;
    bool favourite_;
};
