#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"
#include "MiniaudioBackend.h"

struct MiniaudioBackend::Impl {
    ma_engine engine;
    ma_sound sound;
};

MiniaudioBackend::MiniaudioBackend()
    : impl_(new Impl()), engineOk_(false), soundLoaded_(false) {
    ma_result result = ma_engine_init(NULL, &impl_->engine);
    engineOk_ = (result == MA_SUCCESS);
}

MiniaudioBackend::~MiniaudioBackend() {
    unloadCurrent();
    if (engineOk_) {
        ma_engine_uninit(&impl_->engine);
    }
    delete impl_;
}

bool MiniaudioBackend::loadFile(const std::string& filePath) {
    unloadCurrent();

    if (!engineOk_) return false;

    ma_result result = ma_sound_init_from_file(
        &impl_->engine, filePath.c_str(), MA_SOUND_FLAG_DECODE, NULL, NULL, &impl_->sound);

    if (result != MA_SUCCESS) {
        soundLoaded_ = false;
        return false;
    }

    soundLoaded_ = true;
    return true;
}

void MiniaudioBackend::unloadCurrent() {
    if (soundLoaded_) {
        ma_sound_uninit(&impl_->sound);
        soundLoaded_ = false;
    }
}

void MiniaudioBackend::start() {
    if (!soundLoaded_) return;
    ma_sound_start(&impl_->sound);
}

void MiniaudioBackend::pausePlayback() {
    if (!soundLoaded_) return;
    ma_sound_stop(&impl_->sound);
}

void MiniaudioBackend::stopPlayback() {
    if (!soundLoaded_) return;
    ma_sound_stop(&impl_->sound);
    ma_sound_seek_to_pcm_frame(&impl_->sound, 0);
}

bool MiniaudioBackend::isAtEnd() const {
    if (!soundLoaded_) return false;
    return ma_sound_at_end(&impl_->sound) == MA_TRUE;
}

float MiniaudioBackend::getCursorSeconds() const {
    if (!soundLoaded_ || !engineOk_) return 0.0f;
    ma_uint64 frames = 0;
    ma_sound_get_cursor_in_pcm_frames(const_cast<ma_sound*>(&impl_->sound), &frames);
    ma_uint32 rate = ma_engine_get_sample_rate(const_cast<ma_engine*>(&impl_->engine));
    if (rate == 0) return 0.0f;
    return static_cast<float>(frames) / static_cast<float>(rate);
}

float MiniaudioBackend::getLengthSeconds() const {
    if (!soundLoaded_ || !engineOk_) return 0.0f;
    ma_uint64 frames = 0;
    ma_sound_get_length_in_pcm_frames(const_cast<ma_sound*>(&impl_->sound), &frames);
    ma_uint32 rate = ma_engine_get_sample_rate(const_cast<ma_engine*>(&impl_->engine));
    if (rate == 0) return 0.0f;
    return static_cast<float>(frames) / static_cast<float>(rate);
}

bool MiniaudioBackend::isLoaded() const { return soundLoaded_; }

void MiniaudioBackend::setKnownLengthHintSeconds(float) {
}
