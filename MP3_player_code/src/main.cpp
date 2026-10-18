#include <Arduino.h>
#include "sd_card.h"
#include "OLED.h"

#define BAURD_RATE 9600

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  Serial.begin(BAURD_RATE);
  delay(1000);
  Serial.println("MP3 Player starting...");
  SPI_init();
  if(!SD_init()) {
    digitalWrite(LED_BUILTIN, LOW);
    set_sdState(0);
  } else {
    set_sdState(1);
  }
  OLED_init();
}

void loop() {
  OLED_task();
}