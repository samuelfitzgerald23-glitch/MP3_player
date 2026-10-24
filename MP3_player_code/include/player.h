#ifndef PLAYER_H
#define PLAYER_H

#include "Arduino.h"
#include "SPI_HAL.h"
#include "sd_card.h"
#include <Adafruit_VS1053.h>

#define BUFFSIZE 512

enum PLAYER_STATE {
    PLAYING,
    PAUSED,
    STOPPED
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
        bool playingMusic = 0;

        bool playerStart(const char* path, Tag_t* tag);

        void fill_buff();

        void pauseMusic();
        
        void stopPlaying();

    public:
        PLAYER_STATE state = STOPPED;
        
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

        void task();
        
};

#endif //PLAYER_H