#include "encoder.h"

void encoder_init() {
    pinMode(ENCODER_SW_PIN, INPUT_PULLUP);
    pinMode(ENCODER_DT_PIN, INPUT_PULLUP);
    pinMode(ENCODER_CLK_PIN, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(ENCODER_SW_PIN), encoder_sw_isr, FALLING);
    attachInterrupt(digitalPinToInterrupt(ENCODER_CLK_PIN), encoder_clk_isr, FALLING);
}

void encoder_sw_isr() {
    //Handle encoder button push
    changeScreen();
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

    int n = getRuntimeSelects();

    if (value < 0) {
        value = -1;
        if (currentScreen->id == 0) value = n - 1;
    }
    else if (value >= n) {
        value = 0;
        if (getPageN() == (getPageCount() - 1)) setEnableScroll(1);
    }
    else if (value >= ITEMS_PER_PAGE) {
        value =  0;
        setEnableScroll(1);
    }

    currentScreen->currentSelect = static_cast<OLED_Select>(value);
}