#include <Arduino.h>
#include "state.h"
#include "encoder.h"

#define BAURD_RATE 9600

context *cxt;

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  Serial.begin(BAURD_RATE);
  delay(1000);
  Serial.println("MP3 Player starting...");

  // Initialize SPI
  SPI_init();

  // Initialize the SD card
  if(!SD_init()) {
    digitalWrite(LED_BUILTIN, LOW);
  }

  // Initialize the OLED display
  OLED_init();

  // Initialize the encoder
  encoder_init();

  //init objects
  cxt = new context(new State_Home());
}

void loop() {
  int rot = getRotation();
  if(rot != 0) {
    Serial.print("Encoder rotated: ");
    Serial.println(rot);
    cxt->request_rot(rot);
  }

  if(getPushEvent()) {
    Serial.println("Encoder button pushed!");
    cxt->request_push();
  }

  OLED_task();
  delay(20);
}