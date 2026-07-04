#include "encoder.h"

void encoder_init() {
    pinMode(ENCODER_SW_PIN, INPUT);
    pinMode(ENCODER_DT_PIN, INPUT_PULLUP);
    pinMode(ENCODER_CLK_PIN, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(ENCODER_CLK_PIN), encoder_clk_isr, FALLING);
}

void encoder_clk_isr() {
    // Handle the encoder rotation event
    if (digitalRead(ENCODER_CLK_PIN) == digitalRead(ENCODER_DT_PIN)) {
        moveAttention(1);
    }
    else {
        moveAttention(-1);
    }
}