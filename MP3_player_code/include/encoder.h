#ifndef ENCODER_H
#define ENCODER_H

#include <Arduino.h>
#include "OLED.h"

#define ENCODER_SW_PIN 18
#define ENCODER_DT_PIN 17
#define ENCODER_CLK_PIN 16

void encoder_init();
void encoder_sw_isr();
void encoder_clk_isr();

#endif // ENCODER_H