#ifndef ENCODER_H
#define ENCODER_H

#include <Arduino.h>
#include <stdint.h>

#include "OLED.h"
#include "state.h"

#define ENCODER_SW_PIN 11
#define ENCODER_DT_PIN 13
#define ENCODER_CLK_PIN 12

void encoder_sw_isr();
void encoder_clk_isr();

void encoderPush(state_t* currentState);
void encoderRotate(state_t* currentState);

void encoder_init();
void encoderTask();

#endif // ENCODER_H