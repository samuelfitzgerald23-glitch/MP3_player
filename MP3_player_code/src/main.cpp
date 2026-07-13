#include <Arduino.h>
#include "OLED.h"
#include "encoder.h"

void setup() {
    disable_interrupts();
    Serial.begin(115200);

    stateInit();

    OLED_init();
    encoder_init();
    
    enable_interrupts();
}

void loop() {
    encoderTask();
    OLEDTask();
    delay(50);
}