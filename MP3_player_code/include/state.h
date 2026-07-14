#ifndef STATE_H
#define STATE_H

#include <algorithm>

#define MAX_CHILDREN 10

enum STATEID {
    NOTHING = -1,
    START,
    SETTINGS,
    PLAYLISTS,
    SONGS,
    EQUALISER,
    PLAYBACK,
    BRIGHTNESS
};

struct state_t {
    int id;                                 //corrosponds to the index in the parents children array
    int numChildren;                        //number of children states I have
    void (*push)();                         //action required when an encoder push event occurs
    void (*rotate)(int dir);                //action required when an encoder rotate event occurs
    state_t* parentState;                   //the previous state before this one
    state_t* childrenStates[MAX_CHILDREN];  //a list of the next states
    STATEID name;                           //
    const char* displayName;                //name for display on screen
};

state_t* getState();
void changeState(int stateID);

void stateInit();

//rotate funcs
void changeSelect(int value);
void changeBrightness(int a);

//push funcs
void contextChange();
void indexSong();

#endif //STATE_H