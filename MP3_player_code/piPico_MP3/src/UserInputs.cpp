#include "UserInputs.h"

#define BUTTON_DEBOUNCE_MS 100

#define BUTTON_DOUBLE_PRESS_MS 750

static volatile bool EncoderPushEvent = 0;

static volatile int Rotation = 0;

static volatile bool ButtonOnePending = false;
static volatile bool ButtonTwoPending = false;

static volatile bool ButtonOnePushEvent = 0;
static volatile bool ButtonTwoPushEvent = 0;

static volatile uint32_t lastButtonOnePressTime = 0;
static volatile bool ButtonOneDoublePressEvent = 0;

static volatile uint32_t lastButtonTwoPressTime = 0;
static volatile bool ButtonTwoDoublePressEvent = 0;

static uint32_t lastPressTime[5] = {0, 0, 0, 0, 0}; // Store the last press time for each button

static bool debounceButton(uint8_t pin) {
    if (pin > 4 || pin < 0) return false; // Invalid pin index

    uint32_t currentTime = millis();
    if (currentTime - lastPressTime[pin] > BUTTON_DEBOUNCE_MS) {
        lastPressTime[pin] = currentTime;
        return true; // Button press is valid
    }
    return false; // Button press is ignored due to debounce
}

void inputs_init() {
    //encoder
    pinMode(ENCODER_SW_PIN, INPUT_PULLUP);
    pinMode(ENCODER_DT_PIN, INPUT_PULLUP);
    pinMode(ENCODER_CLK_PIN, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(ENCODER_SW_PIN), encoder_sw_isr, FALLING);
    attachInterrupt(digitalPinToInterrupt(ENCODER_CLK_PIN), encoder_clk_isr, CHANGE);

    //push-buttons
    pinMode(BUTTON_1_PIN, INPUT_PULLUP);
    pinMode(BUTTON_2_PIN, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(BUTTON_1_PIN), buttonOneISR, FALLING);
    attachInterrupt(digitalPinToInterrupt(BUTTON_2_PIN), buttonTwoISR, FALLING);
}

void encoder_sw_isr() {
    //Handle encoder button push
    //Serial.println("Encoder button pushed!");
    if (debounceButton(ENCODER_SW)) {
        EncoderPushEvent = 1;
    }
}

void encoder_clk_isr() {
    // Handle the encoder rotation event
    if (gpio_get(ENCODER_CLK_PIN) == gpio_get(ENCODER_DT_PIN)) {
        //positive rotation
        //Serial.println("Encoder rotated clockwise!");
        if (debounceButton(ENCODER_CLK)) {
            Rotation += 1;
        }
    }
    else {
        //negative rotation
        //Serial.println("Encoder rotated counter-clockwise!");
        if (debounceButton(ENCODER_DT)) {
            Rotation -= 1;
        }
    }
}

void buttonOneISR() {
    if (!debounceButton(BUTTON_1)) return;

    uint32_t now = millis();

    if (ButtonOnePending && (now - lastButtonOnePressTime) < BUTTON_DOUBLE_PRESS_MS) {
        ButtonOneDoublePressEvent = 1;
        ButtonOnePending = false;
    } else {
        lastButtonOnePressTime = now;
        ButtonOnePending = true;
    }
}

void buttonTwoISR() {
    if (!debounceButton(BUTTON_2)) return;

    uint32_t now = millis();

    if (ButtonTwoPending && (now - lastButtonTwoPressTime) < BUTTON_DOUBLE_PRESS_MS) {
        ButtonTwoDoublePressEvent = 1;
        ButtonTwoPending = false;
    } else {
        lastButtonTwoPressTime = now;
        ButtonTwoPending = true;
    }
}

static void checkSinglePress() {
    uint32_t now = millis();

    if (ButtonOnePending && (now - lastButtonOnePressTime) >= BUTTON_DOUBLE_PRESS_MS) {
        ButtonOnePushEvent = 1;
        ButtonOnePending = false;
    }
    if (ButtonTwoPending && (now - lastButtonTwoPressTime) >= BUTTON_DOUBLE_PRESS_MS) {
        ButtonTwoPushEvent = 1;
        ButtonTwoPending = false;
    }
}

bool getPushEvent(BUTTON button) {
    noInterrupts();

    bool temp;

    if (button == BUTTON_1 || button == BUTTON_2) checkSinglePress();

    switch(button) {
        case ENCODER_SW:
            temp = EncoderPushEvent;
            EncoderPushEvent = 0;
            break;
        case BUTTON_1:
            temp = ButtonOnePushEvent;
            ButtonOnePushEvent = 0;
            break;
        case BUTTON_2:
            temp = ButtonTwoPushEvent;
            ButtonTwoPushEvent = 0;
            break;
        case BUTTON_1_DOUBLE_PRESS:
            temp = ButtonOneDoublePressEvent;
            ButtonOneDoublePressEvent = 0;
            break;
        case BUTTON_2_DOUBLE_PRESS:
            temp = ButtonTwoDoublePressEvent;
            ButtonTwoDoublePressEvent = 0;
            break;
        default:
            temp = 0;
            break;
    }
    interrupts();
    return temp;
}

int getRotation() {
    noInterrupts();
    int temp = Rotation;
    Rotation = 0;
    interrupts();
    return constrain(temp, -1, 1);
}