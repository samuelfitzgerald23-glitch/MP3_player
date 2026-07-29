#include <Arduino.h>
#include "OLED.h"
#include "encoder.h"
#include "SDcard.h"

void setup() {
    Serial.begin(9600);

    while (!Serial) {delay(10);}

    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, 1);

    Serial.println("Enableing SPI");
    SPI_init();

    stateInit();

    //Serial.println("Enableing SD card");
    //SD_init();

    Serial.println("Enableing OLED screen");
    OLED_init();

    disable_interrupts();
    Serial.println("Enableing encoder");
    encoder_init();
    enable_interrupts();
}

void loop() {
    encoderTask();
    OLEDTask();
    delay(50);
}