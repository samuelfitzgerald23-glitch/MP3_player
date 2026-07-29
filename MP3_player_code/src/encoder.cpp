#include "encoder.h"

static volatile bool pushEvent;

static volatile bool positiveRotation;

static volatile bool negetiverotation;

void encoder_init() {
    pinMode(ENCODER_SW_PIN, INPUT_PULLUP);
    pinMode(ENCODER_DT_PIN, INPUT_PULLUP);
    pinMode(ENCODER_CLK_PIN, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(ENCODER_SW_PIN), encoder_sw_isr, FALLING);
    attachInterrupt(digitalPinToInterrupt(ENCODER_CLK_PIN), encoder_clk_isr, CHANGE);
}

void encoder_sw_isr() {
    //Handle encoder button push
    pushEvent = 1;
}

void encoder_clk_isr() {
    // Handle the encoder rotation event
    if (gpio_get(ENCODER_CLK_PIN) == gpio_get(ENCODER_DT_PIN)) {
        //positive rotation
        positiveRotation = 1;
    }
    else {
        //negative rotation
        negetiverotation = 1;
    }
}

void encoderPush(state_t* currentState) {
    pushEvent = 0;
    int currentSelect = static_cast<int>(getScreen()->currentSelect);

    //regardless of state a select of -1 (BACK) should change the current state to the previous one
    if (getScreen()->currentSelect == -1) {
        changeState(-1);
        resetScreen(getState());
        return;
    }

    currentState->push();
}

void encoderRotate(state_t* currentState) {
    int dir = 0;
    if (positiveRotation) {
        dir = 1;
        positiveRotation = 0;
    } else if (negetiverotation) {
        dir = -1;
        negetiverotation = 0;
    }

    currentState->rotate(dir);
}

void encoderTask() {
    state_t* currentState = getState();

    bool push = pushEvent;
    bool rotate = positiveRotation||negetiverotation;

    if(push) encoderPush(currentState);
    if(rotate) encoderRotate(currentState);
}

void changeSelect(int dir) {
    //if rotate type is SELECT then the current select type will change, if state has more selects then 5 screen will scroll
    state_t* currentState = getState();
    screen_t* screen = getScreen();
    int nextSelect = static_cast<int>(screen->currentSelect) + dir;
    int n = getRuntimeSelects();

    if (nextSelect <= -1) {
        nextSelect = -1;
        if (currentState->name == START) nextSelect = n - 1;
    }
    else if (nextSelect >= n) {
        nextSelect = 0;
        if (screen->page == (screen->pageCount - 1)) setEnableScroll(1);
    }
    else if (nextSelect >= ITEMS_PER_PAGE) {
        nextSelect =  0;
        setEnableScroll(1);
    }
    screen->currentSelect = static_cast<OLED_Select>(nextSelect);
}

void contextChange() {
    int currentSelect = static_cast<int>(getScreen()->currentSelect);
    changeState(currentSelect);
    resetScreen(getState());
}

void indexSong() {
    screen_t* screen = getScreen();
    int song_index = screen->currentSelect + screen->page;
    Serial.print(song_index);
}