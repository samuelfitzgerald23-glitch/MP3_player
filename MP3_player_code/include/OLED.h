#ifndef OLED_H
#define OLED_H

#include <Arduino.h>
#include <U8g2lib.h>
#include "SPI_HAL.h"
#include "UI_model.h"

#define ZERO_X 0
#define ZERO_Y 1

#define ICON_X_ZERO (ZERO_X + 2)
#define ICON_Y_ZERO (ZERO_Y + 3)

#define TEXT_X_ZERO (ZERO_X + 14)
#define TEXT_Y_ZERO (ZERO_Y + 20)
#define CURSOUR_Y_ZERO (ICON_Y_ZERO + 10)

#define ART_X_ZERO (ZERO_X + 80)
#define ART_Y_ZERO (ZERO_Y + 10)

#define BACK_X_ZERO (ZERO_X + 115)
#define BACK_Y_ZERO (ZERO_Y + 53)

class OLED_Screen {
    private:
        UI_model& model_;

    public:
        void OLED_init();

        UI_model& model() {return model_;}

        void displayFileText();

        void draw_screen(OLED_STATE state);

        OLED_Screen(UI_model model) : model_(model) {
            OLED_init();
        }

        ~OLED_Screen() {}

        void display();
    
};

#endif // OLED_H