#ifndef ANIMATION_H
#define ANIMATION_H

#pragma once
#include <Arduino.h>
#include <string.h>
#include <stdint.h>

#define ANIM_MAX_VISIBLE 32          // widest text window, in characters

struct AnimFrame {
    const uint8_t* bits;             // XBM bitmap data, in flash
    int8_t   dx, dy;                 // offset from the object's origin for this frame
    uint16_t holdMs;                 // how long this frame is shown
};

class AnimatedObject {
    public:
        enum Kind : uint8_t { BITMAP, TEXT };

        // frame-by-frame bitmap (position offsets per frame give you movement paths too)
        static AnimatedObject Bitmap(int x, int y, uint8_t w, uint8_t h,
                                     const AnimFrame* frames, uint8_t count, bool loop = true) {
            AnimatedObject a(BITMAP, x, y);
            a.w_ = w; a.h_ = h; a.frames_ = frames; a.count_ = count; a.loop_ = loop;
            return a;
        }

        // character-step scrolling text; s must outlive the object (a literal or a long-lived buffer)
        static AnimatedObject Text(int x, int y, uint8_t visibleChars, const char* s = "",
                                   uint16_t stepMs = 250, uint16_t pauseMs = 1500, uint8_t gap = 4) {
            AnimatedObject a(TEXT, x, y);
            a.visible_ = visibleChars > ANIM_MAX_VISIBLE ? ANIM_MAX_VISIBLE : visibleChars;
            a.stepMs_ = stepMs; a.pauseMs_ = pauseMs; a.gap_ = gap;
            a.setText(s, 0);
            return a;
        }

        // ---- shared ----
        Kind kind() const { return kind_; }
        int  x() const { return x_ + (kind_ == BITMAP && count_ ? frames_[cur_].dx : 0); }
        int  y() const { return y_ + (kind_ == BITMAP && count_ ? frames_[cur_].dy : 0); }
        bool empty() const { return kind_ == TEXT ? len_ == 0 : count_ == 0; }

        void reset(uint32_t now) {                  // restart from the beginning
            cur_ = 0; offset_ = 0; last_ = now;
            pauseUntil_ = now + pauseMs_;
            playing_ = true;
            rebuild();
        }
        void stop()                 { playing_ = false; }
        void resume(uint32_t now)   { playing_ = true; last_ = now; }

        void tick(uint32_t now) {
            if (!playing_) return;
            if (kind_ == BITMAP) {
                if (count_ == 0 || now - last_ < frames_[cur_].holdMs) return;
                last_ = now;
                if (cur_ + 1 < count_) cur_++;
                else if (loop_)        cur_ = 0;
                else                   playing_ = false;
            } else {
                if (len_ <= visible_) return;                          // fits: stays still
                if ((int32_t)(now - pauseUntil_) < 0) return;          // holding at the start
                if (now - last_ < stepMs_) return;
                last_ = now;
                if (++offset_ >= len_ + gap_) {                        // finished a pass
                    offset_ = 0;
                    pauseUntil_ = now + pauseMs_;
                }
                rebuild();
            }
        }

        // ---- bitmap ----
        const uint8_t* bits() const { return (kind_ == BITMAP && count_) ? frames_[cur_].bits : nullptr; }
        uint8_t w() const { return w_; }
        uint8_t h() const { return h_; }

        // ---- text ----
        // call again whenever the characters behind s change (the length is measured here)
        void setText(const char* s, uint32_t now) {
            src_ = s ? s : "";
            len_ = strlen(src_);
            reset(now);
        }
        const char* text() const { return win_; }   // exactly visible_ characters

    private:
        AnimatedObject(Kind k, int x, int y) : kind_(k), x_(x), y_(y) { win_[0] = '\0'; }

        void rebuild() {
            if (kind_ != TEXT) return;
            const int period = len_ + gap_;
            for (int i = 0; i < visible_; i++) {
                char c = ' ';
                if (len_ <= visible_) { if (i < len_) c = src_[i]; }
                else { int p = (offset_ + i) % period; if (p < len_) c = src_[p]; }
                win_[i] = c;
            }
            win_[visible_] = '\0';
        }

        Kind kind_;
        int  x_, y_;
        bool playing_ = true;
        uint32_t last_ = 0, pauseUntil_ = 0;

        // bitmap
        const AnimFrame* frames_ = nullptr;
        uint8_t w_ = 0, h_ = 0, count_ = 0, cur_ = 0;
        bool loop_ = true;

        // text
        const char* src_ = "";
        int len_ = 0, offset_ = 0;
        uint8_t visible_ = 0, gap_ = 4;
        uint16_t stepMs_ = 250, pauseMs_ = 1500;
        char win_[ANIM_MAX_VISIBLE + 1];
};

#endif //ANIMATION_H