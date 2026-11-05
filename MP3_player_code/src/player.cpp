#include "player.h"
/************************************************************************************************************************** 
 * methods on core 1, actually doing things
*/

volatile uint8_t dbgHead[8];

static bool waitDreq(uint32_t ms) {
    uint32_t t = millis();
    while (!digitalRead(AUDIO_DREQ_PIN)) if (millis() - t > ms) return false;
    return true;
}

static void resetPlayer(Adafruit_VS1053* vs) {
    vs->softReset();
    waitDreq(500);
    vs->sciWrite(0x03, 0x6000);       // SCI_CLOCKF = 3.0x multiplier
    waitDreq(100);
}

bool player::playerStart(const char* path, audio_t audio) {
    track.close();
    resetPlayer(vs);
    track = getSD()->open(path, O_RDONLY);
    if (!track) { paused = 1; return false; }
    bufLen = bufPos = 0;
    stopPos = skipTags(track, audio);
    paused = 0;
    vs->setVolume(volume, volume);
    return (bool)track;
}

void player::stopPlaying() {
    //if no track is open we are already not playing anything
    if (track) {
        track.close();
    }
}

void player::fill_buff() {
    if (!track || paused) return;
    while(vs->readyForData()) {
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
        int n = min(VS1053_DATABUFFERLEN, bufLen - bufPos); // the chip takes 32-byte chunks
        vs->playData(buf + bufPos, n);
        bufPos += n;
    }
}

void player::task() {
    mutex_enter_blocking(&busMutex);

    paused = pendingPause;

    switch(pendingCMD) {
        case PLAY:
            if(playerStart(pendingPath, pendingAudio)) active = 1;
            pendingCMD = IDLE;
            break;
        case STOP:
            stopPlaying();
            pendingCMD = IDLE;
            active = 0;
            break;
        default:
            fill_buff();
    }

    mutex_exit(&busMutex);
}

/*********************************************************************************************************************** 
 * public methods on core 0, posting requests
*/

void player::start_req(const Tag_t& tag) {
    mutex_enter_blocking(&busMutex);
    strncpy(pendingPath, tag.path, sizeof(pendingPath) - 1);
    pendingPath[sizeof(pendingPath) - 1] = '\0';
    pendingAudio = tag.ID;
    pendingPause = 0;
    pendingCMD = PLAY;
    mutex_exit(&busMutex);
}