#ifndef ENCODER_H
#define ENCODER_H

#include <Arduino.h>
#include <stdint.h>

#define ENCODER_SW_PIN 20
#define ENCODER_DT_PIN 19
#define ENCODER_CLK_PIN 18

#define BUTTON_1_PIN 16
#define BUTTON_2_PIN 17

enum BUTTON : uint8_t {
    ENCODER_SW,
    ENCODER_DT,
    ENCODER_CLK,
    BUTTON_1,
    BUTTON_2,
    BUTTON_1_DOUBLE_PRESS,
    BUTTON_2_DOUBLE_PRESS
};

void inputs_init();

void encoder_sw_isr();
void encoder_clk_isr();
void buttonOneISR();
void buttonTwoISR();

bool getPushEvent(BUTTON button);

int getRotation();

#endif // ENCODER_H