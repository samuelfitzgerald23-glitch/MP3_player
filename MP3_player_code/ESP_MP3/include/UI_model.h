#ifndef UI_MODEL_H
#define UI_MODEL_H

#include "Arduino.h"
#include "animation.h"

#define ITEMS_PER_PAGE 4

#define MAX_VOL 254

#define MAX_ANIMS 6

/********************************************************************************************************
 * screen placement defs
 */

#define ZERO_X 0
#define ZERO_Y 1

#define ICON_X_ZERO (ZERO_X + 2)
#define ICON_Y_ZERO (ZERO_Y + 3)

#define TEXT_X_ZERO (ZERO_X + 14)
#define TEXT_Y_ZERO (ZERO_Y + 19)
#define CURSOUR_Y_ZERO (ICON_Y_ZERO + 10)

#define TEXT_LINE_HEIGHT 10
#define TEXT_LINE_2_Y (TEXT_Y_ZERO + TEXT_LINE_HEIGHT)
#define TEXT_LINE_3_Y (TEXT_Y_ZERO + 2*TEXT_LINE_HEIGHT)

#define INFO_TEXT_X_ZERO (TEXT_X_ZERO + 70)

#define ART_X_ZERO (ZERO_X + 80)
#define ART_Y_ZERO (ZERO_Y + 10)

#define BACK_X_ZERO (ZERO_X + 115)
#define BACK_Y_ZERO (ZERO_Y + 53)

/************************************************************************************************************** */

enum OLED_STATE {
    HOME_SCREEN,
    SETTINGS_SCREEN,
    PLAYLIST_SCREEN,
    PLAYING_SCREEN
};

inline const char* noName(int) { return ""; }

struct UI_model {
    OLED_STATE screen = HOME_SCREEN;
    const char* title = "";
    char songInfo[132] = {0};
    AnimatedObject songTitle = AnimatedObject::Text((ICON_X_ZERO + 1), (TEXT_Y_ZERO + 40), 27);

    bool back_button = 0;
    bool paused = 0;
    bool repeat = 1;
    int volume = 20;

    const char* (*item_name)(int) = noName;

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
        item_name = names ? names : noName;
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

    int getIdx() {
        return page * ITEMS_PER_PAGE + current_select;
    }

    void setPowerMode(bool b);

    AnimatedObject* anims[MAX_ANIMS] = {};
    uint8_t nAnims = 0;

    void addAnimation(AnimatedObject* a) {
        if (nAnims < MAX_ANIMS) { a->reset(millis()); anims[nAnims++] = a; }
    }
    
    void clearAnimations() { nAnims = 0; }

    void tick(uint32_t now) {
        songTitle.tick(now);
        for (uint8_t i = 0; i < nAnims; i++) anims[i]->tick(now);
    }
};

#endif //UI_MODEL_H