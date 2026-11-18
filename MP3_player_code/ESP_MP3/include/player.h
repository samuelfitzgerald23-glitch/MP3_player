#ifndef PLAYER_H
#define PLAYER_H

#define USE_VS1053 0
#define USE_TLV320DAC3100 1

#include "Arduino.h"
#include "SPI_HAL.h"
#include "sd_card.h"
#undef FILE_READ
#undef FILE_WRITE

#if USE_VS1053

#include <Adafruit_VS1053.h>
//for audio decoder (will most likely be removed in the future)
#define AUDIO_RESET_PIN 25
#define AUDIO_CS_PIN 26
#define AUDIO_DATA_PIN 27
#define AUDIO_DREQ_PIN 14

#elif USE_TLV320DAC3100

#include <Adafruit_TLV320DAC3100.h>
#include "Wire.h"
//#include <ESP_I2S.h>

#include "AudioFileSource.h"
#include "AudioGeneratorMP3.h"
#include "AudioGeneratorOpus.h"
#include "AudioOutputI2S.h"
#include "AudioFileSourceID3.h"

//for DAC and I2S output
#define AUDIO_RST_PIN 14

#define AUDIO_SDA_PIN 21
#define AUDIO_SCL_PIN 22

#define AUDIO_DIN_PIN 27
#define AUDIO_WSEL_PIN 26
#define AUDIO_BCK_PIN 25

#define SAMPLE_RATE 44100       //44.1kHz

static bool startDAC(Adafruit_TLV320DAC3100 &DAC, char* message) {
    //step 0. init I2C communication
    Serial.println("starting codec");
    if(!DAC.begin()) {
        strcpy(message, "Failed to initialise codec");
        return 0;
    }
    DAC.reset();
    //step 1. set up data i2s interface
    Serial.println("configing interface");
    if(!DAC.setCodecInterface(TLV320DAC3100_FORMAT_I2S, TLV320DAC3100_DATA_LEN_16)) {
        strcpy(message, "Failed to set of codec interface");
        return 0;
    }
    //step 2. set up clocks
    Serial.println("setting up clocks");
    if (!DAC.setCodecClockInput(TLV320DAC3100_CODEC_CLKIN_PLL) ||
      !DAC.setPLLClockInput(TLV320DAC3100_PLL_CLKIN_BCLK)) {
        strcpy(message, "Failed to configure codec clocks!");
        return 0;
    }
    //step 3. set up PLL (Phase Locked Loop)
    Serial.println("setting PLL vals");
    if(!DAC.setPLLValues(1, 1, 8, 0)) {
        strcpy(message, "Failed to set PLL values");
        return 0;
    }
    // Step 4: Configure DAC dividers
    Serial.println("setting DAC dividers");
    if (!DAC.setNDAC(true, 8) ||
        !DAC.setMDAC(true, 2) ||
        !DAC.setDOSR(128)) {
        strcpy(message, "Failed to configure DAC dividers!");
        return 0;
    }
    // Step 5: Power up PLL
    Serial.println("Starting PLL");
    if (!DAC.powerPLL(true)) {
        strcpy(message, "Failed to power up PLL!");
        return 0;
    }
    // Step 6: Configure DAC path - power up both left and right DACs
    Serial.println("setting up DAC path");
    if (!DAC.setDACDataPath(true, true, 
                            TLV320_DAC_PATH_NORMAL,
                            TLV320_DAC_PATH_NORMAL,
                            TLV320_VOLUME_STEP_1SAMPLE)) {
        strcpy(message, "Failed to configure DAC data path!");
        return 0;
    }
    // Step 7: Route DAC output to headphone
    Serial.println("routing DAC output");
    if (!DAC.configureAnalogInputs(TLV320_DAC_ROUTE_MIXER, // Left DAC to mixer
                                    TLV320_DAC_ROUTE_MIXER, // Right DAC to mixer
                                    false, false, false,    // No AIN routing
                                    false)) {               // No HPL->HPR
        strcpy(message, "Failed to configure DAC routing!");
        return 0;
    }
    // Step 8: Unmute DAC and set volume (higher for testing)
    Serial.println("unmuting");
    if (!DAC.setDACVolumeControl(
            false, false, TLV320_VOL_INDEPENDENT) || // Unmute both channels
        !DAC.setChannelVolume(false, 0.0) ||
        !DAC.setChannelVolume(true, 0.0)) {
        strcpy(message, "Failed to configure DAC volume control!");
        return 0;
    }

    //disable speaker amp
    DAC.enableSpeaker(0);

    // Headphone output
    DAC.configureHeadphoneDriver(true, true, TLV320_HP_COMMON_1_35V, false);
    DAC.setHPLVolume(true, 40);
    DAC.setHPRVolume(true, 40);
    DAC.configureHPL_PGA(0, true);
    DAC.configureHPR_PGA(0, true);

    return 1;
}

