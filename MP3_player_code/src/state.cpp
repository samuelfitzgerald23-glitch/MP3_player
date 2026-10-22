#include "state.h"

/*****************************************************************************************************************
 * home state methods
 */

void State_Home::handle_push() {
        switch(context_->model().current_select) {
            case 0: 
                context_->transition_to(new State_Settings());
                return;
            case 1: 
                context_->transition_to(new State_Playlist());
                return;
            default: return;
        }
    }

/******************************************************************************************************************
 * playlist state methods
 */

void State_Playlist::show_playlists() {
    level = 0;
    int n = loadList("/playlists", LIST_FOLDERS);
    context_->model().show_list(PLAYLIST_SCREEN, "Playlists", n, 1, getListName);
}

void State_Playlist::show_songs() {
    level = 1;
    char path[96];
    snprintf(path, sizeof(path), "/playlists/%s", playlist);
    int n = loadList(path, LIST_MP3);
    context_->model().show_list(PLAYLIST_SCREEN, "Playlists", n, 1, getListName);
}

void State_Playlist::handle_push() {

    if (context_->model().current_select == -1) {
        if (level == 1) { show_playlists(); return; }   // up one level
        clearCashe();                                   //clear cashe of file names
        context_->transition_to(new State_Home());      // leave the state
        return;
    }

    // absolute index into the list, not just the position on this page
    int idx = context_->model().page * ITEMS_PER_PAGE + context_->model().current_select;
    if (idx >= getListCount()) return;

    if (level == 0) {
        strncpy(playlist, getListName(idx), sizeof(playlist) - 1);
        playlist[sizeof(playlist) - 1] = '\0';   // copy BEFORE loading songs overwrites the cache
        show_songs();
    } else {
        //char path[160];
        //snprintf(path, sizeof(path), "/playlists/%s/%s", playlist, getListName(idx));
        context_->transition_to(new State_Playing(playlist, idx));
    }
}

/***************************************************************************************************************
 * playing state methods
 */

State_Playing::State_Playing(const char* playlist, int start_index) 
    : count_(getListCount()), idx_(start_index) {

    strncpy(playlist_, playlist, LIST_NAME_LEN - 1);
    playlist_[LIST_NAME_LEN - 1] = '\0';

    Serial.println(playlist_);
    Serial.println(getListName(idx_));
}

void State_Playing::showSong() {
    char path[160];
    snprintf(path, sizeof(path), "/playlists/%s/%s", playlist_, getListName(idx_));
    current_song = getMP3MetaData(path);
    formatTag(current_song, context_->model().songInfo, sizeof(context_->model().songInfo));
}

void State_Playing::handle_push() {
    int select = context_->model().current_select;
    //play song or go back
    switch(select) {
        case -1:
            clearCashe();
            context_->transition_to(new State_Playlist());
            return;
        default:
            showSong();
    }
}