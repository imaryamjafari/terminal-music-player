#pragma once

#include "Screen.h"
#include <vector>
#include "Song.h"

class Application;

class FilterScreen : public Screen {
public:
    explicit FilterScreen(Application& app);

    void render() override;
    bool handleInput() override;
    bool hasResult() const;
    const std::vector<Song*>& getFilteredSongs() const;
    const std::string& getFilterDescription() const;

private:
    enum class SubState { CHOOSE_TYPE, CHOOSE_VALUE };

    Application& app_;
    SubState subState_;
    bool isArtist_; 
    bool resultReady_;
    bool exitToFullView_; 

    std::vector<std::string> distinctValues_;
    std::vector<int> countsPerValue_;

    std::vector<Song*> filteredSongs_;
    std::string filterDescription_;

    void buildDistinctValues();
};