class AudioFileSourceSdFat : public AudioFileSource {
    private:
        SdFs *sd;
        FsFile file;
    public:
        AudioFileSourceSdFat(const char* path, audio_t audio) {
            sd = getSD();
            open(path);
            skipTags(file, audio);
        }

        virtual ~AudioFileSourceSdFat() override {
            close();
        }

        virtual bool close() override {
            if (file) {
                file.close();
            }
            return 1;
        }

        virtual bool open(const char* filename) override {
            close();
            file = sd->open(filename, O_RDONLY);
            return (bool)file;
        }

        virtual uint32_t read(void *data, uint32_t len) override {
            if (!file) return 0;
            int bytesRead = file.read(data, len);
            return (bytesRead < 0) ? 0 : (uint32_t)bytesRead;
        }

        virtual bool seek(int32_t pos, int dir) override {
            if (!file) return false;
            if (dir == SEEK_SET) return file.seekSet(pos);
            if (dir == SEEK_CUR) return file.seekCur(pos);
            if (dir == SEEK_END) return file.seekEnd(pos);
            return false;
        }

        virtual bool isOpen() override {
            return (bool)file;
        }

        virtual uint32_t getSize() override {
            if (!file) return 0;
            return file.size();
        }

        virtual uint32_t getPos() override {
            if (!file) return 0;
            return file.curPosition();
        }
};

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
        FsFile track;
        #elif USE_TLV320DAC3100
        //I2SClass i2s;
        AudioOutputI2S *i2s = nullptr;
        Adafruit_TLV320DAC3100 codec;

        AudioFileSourceSdFat *track = nullptr;
        AudioGenerator *decoder = nullptr;

        AudioFileSourceID3 *id3 = nullptr;

        void sineTest() {
            static int16_t buf[256 * 2];
            static float phase = 0;
            for (int i = 0; i < 256; i++) {
                int16_t s = (int16_t)(6000 * sinf(phase));   // about -14 dB: start quiet
                phase += 2.0f * PI * 440.0f / SAMPLE_RATE;
                if (phase > 2.0f * PI) phase -= 2.0f * PI;
                buf[2 * i] = s;                              // left
                buf[2 * i + 1] = s;                          // right
            }
            uint16_t done = 0;
            while (done < 256) {
                done += i2s->ConsumeSamples(buf + 2 * done, 256 - done);
                if (done < 256) delay(1);                        // DMA buffers full: yield, then retry the rest
            }
        }

        void cleanUpTrack() {
            //clean up function for audio decoder
            if(decoder) {
                delete decoder;
                decoder = nullptr;
            }
            if(id3) {
                delete id3;
                id3 = nullptr;
            }
            if(track) {
                delete track;
                track = nullptr;
            }
        }
        #endif

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
            #elif USE_TLV320DAC3100
            return 0;
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
            #elif USE_TLV320DAC3100
            //init DAC
            pinMode(AUDIO_RST_PIN, OUTPUT);                       // reset pulse before configuring
            digitalWrite(AUDIO_RST_PIN, LOW);  delay(100);
            digitalWrite(AUDIO_RST_PIN, HIGH); delay(100);
            Wire.begin(AUDIO_SDA_PIN, AUDIO_SCL_PIN);

            char message[256] = "";
            if(!startDAC(codec, message)) {
                Serial.print(message);
                while(1);
            }

            i2s = new AudioOutputI2S();
            i2s->SetPinout(AUDIO_BCK_PIN, AUDIO_WSEL_PIN, AUDIO_DIN_PIN);
            i2s->SetRate(SAMPLE_RATE);
            i2s->SetChannels(2);
            if(!i2s->begin()) {
                Serial.println("i2s failed to start");
                while(1);
            }
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

        void setVol(int vol) {
            vol = constrain(vol, 0, 254);
            volume = vol;
            #if USE_VS1053
            vs->setVolume(volume, volume);
            #elif USE_TLV320DAC3100
            bool mute = (vol >= 127);                        // -63.5 dB is the quietest the DAC reaches
            float dB = mute ? -63.5f : -0.5f * vol;

            codec.setDACVolumeControl(mute, mute);           // mute flags: true = muted
            codec.setChannelVolume(false, dB);               // left
            codec.setChannelVolume(true,  dB);               // right
            #endif
        }

        //called on core 1
        void task();
        
};

#endif //PLAYER_H