#pragma once

#include <string>
#include <cstdint>

class IAudioBackend {
public:
    virtual ~IAudioBackend() = default;

    virtual bool loadFile(const std::string& filePath) = 0;

    virtual void unloadCurrent() = 0;

    virtual void start() = 0;
    virtual void pausePlayback() = 0;
    virtual void stopPlayback() = 0;

    virtual bool isAtEnd() const = 0;

    virtual float getCursorSeconds() const = 0;
    virtual float getLengthSeconds() const = 0;

    virtual void setKnownLengthHintSeconds(float seconds) = 0;

    virtual bool isLoaded() const = 0;
};
