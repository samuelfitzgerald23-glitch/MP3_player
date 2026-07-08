#ifndef OLED_H
#define OLED_H

#include <stdint.h>

#include <U8g2lib.h>
#include <Arduino.h>
#include <SPI.h>
#include <algorithm>

#define SCL_PIN 2
#define SDA_PIN 3
#define RESET_PIN 4
#define DC_PIN 5
#define CS_PIN 6

#define MAX_CHILDREN 10
#define ITEMS_PER_PAGE 5

#define PLAYBACK_X_ZERO 82
#define PLAYBACK_Y_ZERO 10

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

struct OLED_Screen_t {
    uint8_t id;
    uint8_t N_selects;
    void (*drawScreen)();
    OLED_Select currentSelect;
    uint8_t childrenIDs[MAX_CHILDREN];
    uint8_t parentID;
    const char* name;
};

//getters
OLED_Screen_t* getCurrentScreen();
int getRuntimeSelects();
int getPageN();
int getPageCount();
void setEnableScroll(bool a);

// screen manipulation
void changeScreen();
void scrollScreen();

// screen specific functions
void OLED_MainScreen();
void OLED_SettingsScreen();
void OLED_TestScreen();

//general screen functions
void TotalScreen();
void DrawOptions();

// functions used by main
void OLED_init();
void OLEDTask();

#endif // OLED_H