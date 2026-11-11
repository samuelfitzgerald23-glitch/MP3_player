#ifndef SPI_HAL_H
#define SPI_HAL_H

#include <SPI.h>
#include "bus_locking.h"

#define SCL_PIN 2
#define TX_PIN 3    //MOSI
#define RX_PIN 4    //MISO

//for OLED screen
#define OLED_RESET_PIN 6
#define OLED_DC_PIN 7
#define OLED_CS_PIN 8

//for SD card
#define SD_CS_PIN 12

//for audio decoder
#define AUDIO_RESET_PIN 14
#define AUDIO_CS_PIN 13
#define AUDIO_DATA_PIN 11
#define AUDIO_DREQ_PIN 15

void SPI_init(); 

#endif // SPI_HAL_H
