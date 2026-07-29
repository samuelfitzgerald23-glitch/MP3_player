#ifndef OLED_H
#define OLED_H

#include <stdint.h>

#include <U8g2lib.h>
#include <Arduino.h>
#include "SPI_HAL.h"

#include "state.h"

#define ITEMS_PER_PAGE 5

#define ART_X_ZERO 82
#define ART_Y_ZERO 10

#define TEXT_X_ZERO 16
#define TEXT_Y_ZERO 11

#define ICON_X_ZERO 4
#define ICON_Y_ZERO 4

enum OLED_Select {
    BACK = -1,
    SELECT0,
    SELECT1,
    SELECT2,
    SELECT3,
    SELECT4
};

struct screen_t {
    int page;
    int pageCount;
    OLED_Select currentSelect;
    int N_selects;
    int runtimeSelects;
};

//getters
screen_t* getScreen();
int getRuntimeSelects();

//screen manipulation
void resetScreen(state_t* state);
void setEnableScroll(bool a);
void scrollScreen();

//drawing funcs
void DrawStateOptions(state_t* state);
void drawScreen(state_t* state);

void OLED_init();
void OLEDTask();

#endif // OLED_H