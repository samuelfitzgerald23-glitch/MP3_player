#include "state.h"


/*Forward declearations of states */
static state_t startState;

static state_t settingsState;
static state_t playlistState;

static state_t equaliserState;
static state_t playbackState;
static state_t brightnessState;

static state_t songsState;

state_t* currentState = &startState;

state_t* getState() {
    return currentState;
}

void changeState(int stateID) {
    if (stateID == -1) {
        currentState = currentState->parentState;
    } else {
        stateID = std::clamp(stateID, 0, MAX_CHILDREN);
        currentState = currentState->childrenStates[stateID];
    }
}

void stateInit() {
    /*Actual deffinitions of states*/

    //start
    startState = {0, 2, SELECT, SELECT, &startState, {&settingsState, &playlistState}, START, "Start"};

    //start states children
    settingsState = {0, 3, SELECT, SELECT, &startState, {&equaliserState, &playbackState, &brightnessState}, SETTINGS, "Settings"};
    playlistState = {1, 1, SELECT, SELECT, &startState, {&songsState}, PLAYLISTS, "Playlists"};    //pick playlist

    //settings state children
    equaliserState = {0, 0, SELECT, ADJUST, &settingsState, {}, EQUALISER, "Equaliser"};       //change EQ settings
    playbackState = {1, 0, ADJUST, SELECT, &settingsState, {}, PLAYBACK, "Playback"};          //change playback mode
    brightnessState = {2, 0, SELECT, ADJUST, &settingsState, {}, BRIGHTNESS, "Brightness"};    //change brightness level of screen

    //playlists state children
    songsState = {0, 0, PLAY, SELECT, &playlistState, {}, SONGS, "All songs"};              //pick song to play
}