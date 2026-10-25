#ifndef PLAYER_H
#define PLAYER_H

#include "Arduino.h"
#include "SPI_HAL.h"
#include "sd_card.h"
#include <Adafruit_VS1053.h>

#define BUFFSIZE 512

enum PLAYER_CMD {
    IDLE,
    STOP,
    PLAY,
};

class player {
    private:
        Adafruit_VS1053 *vs;

        FsFile track;
        uint8_t buf[BUFFSIZE];
        int bufLen = 0;
        int bufPos = 0;
        int stopPos;

        int volume = 20;
        bool paused = 0;
        volatile bool finished = 0;

        bool playerStart(const char* path, audio_t audio);

        void fill_buff();

        void pauseMusic();
        
        void stopPlaying();

        PLAYER_CMD pendingCMD = IDLE;
        bool pendingPause = 0;
        char pendingPath[160];
        audio_t pendingAudio = NONE;

    public:
        
        player() {
            //init VS1053 chip
            vs = new Adafruit_VS1053(AUDIO_RESET_PIN, AUDIO_CS_PIN, AUDIO_DATA_PIN, AUDIO_DREQ_PIN);
            if (!vs->begin()) {
                Serial.println("VS1053 failed to initialise");
                while(1);
            }
            Serial.print("VS1053 initialised");
            vs->setVolume(volume, volume);
        }

        bool getFinished() {return finished;}

        //called on core 0
        void start_req(const Tag_t& tag);
        void pause_req() {
            mutex_enter_blocking(&busMutex);
            pendingPause = 1;
            mutex_exit(&busMutex);
        }
        void stop_req() {
            mutex_enter_blocking(&busMutex);
            pendingCMD = STOP;
            mutex_exit(&busMutex);
        }

        //called on core 1
        void task();
        
};

#endif //PLAYER_H