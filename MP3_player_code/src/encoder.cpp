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
    if (digitalRead(ENCODER_CLK_PIN) == digitalRead(ENCODER_DT_PIN)) {
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
    screen_t* screen = getScreen();
    int currentSelect = static_cast<int>(screen->currentSelect);

    //regardless of state a select of -1 (BACK) should change the current state to the previous one
    if (currentSelect == -1) {
        changeState(currentSelect);
        resetScreen(getState());
        return;
    }

    switch (currentState->push) {
        case SELECT:
            //if push type is SELECT then this corrosponds with a push action changing the state
            changeState(currentSelect);
            resetScreen(getState());
            break;
        case ADJUST:
            //if push type is ADJUST then the push will change a configuration mode
            if (currentState->name == PLAYBACK) {
                //change the playback mode (in order or shuffle)
            }
            break;
        case PLAY:
            //if push type is PLAY then push will cause an action to be done
            if (currentState->name == SONGS) {
                //play selected song
                int song_index = currentSelect + screen->page;
            }
            break;
        default:
            break;
    }
}

void encoderRotate(state_t* currentState) {
    int value = 0;
    if (positiveRotation) {
        value = 1;
        positiveRotation = 0;
    } else if (negetiverotation) {
        value = -1;
        negetiverotation = 0;
    }

    switch (currentState->rotate) {
        case SELECT:{
            //if rotate type is SELECT then the current select type will change, if state has more selects then 5 screen will scroll
            screen_t* screen = getScreen();
            int nextSelect = static_cast<int>(screen->currentSelect) + value;
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
            break;
            }
        case ADJUST:
            //if rotate type is ADJUST then rotation will change a setting variable (e.g treble)
            if (currentState->name == BRIGHTNESS) {
                //change the brightness of the screen
                changeBrightness(value);
            }
            break;
        case PLAY:
            //I dont think this is really needed, but just in case
            break;
        default:
            break;
    }
}

void encoderTask() {
    state_t* currentState = getState();

    bool push = pushEvent;
    bool rotate = positiveRotation||negetiverotation;

    if(push) encoderPush(currentState);
    if(rotate) encoderRotate(currentState);
}