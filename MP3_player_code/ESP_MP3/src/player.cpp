#include "player.h"
/************************************************************************************************************************** 
 * methods on core 1, actually doing things
*/

volatile uint8_t dbgHead[8];

#if USE_VS1053
static bool waitDreq(uint32_t ms) {
    uint32_t t = millis();
    while (!digitalRead(AUDIO_DREQ_PIN)) if (millis() - t > ms) return false;
    return true;
}

static void resetPlayer(Adafruit_VS1053* player) {
    player->softReset();
    waitDreq(500);
    player->sciWrite(0x03, 0x6000);       // SCI_CLOCKF = 3.0x multiplier
    waitDreq(100);
}
#endif

bool player::playerStart(const char* path, audio_t audio) {
    #if USE_VS1053
    track.close();
    resetPlayer(vs);
    track = getSD()->open(path, O_RDONLY);
    if (!track) { paused = 1; return false; }
    bufLen = bufPos = 0;
    stopPos = skipTags(track, audio);
    paused = 0;
    return (bool)track;
    #elif USE_TLV320DAC3100
    cleanUpTrack();
    track = new AudioFileSourceSdFat(path, audio);
    switch (audio) {
        case MP3V2:
            id3 = new AudioFileSourceID3(track);
            decoder = new AudioGeneratorMP3();
            decoder->begin(id3, i2s);
            break;
        case OGG:
            decoder = new AudioGeneratorOpus();
            decoder->begin(track, i2s);
            break;
        default:
            break;
    }
    bool opened = track->isOpen();
    if(!opened) {paused = 1; return 0;}
    paused = 0;
    return opened;
    #else
    return 0;
    #endif
}

void player::stopPlaying() {
    //if no track is open we are already not playing anything
    #if USE_VS1053
    if (track) {
        track.close();
    }
    #elif USE_TLV320DAC3100
    if (track->isOpen()) {
        decoder->stop();
        cleanUpTrack();
    }
    #endif
}

void player::fill_buff() {
    if(paused) return;
    #if USE_VS1053
    if (!track) return;
    while(playerReadyForData()) {
        if (bufPos >= bufLen) {
            int n = track.read(buf, sizeof(buf));
            for (int i = 0; i < 8; i++) dbgHead[i] = buf[i];
            if (n > 0) {
                uint64_t pos = track.curPosition();
                if (pos > (uint64_t)stopPos) n -= (pos - stopPos);
            }
            if (n <= 0) {
                stopPlaying();
                finished = 1;
                return; 
            }
            bufLen = n; bufPos = 0;
        }
        int n = min(ADUIO_BUFFER_SIZE, bufLen - bufPos); // the chip takes 32-byte chunks
        vs->playData(buf + bufPos, n);
        bufPos += n;
    }
    #elif USE_TLV320DAC3100
    if(decoder && decoder->isRunning()) {
        if(!decoder->loop()) {
            stopPlaying();
            finished = 1;
            return;
        }
    }
    #endif
}

void player::task() {
    std::lock_guard<std::mutex> guard(busMutex);

    paused = pendingPause;

    switch(pendingCMD) {
        case PLAY:
            playerStart(pendingPath, pendingAudio);
            active = 1;
            pendingCMD = IDLE;
            break;
        case STOP:
            stopPlaying();
            pendingCMD = IDLE;
            active = 0;
            break;
        default:
            //fill_buff();
            sineTest();

    }
}

/*********************************************************************************************************************** 
 * public methods on core 0, posting requests
*/

void player::start_req(const Tag_t& tag) {
    std::lock_guard<std::mutex> guard(busMutex);
    strncpy(pendingPath, tag.path, sizeof(pendingPath) - 1);
    pendingPath[sizeof(pendingPath) - 1] = '\0';
    pendingAudio = tag.ID;
    pendingPause = 0;
    pendingCMD = PLAY;
}