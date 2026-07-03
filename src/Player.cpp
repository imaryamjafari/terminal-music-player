#include "Player.h"
#include <cstdlib>
#include <ctime>
#include <algorithm>

std::string playbackModeToString(PlaybackMode mode) {
    switch (mode) {
        case PlaybackMode::NO_REPEAT: return "NO_REPEAT";
        case PlaybackMode::REPEAT_ONE: return "REPEAT_ONE";
        case PlaybackMode::REPEAT_ALL: return "REPEAT_ALL";
        case PlaybackMode::SHUFFLE: return "SHUFFLE";
    }
    return "NO_REPEAT";
}

PlaybackMode playbackModeFromString(const std::string& s) {
    if (s == "REPEAT_ONE") return PlaybackMode::REPEAT_ONE;
    if (s == "REPEAT_ALL") return PlaybackMode::REPEAT_ALL;
    if (s == "SHUFFLE") return PlaybackMode::SHUFFLE;
    return PlaybackMode::NO_REPEAT;
}

Player::Player(std::unique_ptr<IAudioBackend> backend)
    : backend_(std::move(backend)),
      activePlaylist_(nullptr),
      currentIndex_(-1),
      state_(PlayerState::STOPPED),
      mode_(PlaybackMode::NO_REPEAT) {
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
}

Player::~Player() {
}

void Player::setActivePlaylist(const Playlist* playlist) {
    std::lock_guard<std::mutex> lock(mutex_);
    stopLocked();
    activePlaylist_ = playlist;
    currentIndex_ = -1;
    shuffleHistory_.clear();
}

const Playlist* Player::getActivePlaylist() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return activePlaylist_;
}

bool Player::setCurrentIndex(size_t index) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (activePlaylist_ == nullptr || activePlaylist_->isEmpty()) return false;
    if (index >= activePlaylist_->size()) return false;
    currentIndex_ = static_cast<int>(index);
    return true;
}

bool Player::play() {
    std::lock_guard<std::mutex> lock(mutex_);
    return playLocked();
}

void Player::pause() {
    std::lock_guard<std::mutex> lock(mutex_);
    pauseLocked();
}

void Player::resume() {
    std::lock_guard<std::mutex> lock(mutex_);
    resumeLocked();
}

void Player::stop() {
    std::lock_guard<std::mutex> lock(mutex_);
    stopLocked();
}

bool Player::next() {
    std::lock_guard<std::mutex> lock(mutex_);
    return nextLocked();
}

bool Player::previous() {
    std::lock_guard<std::mutex> lock(mutex_);
    return previousLocked();
}

void Player::tick() {
    std::lock_guard<std::mutex> lock(mutex_);
    if (state_ != PlayerState::PLAYING) return;
    if (backend_ && backend_->isAtEnd()) {
        nextLocked();
    }
}

PlayerState Player::getState() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return state_;
}

PlaybackMode Player::getMode() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return mode_;
}

void Player::setMode(PlaybackMode mode) {
    std::lock_guard<std::mutex> lock(mutex_);
    mode_ = mode;
}

Song* Player::getCurrentSong() const {
    std::lock_guard<std::mutex> lock(mutex_);
    if (activePlaylist_ == nullptr || currentIndex_ < 0) return nullptr;
    return activePlaylist_->getSong(static_cast<size_t>(currentIndex_));
}

int Player::getCurrentIndex() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return currentIndex_;
}

float Player::getCursorSeconds() const {
    std::lock_guard<std::mutex> lock(mutex_);
    if (!backend_) return 0.0f;
    return backend_->getCursorSeconds();
}

float Player::getLengthSeconds() const {
    std::lock_guard<std::mutex> lock(mutex_);
    if (!backend_) return 0.0f;
    return backend_->getLengthSeconds();
}

Player::Snapshot Player::getSnapshot() const {
    std::lock_guard<std::mutex> lock(mutex_);

    Snapshot snap;
    snap.state = state_;
    snap.mode = mode_;
    snap.currentSong = (activePlaylist_ != nullptr && currentIndex_ >= 0)
                            ? activePlaylist_->getSong(static_cast<size_t>(currentIndex_))
                            : nullptr;
    snap.playlistName = (activePlaylist_ != nullptr) ? activePlaylist_->getName() : "";
    snap.cursorSeconds = backend_ ? backend_->getCursorSeconds() : 0.0f;
    snap.lengthSeconds = backend_ ? backend_->getLengthSeconds() : 0.0f;
    return snap;
}

