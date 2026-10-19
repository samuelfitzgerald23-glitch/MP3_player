#ifndef STATE_H
#define STATE_H

#include <Arduino.h>
#include "sd_card.h"
#include "OLED.h"

class context;

class State {
    protected:
        context *context_;
    public:
        virtual ~State(){}

        void set_context(context *context) {
        this->context_ = context;
    }

    // general class for all states, each state will implement its own handle() function
    virtual void handle_push() = 0;
    virtual void handle_rot(int dir) = 0;
    virtual void setScreen() = 0;
        
};

class context {
    private:
        State *state_;
    public:
        context(State *state) : state_(nullptr) {
            this->transition_to(state);
        }

        ~context() {
            delete state_;
        }

        void transition_to(State *state) {
            if (this->state_ != nullptr)
                delete this->state_;
            this->state_ = state;
            this->state_->set_context(this);
            this->state_->setScreen();
        }

        /*
        handle encoder events and pass them to the current state
        */

        void request_push() {
            this->state_->handle_push();
        }

        void request_rot(int dir) {
            this->state_->handle_rot(dir);
        }

        void request_setScreen() {
            this->state_->setScreen();
        }

        /*
        functions to pause/play, skip, and go back a song
        should always be able to call these independent of the state
        */
        void request_skip() {}

        void request_pause_play() {}

        void request_back() {}
};

class State_Settings;
class State_Playlist;

class State_Home : public State {
    private:
        bool scrolling = false;
    public:
        void handle_push() override {
            switch(getOLEDScreen()->current_select) {
                case SELECT0: context_->transition_to(new State_Settings());
                case SELECT1: context_->transition_to(new State_Playlist());
                default: return;
            }
        }
        void handle_rot(int dir) override {
            scroll_screen(dir, getOLEDScreen());
        }
        void setScreen() override {
            setOLEDState(HOME_SCREEN);
            toggle_back(0);
        }
};

class State_Settings : public State {
    public:
        void handle_push() override {
            switch(getOLEDScreen()->current_select) {
                case BACK: context_->transition_to(new State_Home());
                case SELECT0: return;
                case SELECT1: return;
                default: return;
            }
        }
        void handle_rot(int dir) override {
            scroll_screen(dir, getOLEDScreen());
        }
        void setScreen() override {
            setOLEDState(SETTINGS_SCREEN);
            toggle_back(1);
        }
};

class State_Playlist : public State {
    public:
        void handle_push() override {
            switch(getOLEDScreen()->current_select) {
                case BACK: context_->transition_to(new State_Home());
                case SELECT0: return;
                case SELECT1: return;
                default: return;
            }
        }
        void handle_rot(int dir) override {
            scroll_files(dir);
        }
        void setScreen() override {
            setOLEDState(PLAYLIST_SCREEN);
            toggle_back(1);
        }
};

#endif // STATE_H