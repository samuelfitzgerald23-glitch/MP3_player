#include "player.h"
/************************************************************************************************************************** 
 * methods on core 1, actually doing things
*/

bool player::playerStart(const char* path, Tag_t* tag) {
    track.close();
    track = getSD()->open(path, O_RDONLY);
    bufLen = bufPos = 0;
    stopPos = skipTags(track, tag);
    return (bool)track;
}

void player::stopPlaying() {
    //if no track is open we are already not playing anything
    if (track) {
        track.close();
        vs->softReset();
    }
    playingMusic = 0;
}

void player::fill_buff() {
    if (!track || playingMusic) return;
    while(vs->readyForData()) {
        if (bufPos >= bufLen) {
            bufLen = track.read(buf, sizeof(buf));
            //if the number of bytes read is 0 or less, or we have reach the determined end of file, stop playing
            if (bufLen <= 0 || bufPos >= stopPos) {stopPlaying(); return;}
            bufPos = 0;
        }
        int n = min(VS1053_DATABUFFERLEN, bufLen - bufPos); // the chip takes 32-byte chunks
        vs->playData(buf + bufPos, n);
        bufPos += n;
    }
}

void player::pauseMusic() {
    playingMusic = !playingMusic;
}

void player::task() {

}

/*********************************************************************************************************************** 
 * public methods on core 0, posting requests
*/

