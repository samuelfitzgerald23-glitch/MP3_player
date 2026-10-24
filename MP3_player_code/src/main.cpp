#include <Arduino.h>
#include "state.h"
#include "OLED.h"
#include "encoder.h"

#define BAURD_RATE 9600

UI_model model;
OLED_Screen *display;
context *cxt;

player *audio;
bool player_reader = 0;

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
  display = new OLED_Screen(model);

  // Initialize the encoder
  encoder_init();

  //init objects
  cxt = new context(model, new State_Home());

  audio = new player();
  player_reader = 1;
}

void loop() {
  int rot = getRotation();
  if(rot != 0) {
    // Serial.print("Encoder rotated: ");
    // Serial.println(rot);
    cxt->request_rot(rot);
  }

  if(getPushEvent()) {
    //Serial.println("Encoder button pushed!");
    cxt->request_push();
  }

  display->display();
  delay(20);
}

void setup1() {
  while (!player_reader);
}

void loop1() {
  
}