bool Player::loadIndexIntoBackendLocked(size_t index) {
    if (activePlaylist_ == nullptr) return false;
    if (index >= activePlaylist_->size()) return false;

    Song* song = activePlaylist_->getSong(index);
    if (song == nullptr) return false;

    bool ok = backend_->loadFile(song->getFilePath());
    if (ok) {
        backend_->setKnownLengthHintSeconds(static_cast<float>(song->getDurationSec()));
    }
    return ok;
}

bool Player::playLocked() {
    if (activePlaylist_ == nullptr || activePlaylist_->isEmpty()) {
        return false;
    }

    if (currentIndex_ < 0) {
        currentIndex_ = 0;
    }

    if (!loadIndexIntoBackendLocked(static_cast<size_t>(currentIndex_))) {
        return nextLocked();
    }

    Song* song = activePlaylist_->getSong(static_cast<size_t>(currentIndex_));
    if (song != nullptr) {
        song->incrementPlayCount();
    }

    backend_->start();
    state_ = PlayerState::PLAYING;
    return true;
}

void Player::pauseLocked() {
    if (state_ != PlayerState::PLAYING) return;
    backend_->pausePlayback();
    state_ = PlayerState::PAUSED;
}

void Player::resumeLocked() {
    if (state_ != PlayerState::PAUSED) return;
    backend_->start();
    state_ = PlayerState::PLAYING;
}

void Player::stopLocked() {
    if (backend_) {
        backend_->stopPlayback();
    }
    state_ = PlayerState::STOPPED;
}

int Player::computeNextIndexLocked() const {
    if (activePlaylist_ == nullptr || activePlaylist_->isEmpty()) return -1;
    size_t n = activePlaylist_->size();

    switch (mode_) {
        case PlaybackMode::NO_REPEAT: {
            int next = currentIndex_ + 1;
            if (static_cast<size_t>(next) >= n) return -1;
            return next;
        }
        case PlaybackMode::REPEAT_ONE: {
            return currentIndex_;
        }
        case PlaybackMode::REPEAT_ALL: {
            int next = (currentIndex_ + 1) % static_cast<int>(n);
            return next;
        }
        case PlaybackMode::SHUFFLE: {
            if (n == 1) return currentIndex_;
            int candidate;
            do {
                candidate = std::rand() % static_cast<int>(n);
            } while (candidate == currentIndex_);
            return candidate;
        }
    }
    return -1;
}

int Player::computePreviousIndexLocked() const {
    if (activePlaylist_ == nullptr || activePlaylist_->isEmpty()) return -1;
    size_t n = activePlaylist_->size();

    if (mode_ == PlaybackMode::SHUFFLE) {
        if (n == 1) return currentIndex_;
        int candidate;
        do {
            candidate = std::rand() % static_cast<int>(n);
        } while (candidate == currentIndex_);
        return candidate;
    }

    int prev = currentIndex_ - 1;
    if (prev < 0) {
        prev = static_cast<int>(n) - 1;
    }
    return prev;
}

bool Player::nextLocked() {
    if (activePlaylist_ == nullptr || activePlaylist_->isEmpty()) return false;

    int nextIdx = computeNextIndexLocked();
    if (nextIdx < 0) {
        stopLocked();
        return false;
    }

    currentIndex_ = nextIdx;
    if (!loadIndexIntoBackendLocked(static_cast<size_t>(currentIndex_))) {
        return nextLocked();
    }

    Song* song = activePlaylist_->getSong(static_cast<size_t>(currentIndex_));
    if (song != nullptr) {
        song->incrementPlayCount();
    }

    backend_->start();
    state_ = PlayerState::PLAYING;
    return true;
}

bool Player::previousLocked() {
    if (activePlaylist_ == nullptr || activePlaylist_->isEmpty()) return false;

    int prevIdx = computePreviousIndexLocked();
    if (prevIdx < 0) return false;

    currentIndex_ = prevIdx;
    if (!loadIndexIntoBackendLocked(static_cast<size_t>(currentIndex_))) {
        return previousLocked();
    }

    Song* song = activePlaylist_->getSong(static_cast<size_t>(currentIndex_));
    if (song != nullptr) {
        song->incrementPlayCount();
    }

    backend_->start();
    state_ = PlayerState::PLAYING;
    return true;
}
