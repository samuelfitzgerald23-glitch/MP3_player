#include <Arduino.h>
#include "OLED.h"

#define LED_PIN 25

void setup() {
    Serial.begin(115200);

    while (!Serial) {
        delay(10); // Wait for Serial to be ready
    }

    OLED_init();

    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, HIGH); // Turn on the LED
}

void loop() {
    OLEDTask();
    delay(10);
}