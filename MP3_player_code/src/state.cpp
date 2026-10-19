#include "state.h"

/******************************************************************************************************************
 * general methods
 */

void scroll_screen(int dir, OLED_Screen* oled_screen) {
    // scroll the screen in the given direction
    int current = static_cast<int>(oled_screen->current_select) + dir;
    if(current < -1 || (current < 0 && !oled_screen->back_button)) {
        oled_screen->scroll_page(-1);
        current = oled_screen->running_selects - 1;
    } else if(current >= ITEMS_PER_PAGE || current >= oled_screen->running_selects) {
        bool last_page = oled_screen->running_selects <= ITEMS_PER_PAGE;  // check before paging
        current = (last_page && oled_screen->current_select) ? -1 : 0;
        oled_screen->scroll_page(1);
    }
    oled_screen->current_select = static_cast<OLED_Select>(current);
}

/*****************************************************************************************************************
 * home state methods
 */

void State_Home::handle_push() {
        switch(getOLEDScreen()->current_select) {
            case SELECT0: 
                context_->transition_to(new State_Settings());
                return;
            case SELECT1: 
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
    setOLEDState(PLAYLIST_SCREEN, n);
}

void State_Playlist::show_songs() {
    level = 1;
    char path[96];
    snprintf(path, sizeof(path), "/playlists/%s", playlist);
    int n = loadList(path, LIST_MP3);
    setOLEDState(PLAYLIST_SCREEN, n);
}

void State_Playlist::handle_push() {
    OLED_Screen* oled = getOLEDScreen();

    if (oled->current_select == BACK) {
        if (level == 1) { show_playlists(); return; }   // up one level
        context_->transition_to(new State_Home());      // leave the state
        return;
    }

    // absolute index into the list, not just the position on this page
    int idx = oled->getPage() * ITEMS_PER_PAGE + static_cast<int>(oled->current_select);
    if (idx >= getListCount()) return;

    if (level == 0) {
        strncpy(playlist, getListName(idx), sizeof(playlist) - 1);
        playlist[sizeof(playlist) - 1] = '\0';   // copy BEFORE loading songs overwrites the cache
        show_songs();
    } else {
        char path[160];
        snprintf(path, sizeof(path), "/playlists/%s/%s", playlist, getListName(idx));
        // hand `path` to the player here
    }
}