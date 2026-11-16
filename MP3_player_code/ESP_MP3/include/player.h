#ifndef PLAYER_H
#define PLAYER_H

#define USE_VS1053 0
#define USE_TLV320DAC3100 0

#include "Arduino.h"
#include "SPI_HAL.h"
#include "sd_card.h"
#undef FILE_READ
#undef FILE_WRITE

#if USE_VS1053
#include <Adafruit_VS1053.h>
#endif

#define BUFFSIZE 512

#if USE_VS1053
#define ADUIO_BUFFER_SIZE VS1053_DATABUFFERLEN
#else
#define ADUIO_BUFFER_SIZE 32
#endif

enum PLAYER_CMD {
    IDLE,
    STOP,
    PLAY,
};

class player {
    private:
        #if USE_VS1053
        Adafruit_VS1053 *vs;
        #endif

        FsFile track;
        uint8_t buf[BUFFSIZE];
        int bufLen = 0;
        int bufPos = 0;
        int stopPos;

        int volume = 20;
        bool paused = 0;
        volatile bool finished = 0;
        volatile bool active = 0;

        bool playerStart(const char* path, audio_t audio);

        bool playerReadyForData() {
            #if USE_VS1053
            return vs->readyForData();
            #else
            return 0;
            #endif
        }

        void fill_buff();
        
        void stopPlaying();

        PLAYER_CMD pendingCMD = IDLE;
        bool pendingPause = 0;
        char pendingPath[160];
        audio_t pendingAudio = NONE;

    public: 

        player() {
            #if USE_VS1053
            //init VS1053 chip
            vs = new Adafruit_VS1053(AUDIO_RESET_PIN, AUDIO_CS_PIN, AUDIO_DATA_PIN, AUDIO_DREQ_PIN);
            if (!vs->begin()) {
                Serial.println("VS1053 failed to initialise");
                //while(1);
            } else {
                Serial.println("VS1053 initialised");
                vs->setVolume(volume, volume);
                //vs->sineTest(0x44, 500);
            }
            Serial.printf("VS1053 status = 0x%04X\n", vs->sciRead(0x01));
            #endif
        }

        bool getFinished() {
            bool f = finished;
            finished = 0;
            return f;
        }
        bool isActive() {return active;}

        //called on core 0
        void start_req(const Tag_t& tag);
        void pause_req(bool p) {
            std::lock_guard<std::mutex> guard(busMutex);
            pendingPause = p;
        }
        void stop_req() {
            std::lock_guard<std::mutex> guard(busMutex);
            pendingCMD = STOP;
        }

        void setVol(int i) {
            std::lock_guard<std::mutex> guard(busMutex);
            volume = i;
            #if USE_VS1053
            vs->setVolume(i, i);
            #endif
        }

        //called on core 1
        void task();
        
};

#endif //PLAYER_H