#ifndef STATE_H
#define STATE_H

#include <Arduino.h>
#include "sd_card.h"
#include "UI_model.h"

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
        UI_model& model_;
        State *state_;
    public:
        context(UI_model& model, State *state) : model_(model), state_(nullptr) {
            this->transition_to(state);
        }

        ~context() {
            delete state_;
        }
        UI_model& model() {return model_;}

        void transition_to(State *state) {
            if (this->state_ != nullptr)
                delete this->state_;
            this->state_ = state;
            this->state_->set_context(this);
            this->state_->setScreen();
            Serial.println(model().title);
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
        void handle_push() override;
        void handle_rot(int dir) override {
            context_->model().scrollScreen(dir);
        }
        void setScreen() override {
            context_->model().show_list(HOME_SCREEN, "Home", 2, scrolling, nullptr);
        }
};

class State_Settings : public State {
    public:
        void handle_push() override {
            switch(context_->model().current_select) {
                case -1: 
                    context_->transition_to(new State_Home());
                    return;
                default: return;
            }
        }
        void handle_rot(int dir) override {
            context_->model().scrollScreen(dir);
        }
        void setScreen() override {
            context_->model().show_list(SETTINGS_SCREEN, "Settings", 7, 1, nullptr);
        }
};

class State_Playlist : public State {
    private:
        int  level = 0;                       // 0 = playlist list, 1 = songs
        char playlist[LIST_NAME_LEN] = {0};   // chosen playlist folder
        void show_playlists();
        void show_songs();
    public:
        void handle_push() override;
        void handle_rot(int dir) override {
            context_->model().scrollScreen(dir);
        }
        void setScreen() override {
            show_playlists();
        }
};

class State_Playing : public State {
    private:
        char playlist_[LIST_NAME_LEN];
        Tag_t current_song;
        int count_;
        int idx_;
        void showSong();
    public:
        State_Playing(const char* playlist, int start_index);
        void handle_push() override;
        void handle_rot(int dir) override {
            context_->model().scrollScreen(dir);
        }
        void setScreen() override {
            context_->model().show_list(PLAYING_SCREEN, playlist_, count_, 1, getListName);
            showSong();
        };
};

#endif // STATE_H