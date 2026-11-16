#ifndef ENCODER_H
#define ENCODER_H

#include <Arduino.h>
#include <stdint.h>

//pins 34, 35, 36, and 39 do not have internal pull ups or downs
//encoder has pull up resistors, so we can use INPUT mode for the encoder pins

#define ENCODER_SW_PIN 35
#define ENCODER_DT_PIN 34
#define ENCODER_CLK_PIN 39

#define CLK_PIN GPIO_NUM_39
#define DT_PIN GPIO_NUM_34
#define SW_PIN GPIO_NUM_35

#define BUTTON_1_PIN 32
#define BUTTON_2_PIN 33

enum BUTTON : uint8_t {
    ENCODER_DT,
    ENCODER_CLK,
    ENCODER_SW,
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