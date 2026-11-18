#ifndef OLED_H
#define OLED_H

#include <Arduino.h>
#include <U8g2lib.h>
#include "SPI_HAL.h"
#include "UI_model.h"

class OLED_Screen {
    private:
        UI_model& model_;

    public:
        void OLED_init();

        UI_model& model() {return model_;}

        void showList(int count);

        void displayFileText();

        void draw_screen(OLED_STATE state);

        OLED_Screen(UI_model& model) : model_(model) {
            OLED_init();
        }

        ~OLED_Screen() {}

        void display();
    
};

#endif // OLED_H