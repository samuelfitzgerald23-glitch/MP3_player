#include "state.h"

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

void scroll_screen(int dir, OLED_Screen* oled_screen) {
    // scroll the screen in the given direction
    int current = static_cast<int>(oled_screen->current_select) + dir;
    if(current < -1 || (current < 0 && oled_screen->oled_state == HOME_SCREEN)) {
        current = oled_screen->running_selects - 1;
        oled_screen->scroll_page(-1);
    } else if(current >= oled_screen->running_selects || current >= ITEMS_PER_PAGE) {
        current = 0;
        oled_screen->scroll_page(1);
    }
    oled_screen->current_select = static_cast<OLED_Select>(current);
}

void scroll_files(int dir) {
    // scroll through playlists
}