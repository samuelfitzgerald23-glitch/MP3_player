#ifndef OLED_H
#define OLED_H

#include <Arduino.h>
#include <U8g2lib.h>
#include "SPI_HAL.h"
#include "sd_card.h"

#define ZERO_X 0
#define ZERO_Y 1

#define TEXT_X_ZERO (ZERO_X + 14)
#define TEXT_Y_ZERO (ZERO_Y + 10)

#define ART_X_ZERO (ZERO_X + 80)
#define ART_Y_ZERO (ZERO_Y + 10)

#define ICON_X_ZERO (ZERO_X + 2)
#define ICON_Y_ZERO (ZERO_Y + 3)

#define BACK_X_ZERO (ZERO_X + 115)
#define BACK_Y_ZERO (ZERO_Y + 53)

#define ITEMS_PER_PAGE 5

enum OLED_Select {
    BACK = -1,
    SELECT0,
    SELECT1,
    SELECT2,
    SELECT3,
    SELECT4
};

enum OLED_STATE {
    HOME_SCREEN,
    SETTINGS_SCREEN,
    PLAYLIST_SCREEN,
    PLAYING_SCREEN
};

class OLED_Screen {
    private:
        int page = 0;
        int page_count;
        int num_selects;

    public:
        OLED_STATE oled_state;
        bool back_button;
        int running_selects;
        OLED_Select current_select = SELECT0;

        void set_state(OLED_STATE state, int count = 0) {
            oled_state = state;
            switch(state) {
                case HOME_SCREEN:
                    num_selects = 2;
                    break;
                case SETTINGS_SCREEN:
                    num_selects = 7;
                    break;
                default:
                    num_selects = count;
                    break;
            }
            reset_screen(num_selects);
        }

        void scroll_page(int dir) {
            page += dir;
            running_selects -= dir * ITEMS_PER_PAGE;
            if(page >= page_count) {
                page = 0;
                running_selects = num_selects;
            } else if(page < 0) {
                page = page_count - 1;
                running_selects = num_selects - (page * ITEMS_PER_PAGE);
            }
        }

        int getPage() const {return page;}

        void reset_screen(int n) {
            current_select = SELECT0;
            running_selects = n;
            page = 0;
            page_count = (n + ITEMS_PER_PAGE - 1)/ITEMS_PER_PAGE;
        }

        void draw_screen(OLED_STATE state);

        OLED_Screen(OLED_STATE state = HOME_SCREEN, int num = 2, bool back = false) : 
        oled_state(state), 
        num_selects(num), 
        back_button(back) {
            page_count = (num_selects + ITEMS_PER_PAGE - 1)/ITEMS_PER_PAGE;
        }

        ~OLED_Screen() {}
    
};

OLED_Screen* getOLEDScreen();

void setOLEDState(OLED_STATE state, int count = 0);
void toggle_back(bool a);

void OLED_init();
void OLED_task();

#endif // OLED_H