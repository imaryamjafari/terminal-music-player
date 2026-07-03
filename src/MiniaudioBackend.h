#pragma once

#include "IAudioBackend.h"
#include <string>

class MiniaudioBackend : public IAudioBackend {
public:
    MiniaudioBackend();
    ~MiniaudioBackend() override;

    MiniaudioBackend(const MiniaudioBackend&) = delete;
    MiniaudioBackend& operator=(const MiniaudioBackend&) = delete;

    bool loadFile(const std::string& filePath) override;
    void unloadCurrent() override;
    void start() override;
    void pausePlayback() override;
    void stopPlayback() override;
    bool isAtEnd() const override;
    float getCursorSeconds() const override;
    float getLengthSeconds() const override;
    void setKnownLengthHintSeconds(float seconds) override;
    bool isLoaded() const override;

private:
    struct Impl;
    Impl* impl_;

    bool engineOk_;
    bool soundLoaded_;
};
