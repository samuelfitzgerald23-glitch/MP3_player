#ifndef STATE_H
#define STATE_H

#include <Arduino.h>
#include <algorithm>

#include "sd_card.h"
#include "UI_model.h"
#include "player.h"

class PlayQueue {
    private:
        char playlist_[LIST_NAME_LEN] = {0};
        char names_[MAX_LIST_ITEMS][LIST_NAME_LEN];
        int  count_ = 0;
        int  idx_   = 0;
        bool repeat_ = 1;
    public:
        // copy from the list cache; call while it still holds the songs
        void load(const char* playlist, int start) {
            strncpy(playlist_, playlist, LIST_NAME_LEN - 1);
            playlist_[LIST_NAME_LEN - 1] = '\0';
            count_ = getListCount();
            for (int i = 0; i < count_; i++) {
                strncpy(names_[i], getListName(i), LIST_NAME_LEN - 1);
                names_[i][LIST_NAME_LEN - 1] = '\0';
            }
            idx_ = (start < count_) ? start : 0;
        }
        void setRepeat(bool b) {repeat_ = b;}
        bool empty() const { return count_ == 0; }
        int  index() const { return idx_; }
        void set(int i)    { if (i >= 0 && i < count_) idx_ = i; }
        bool next()        {
            if(idx_ + 1 < count_) {idx_++; return 1;}
            if(repeat_ && count_) {idx_ = 0; return 1;}
            return 0;
        }
        bool prev()        {
            if (idx_ > 0) {idx_--; return 1;}
            if(repeat_ && count_) {idx_ = count_ - 1; return 1;}
            return 0;
        }
        void path(char* out, size_t n) const {
            snprintf(out, n, "/playlists/%s/%s", playlist_, names_[idx_]);
        }
        // void addToQueue(const char* songToAdd) {
        //     if(count_++ > MAX_LIST_ITEMS) return;                   //queue at max size, cannot add more
        //     count_++;
        //     int start = (idx_++ > MAX_LIST_ITEMS) ? 0 : idx_++;     //if idx at end of queue add to the start of the queue
        //     for (size_t i = count_-1; i < start; i--) {
        //         names_[i] = names_[i-1];
        //     }
        // }
};

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
        player *audio_;
        PlayQueue queue_;
    public:
        context(UI_model& model, State *state, player *audio) 
        : model_(model), state_(nullptr), audio_(audio) {
            this->transition_to(state);
        }

        ~context() {
            delete state_;
        }
        UI_model& model() {return model_;}
        player* audio() {return audio_;}
        PlayQueue* queue() {return &queue_;}

        void transition_to(State *state) {
            if (this->state_ != nullptr)
                delete this->state_;
            this->state_ = state;
            this->state_->set_context(this);
            this->state_->setScreen();
            Serial.println(model().title);
        }

        void playCurrent() {
            char path[160];
            queue_.path(path, sizeof(path));
            Tag_t tag = getMetaData(path);
            Serial.printf("playing: '%s'\n", tag.path);
            //format metadata to display on OLED
            formatTag(tag, model_.songInfo, sizeof(model_.songInfo));
            //start playing the next song
            audio_->start_req(tag);
            model_.paused = 0;
        }

        /*
        handle user events and pass them to the current state
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

        //button 1
        //single press
        void request_skip() {
            if(!audio_->isActive()) return;
            if(queue_.next()) playCurrent();
            else request_stop();
        }
        //double press
        void request_back() {
            if(!audio_->isActive()) return;
            queue_.prev();
            playCurrent();
        }

        //button 2
        //single press
        void request_pause_play() {
            if(audio_->isActive()) {
            model_.paused = !model_.paused;
            audio_->pause_req(model_.paused);
            }
        }

        //double press
        void request_stop() {
            if(audio_->isActive()) audio_->stop_req();
        }
};

class State_Home : public State {
    public:
        void handle_push() override;
        void handle_rot(int dir) override {
            context_->model().scrollScreen(dir);
        }
        void setScreen() override {
            context_->model().show_list(HOME_SCREEN, "Home", 2, 0, nullptr);
        }
};



static const char* const SETTINGS_NAMES[] = {
    "Power Save",
    "Volume",
    "Repeat Playlist",
    //"Equaliser",
    //"Shuffle Playlist",
};

#define N_SETTINGS (sizeof(SETTINGS_NAMES) / sizeof(SETTINGS_NAMES[0]))

inline const char* show_setting(int i) {
    if (i < 0 || i >= N_SETTINGS) return "";
    return SETTINGS_NAMES[i];
}

class State_Settings : public State {
    private:
        //default settings
        //bool powerSaveDefault = 0;    //not required?
        int volumeDefault = 20;
        // int treble = 0;              //not implamented yet
        // int bass = 0;
        bool repeatDefault = 1;
        //bool shufflePlaylist = 0;     //maybe later

        void applyDefaults() {
            //apply default settings
            context_->audio()->setVol(volumeDefault);
            context_->queue()->setRepeat(repeatDefault);
        }

    public:
        void handle_push() override;
        void handle_rot(int dir) override {
            context_->model().scrollScreen(dir);
        }
        void setScreen() override {
            context_->model().show_list(SETTINGS_SCREEN, "Settings", N_SETTINGS, 1, show_setting);
        }
};

class State_PowerSave : public State {
    public:
        void handle_push() override {
            context_->model().setPowerMode(0);
            context_->transition_to(new State_Settings);
            return;
        };
        void handle_rot(int dir) override {
            return;
        }
        void setScreen() override {
            context_->model().show_list(SETTINGS_SCREEN, "Power save", 1, 1, nullptr);
            context_->model().setPowerMode(1);
        }
};

//stay on setting screen, but encoder changes volume, pressing sets vol
class State_volume : public State {
    private:
        int tempVol;
    public:
        State_volume() {
            tempVol = context_->model().volume;
        }
        void handle_push() override {
            context_->audio()->setVol(tempVol);
            context_->transition_to(new State_Settings);
            return;
        };
        void handle_rot(int dir) override {
            tempVol = constrain((tempVol-dir), 0, 256);
            context_->model().volume = tempVol;
        }
        void setScreen() override {
            return;
        };
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
        int count_;
        
    public:
        State_Playing(const char* playlist);
        void handle_push() override;
        void handle_rot(int dir) override {
            context_->model().scrollScreen(dir);
        }
        void setScreen() override {
            context_->model().show_list(PLAYING_SCREEN, playlist_, count_, 1, getListName);
            context_->playCurrent();
        };
};

#endif // STATE_H