#ifndef OLED_H
#define OLED_H

#include <stdint.h>

#include <U8g2lib.h>
#include <Arduino.h>
#include <SPI.h>

#define SCL_PIN 2
#define SDA_PIN 3
#define RESET_PIN 4
#define DC_PIN 5
#define CS_PIN 6

#define PLAYBACK_X_ZERO 82
#define PLAYBACK_Y_ZERO 10

#define TEXT_X_ZERO 16
#define TEXT_Y_ZERO 11

#define ICON_X_ZERO 4
#define ICON_Y_ZERO 4

enum OLED_Select {
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
};


void OLED_init();
void OLED_MainScreen();
void OLEDTask();
void moveAttention(int n);

#endif // OLED_H