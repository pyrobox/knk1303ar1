/* KNK1303AR1 rev.1 | FirstStep Firing System | https://forumfajerwerki.pl
 * file: unit_var.cpp | date: 2023.11.22
 */
/*==========1==========2==========3==========4==========5==========6==========7==========*/
/*
public:
void pinSetup();
*/
#include <Arduino.h>
#include "unit_var.hpp"

/*=======================================================================================*/
char tabdec[11] = {'0','1','2','3','4','5','6','7','8','9','\0'};
char tabhex[17] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F','\0'};

/*========================================================================NANO_EVERY=====*/
/*              NANO EVERY           ==>           KNK1303AR1  
                _________                          _________
        SCK|13 -|   ^   |- 12|CIPO        SPI_SCK -|   ^   |- SPI_MISO
           3V3 -|       |- 11|COPI            3V3 -|       |- SPI_MOSI
          AREF -|       |- 10                AREF -|       |- SPI_CS
         14/A0 -|       |- 9               OUT_FR -|       |- SPI_CE
         15/A1 -|       |- 8               IN_ARM -|       |- SPI_IRQ
         16/A2 -|       |- 7|RXD2         IN_TEST -|       |- SPI_NC
         17/A3 -|       |- 6|TXD3         SW_MENU -|       |- 595_CLOCK
     SDA/18/A4 -|       |- 5              I2C_SDA -|       |- 595_LATCH
     SCL/19/A5 -|       |- 4|INT1         I2C_SCL -|       |- 595_DATA
         20/A6 -|       |- 3|INT0|RXD3    SW_NEXT -|       |- INT0
         21/A7 -|       |- 2|TXD2          SW_SET -|       |- 485_DIR 
            5V -|       |- GND                 5V -|       |- GND
           RST -|       |- RST                  x -|       |- x
           GND -|       |- RX                 GND -|       |- 485_RX
           VIN -|_______|- TX                   x -|_______|- 485_TX
*/
/*==========================================================================ALIASES======*/
//declarations ---> unit_var.hpp/cpp

//-----485
const int pin_485dir = 2;
//-----int0
//const int pin_trigger = 3;
//-----595
const int pin_74595data  = 4;
const int pin_74595latch = 5;
const int pin_74595clock = 6;
//-----in/out
const int pin_inTest = A2;  //16
//const int pin_inArm = A1;   //15
const int pin_outFire = A0; //14
//-----switch
//const int pin_buttonMenu = A3; //17
//const int pin_buttonNext = A6; //20
//const int pin_buttonSet = A7;  //21

/*---------------------------------------------------------------------------------------*/
void pinSetup() {
  //-----485
  pinMode( pin_485dir, OUTPUT); digitalWrite( pin_485dir, LOW); //=2 //set receive mode
  //-----int0
  //pinMode( pin_trigger, INPUT); digitalWrite( pin_trigger, LOW); //=3 //irq INT0
  //-----595
  pinMode( pin_74595data,  OUTPUT); digitalWrite( pin_74595data,  LOW); //=4
  pinMode( pin_74595latch, OUTPUT); digitalWrite( pin_74595latch, LOW); //=5
  pinMode( pin_74595clock, OUTPUT); digitalWrite( pin_74595clock, LOW); //=6
  //-----in/out
  pinMode( pin_inTest, INPUT_PULLUP); digitalWrite( pin_inTest, HIGH); //=A0/14
  //pinMode( pin_inArm,  INPUT_PULLUP); digitalWrite( pin_inArm,  HIGH); //=A1/15 //-> ino
  pinMode( pin_outFire, OUTPUT);      digitalWrite( pin_outFire, LOW); //=A2/16
  //-----switch
  //pinMode( pin_buttonMenu, INPUT_PULLUP); digitalWrite( pin_buttonMenu, HIGH); //=A3 //-> ino
  //pinMode( pin_buttonNext, INPUT_PULLUP); digitalWrite( pin_buttonNext, HIGH); //=A6 //-> ino
  //pinMode( pin_buttonSet,  INPUT_PULLUP); digitalWrite( pin_buttonSet,  HIGH); //=A7 //-> ino
}
