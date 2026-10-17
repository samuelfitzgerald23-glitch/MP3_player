#include <Arduino.h>
#include "sd_card.h"
#include "SPI_HAL.h"

#define BAURD_RATE 9600

bool sd_state;

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  Serial.begin(BAURD_RATE);
  delay(1000);
  Serial.println("MP3 Player starting...");
  SPI_init();
  if(!SD_init()) {
    digitalWrite(LED_BUILTIN, LOW);
  }

}

void loop() {
}