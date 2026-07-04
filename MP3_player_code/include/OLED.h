#ifndef OLED_H
#define OLED_H

#include <stdint.h>

#include <U8g2lib.h>
#include <Arduino.h>
#include <SPI.h>

void OLED_init();
void OLED_Screen1();
void OLEDTask();

#endif // OLED_H