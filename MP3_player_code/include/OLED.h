#ifndef OLED_H
#define OLED_H

#include <Arduino.h>
#include <U8g2lib.h>
#include "SPI_HAL.h"

#define ZERO_X 2
#define ZERO_Y 1

#define TEXT_X_ZERO (ZERO_X + 14)
#define TEXT_Y_ZERO (ZERO_Y + 10)

#define ART_X_ZERO (ZERO_X + 80)
#define ART_Y_ZERO (ZERO_Y + 10)

#define ICON_X_ZERO (ZERO_X + 2)
#define ICON_Y_ZERO (ZERO_Y + 3)

void set_sdState(bool state);
void OLED_init();
void OLED_task();

#endif // OLED_H