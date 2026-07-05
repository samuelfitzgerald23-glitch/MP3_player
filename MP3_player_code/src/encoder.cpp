#include "encoder.h"

void encoder_init() {
    pinMode(ENCODER_SW_PIN, INPUT);
    pinMode(ENCODER_DT_PIN, INPUT_PULLUP);
    pinMode(ENCODER_CLK_PIN, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(ENCODER_CLK_PIN), encoder_clk_isr, FALLING);
}

void encoder_clk_isr() {
    // Handle the encoder rotation event
    OLED_Screen_t* currentScreen = getCurrentScreen();

    int value = static_cast<int>(currentScreen->currentSelect);

    if (digitalRead(ENCODER_CLK_PIN) == digitalRead(ENCODER_DT_PIN)) {
        //positive rotation
        value += 1;
    }
    else {
        //negative rotation
        value -= 1;
    }

    if (value < 0) {
        value = currentScreen->N_selects - 1;
    }
    else if (value >= currentScreen->N_selects) {
        value = 0;
    }

    currentScreen->currentSelect = static_cast<OLED_Select>(value);
}