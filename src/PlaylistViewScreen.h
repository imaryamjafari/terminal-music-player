#pragma once

#include "Screen.h"
#include <vector>
#include <string>
#include "Song.h"

class Application;

class PlaylistViewScreen : public Screen {
public:
    explicit PlaylistViewScreen(Application& app);

    void render() override;
    bool handleInput() override;
    bool wantsNowPlaying() const;

private:
    enum class SortField { NONE, TITLE, ARTIST, ALBUM, YEAR, DURATION };

    Application& app_;

    SortField sortField_;
    bool sortDescending_;

    std::string searchQuery_; 
    bool filterActive_;
    std::vector<Song*> filterResultSongs_;
    std::string filterDescription_;

    bool wantsNowPlaying_ = false;
    std::vector<Song*> computeDisplayList() const;

    int mapDisplayIndexToPlaylistIndex(Song* song) const;

    void renderSortSubmenu();
    void handleSortSubmenu();
    void handleSearchPrompt();

    enum class FavouriteToggleResult {
        NotMatched,    
        Success,       
        IndexOutOfRange  
    };
    FavouriteToggleResult tryHandleFavouriteToggle(const std::string& raw,
                                                   const std::vector<Song*>& displayList);
};
