#pragma once

#include <string>
#include <vector>
#include <memory>
#include <mutex>
#include "Song.h"
#include "Playlist.h"
#include "IAudioBackend.h"

enum class PlayerState {
    STOPPED,
    PLAYING,
    PAUSED
};

enum class PlaybackMode {
    NO_REPEAT,
    REPEAT_ONE,
    REPEAT_ALL,
    SHUFFLE
};

std::string playbackModeToString(PlaybackMode mode);
PlaybackMode playbackModeFromString(const std::string& s);

class Player {
public:
    explicit Player(std::unique_ptr<IAudioBackend> backend);
    ~Player();

    Player(const Player&) = delete;
    Player& operator=(const Player&) = delete;

    void setActivePlaylist(const Playlist* playlist);
    const Playlist* getActivePlaylist() const;

    bool play();
    void pause();
    void resume();
    void stop();
    bool next();
    bool previous();

    bool setCurrentIndex(size_t index);

    void tick();

    PlayerState getState() const;
    PlaybackMode getMode() const;
    void setMode(PlaybackMode mode);
    Song* getCurrentSong() const;
    int getCurrentIndex() const;
    float getCursorSeconds() const;
    float getLengthSeconds() const;

    Snapshot getSnapshot() const;

private:
    mutable std::mutex mutex_;

    std::unique_ptr<IAudioBackend> backend_;
    const Playlist* activePlaylist_;
    int currentIndex_;
    PlayerState state_;
    PlaybackMode mode_;

    std::vector<int> shuffleHistory_;

    struct Snapshot {
        PlayerState state;
        PlaybackMode mode;
        Song* currentSong;
        std::string playlistName;
        float cursorSeconds;
        float lengthSeconds;
    };

    bool playLocked();
    void pauseLocked();
    void resumeLocked();
    void stopLocked();
    bool nextLocked();
    bool previousLocked();

    bool loadIndexIntoBackendLocked(size_t index);
    int computeNextIndexLocked() const;
    int computePreviousIndexLocked() const;
};
