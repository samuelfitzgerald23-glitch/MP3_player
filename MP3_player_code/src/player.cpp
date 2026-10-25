#include "player.h"
/************************************************************************************************************************** 
 * methods on core 1, actually doing things
*/

bool player::playerStart(const char* path, audio_t audio) {
    track.close();
    vs->softReset();
    track = getSD()->open(path, O_RDONLY);
    if (!track) { paused = 1; return false; }
    bufLen = bufPos = 0;
    stopPos = skipTags(track, audio);
    paused = 0;
    return (bool)track;
}

void player::stopPlaying() {
    //if no track is open we are already not playing anything
    if (track) {
        track.close();
        vs->softReset();
    }
    paused = 1;
}

void player::fill_buff() {
    if (!track || paused) return;
    while(vs->readyForData()) {
        if (bufPos >= bufLen) {
            int n = track.read(buf, sizeof(buf));
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

void player::pauseMusic() {
    paused = !paused;
}

void player::task() {
    mutex_enter_blocking(&busMutex);
    vs->setVolume(volume, volume);

    if(pendingPause) {pauseMusic(); pendingPause = 0;}

    switch(pendingCMD) {
        case PLAY:
            playerStart(pendingPath, pendingAudio);
            pendingCMD = IDLE;
            break;
        case STOP:
            stopPlaying();
            pendingCMD = IDLE;
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
    pendingCMD = PLAY;
    mutex_exit(&busMutex);
}