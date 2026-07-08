#ifndef ENCODER_H
#define ENCODER_H

#include <Arduino.h>
#include "OLED.h"
#include <stdint.h>

#define ENCODER_SW_PIN 22
#define ENCODER_DT_PIN 21
#define ENCODER_CLK_PIN 20

void encoder_init();
void encoder_sw_isr();
void encoder_clk_isr();

#endif // ENCODER_H