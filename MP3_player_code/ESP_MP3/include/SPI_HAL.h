#ifndef SPI_HAL_H
#define SPI_HAL_H

#include <SPI.h>
#include "bus_locking.h"

#define SCL_PIN 18
#define TX_PIN 23    //MOSI
#define RX_PIN 19    //MISO

//for OLED screen
#define OLED_RESET_PIN 17
#define OLED_DC_PIN 16
#define OLED_CS_PIN 4

//for SD card
#define SD_CS_PIN 5

//void SPI_init(); 

#endif // SPI_HAL_H
