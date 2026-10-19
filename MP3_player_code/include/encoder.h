#ifndef ENCODER_H
#define ENCODER_H

#include <Arduino.h>
#include <stdint.h>

#define ENCODER_SW_PIN 20
#define ENCODER_DT_PIN 19
#define ENCODER_CLK_PIN 18

void encoder_init();

void encoder_sw_isr();
void encoder_clk_isr();

int getPushEvent();

int getRotation();

#endif // ENCODER_H