#include <Arduino.h>
#include "state.h"
#include "OLED.h"
#include "UserInputs.h"

#define BAURD_RATE 9600

#define DISPLAY_UPDATE_MS 20

UI_model model;
OLED_Screen *display;
context *cxt;

player *audio;
volatile bool player_ready = 0;

void setup() {
  mutex_init(&busMutex);

  //pinMode(A2, INPUT);

  Serial.begin(BAURD_RATE);
  delay(1000);
  Serial.println("MP3 Player starting...");

  // Initialize SPI
  SPI_init();

  // Initialize the OLED display
  display = new OLED_Screen(model);

  // Initialize the SD card
  SD_init();

  // Initialize the user inputs
  inputs_init();

  //init player
  audio = new player();

  //init objects
  cxt = new context(model, new State_Home(), audio);
  player_ready = 1;
}

static uint32_t t = 0;
static uint32_t lastDisplayUpdate = 0;

void loop() {
  if (audio->getFinished()) cxt->request_skip();

  // if (millis() - t > 1) {
  //     t = millis();
  //     Serial.printf("on: %d\n", analogRead(A2));
  // }

  int rot = getRotation();
  if(rot != 0) cxt->request_rot(rot);

  if(getPushEvent(ENCODER_SW)) cxt->request_push();

  if(getPushEvent(BUTTON_1)) cxt->request_skip();
  if(getPushEvent(BUTTON_2)) cxt->request_pause_play();
  if(getPushEvent(BUTTON_1_DOUBLE_PRESS)) cxt->request_back();
  if(getPushEvent(BUTTON_2_DOUBLE_PRESS)) cxt->request_stop();

  if(millis() - lastDisplayUpdate > DISPLAY_UPDATE_MS) {
    display->display();
    lastDisplayUpdate = millis();
  }
}

void setup1() {
  while (!player_ready);
}

void loop1() {
  audio->task();
  delay(1);
}