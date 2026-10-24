#ifndef UI_MODEL_H
#define UI_MODEL_H

#include "Arduino.h"

#define ITEMS_PER_PAGE 4

enum OLED_STATE {
    HOME_SCREEN,
    SETTINGS_SCREEN,
    PLAYLIST_SCREEN,
    PLAYING_SCREEN
};

struct UI_model {
    OLED_STATE screen = HOME_SCREEN;
    const char* title = "";
    char songInfo[132] = {0};

    bool back_button = 0;
    bool paused = 0;

    const char* (*item_name)(int) = nullptr;   // set by the state; renderer calls it

    int page = 0;
    int n_selects = 2;
    int running_selects = 2;
    int current_select = 0;
    int page_count = 1;

    void show_list(OLED_STATE s, const char* t, int count, bool back, const char* (*names)(int)) {
        screen = s; 
        title = t; 
        back_button = back;
        n_selects = count; 
        item_name = names;
        page = 0; 
        current_select = 0;
        running_selects = n_selects;
        page_count = max(1, (n_selects + ITEMS_PER_PAGE - 1) / ITEMS_PER_PAGE);
    }

    void scrollPage(int dir) {
        page += dir;
        running_selects -= dir * ITEMS_PER_PAGE;
        if(page >= page_count) {
            page = 0;
            running_selects = n_selects;
        } else if(page < 0) {
            page = page_count - 1;
            running_selects = n_selects - (page * ITEMS_PER_PAGE);
        }
    }

    void scrollScreen(int dir) {
        //scroll the screen in direction i
        int current = current_select + dir;
        if(current < -1 || (current < 0 && !back_button)) {
            scrollPage(-1);
            current = min(running_selects, ITEMS_PER_PAGE) - 1;
        } else if(current >= ITEMS_PER_PAGE || current >= running_selects) {
            bool last_page = running_selects <= ITEMS_PER_PAGE;  // check before paging
            current = (last_page && back_button) ? -1 : 0;
            scrollPage(1);
        }
        current_select = current;
    }
};

#endif //UI_MODEL_H