/* KNK1303AR1 rev.1 | FirstStep Firing System | https://forumfajerwerki.pl
 * file: set_arm.cpp | date: 2023.10.27
 */
/*==========1==========2==========3==========4==========5==========6==========7==========*/
/*
public:
void arm_Check();
void key_On();
void key_Off();
*/
#include <Arduino.h>
#include <EEPROM.h>
#include "headerfile.hpp"
#include "unit_var.hpp"

#include "LiquidCrystal_PCF8574.h"
extern LiquidCrystal_PCF8574 lcd;

extern byte iArmH;     //(set_setup) 0/1 arm.hard
extern byte iArmS;     //(set_setup) 0/2 arm.soft
extern byte iArm;      //(set_setup) 0..3 arm.h+s
extern byte iReset;    //(set_setup) 0/1 soft reset flag
extern byte iLcdLine2; //(menu1)

extern led_show( byte i); //(ino)

/*=======================================================================================*/
void arm_Check() { 
  int i = iArmH + iArmS; //=iArm
  if( iLcdLine2 == 1) { i = 4; }
  lcd.setCursor( 0, 1);
  switch ( i) {                       
    case 0: lcd.print("SAFE "); led_show( 0); break; //green
    case 1: lcd.print("ARM.H"); led_show( 1); break; //yellow
    case 2: lcd.print("ARM.S"); led_show( 2); break; //yellow
    case 3: lcd.print("ARMED"); led_show( 3); break; //red
    case 4: break; //nic nie pokazuje na lcd
  }
}

/*---------------------------------------------------------------------------------------*/
void key_On() { //button_pressed
  iArmH = 1; //arm.hard_on
  iArm = iArmH + iArmS;
  arm_Check();
}
//----------
void key_Off() { //button_released
  iArmH = 0; //arm.hard_off
  iArm = iArmH + iArmS;
  arm_Check();
}
