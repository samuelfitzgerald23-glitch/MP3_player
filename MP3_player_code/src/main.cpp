#include <Arduino.h>
#include "OLED.h"
#include "encoder.h"

void setup() {
    disable_interrupts();
    Serial.begin(115200);

    OLED_init();
    encoder_init();
    
    enable_interrupts();
}

void loop() {
    OLEDTask();
    delay(10);
}