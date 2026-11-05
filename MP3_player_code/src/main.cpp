#include <Arduino.h>
#include "state.h"
#include "OLED.h"
#include "encoder.h"

#define BAURD_RATE 9600

UI_model model;
OLED_Screen *display;
context *cxt;

player *audio;
volatile bool player_ready = 0;

void setup() {
  mutex_init(&busMutex);

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

  //init player
  audio = new player();

  //init objects
  cxt = new context(model, new State_Home(), audio);
  player_ready = 1;
}

void loop() {
  if (audio->getFinished()) cxt->request_skip();

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
  while (!player_ready);
}

void loop1() {
  audio->task();
  delay(1);
}