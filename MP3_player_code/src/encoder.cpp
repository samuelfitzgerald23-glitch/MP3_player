#include "encoder.h"

static volatile int pushEvent = 0;

static volatile int Rotation = 0;


void encoder_init() {
    pinMode(ENCODER_SW_PIN, INPUT_PULLUP);
    pinMode(ENCODER_DT_PIN, INPUT_PULLUP);
    pinMode(ENCODER_CLK_PIN, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(ENCODER_SW_PIN), encoder_sw_isr, FALLING);
    attachInterrupt(digitalPinToInterrupt(ENCODER_CLK_PIN), encoder_clk_isr, CHANGE);
}

void encoder_sw_isr() {
    //Handle encoder button push
    Serial.println("Encoder button pushed!");
    pushEvent += 1;
}

void encoder_clk_isr() {
    // Handle the encoder rotation event
    if (gpio_get(ENCODER_CLK_PIN) == gpio_get(ENCODER_DT_PIN)) {
        //positive rotation
        Serial.println("Encoder rotated clockwise!");
        Rotation += 1;
    }
    else {
        //negative rotation
        Serial.println("Encoder rotated counter-clockwise!");
        Rotation -= 1;
    }
}

int getPushEvent() {
    noInterrupts();
    bool temp = pushEvent;
    pushEvent = 0;
    interrupts();
    return temp;
}

int getRotation() {
    noInterrupts();
    int temp = Rotation;
    Rotation = 0;
    interrupts();
    return temp;
}