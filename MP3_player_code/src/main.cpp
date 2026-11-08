#include <Arduino.h>
#include "state.h"
#include "OLED.h"
#include "UserInputs.h"

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

  // Initialize the user inputs
  inputs_init();

  //init player
  audio = new player();

  //init objects
  cxt = new context(model, new State_Home(), audio);
  player_ready = 1;
}

static uint32_t t = 0;

void loop() {
  if (audio->getFinished()) cxt->request_skip();

  if (millis() - t > 100) {
      t = millis();
      Serial.printf("Paused: %d\n", cxt->model().paused);
      Serial.printf("PullUp16: %d, PullUp17: %d\n", gpio_is_pulled_up(16), gpio_is_pulled_up(17));
      Serial.printf("fn16=%d fn17=%d\n", gpio_get_function(16), gpio_get_function(17));
      Serial.printf("b1=%d b2=%d sw=%d\n", digitalRead(BUTTON_1_PIN),
                    digitalRead(BUTTON_2_PIN), digitalRead(ENCODER_SW_PIN));
  }

  int rot = getRotation();
  if(rot != 0) {
    // Serial.print("Encoder rotated: ");
    // Serial.println(rot);
    cxt->request_rot(rot);
  }

  if(getPushEvent(ENCODER_SW)) {
    //Serial.println("Encoder button pushed!");
    cxt->request_push();
  }

  if(getPushEvent(BUTTON_1)) cxt->request_skip();
  if(getPushEvent(BUTTON_2)) cxt->request_pause_play();

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