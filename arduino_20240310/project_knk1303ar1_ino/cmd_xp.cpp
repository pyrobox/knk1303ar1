/* KNK1303AR1 rev.1 | FirstStep Firing System | https://forumfajerwerki.pl
 * file: cmd_xp.cpp | date: 2023.10.26
 */
/*==========1==========2==========3==========4==========5==========6==========7==========*/
/*
public:
void cmd_XP();
*/
#include <Arduino.h>
#include "headerfile.hpp"
#include "unit_var.hpp"

#include "LiquidCrystal_PCF8574.h"
extern LiquidCrystal_PCF8574 lcd;

extern boolean comenabled; //(ino)

int iHB = 0; //local var

extern time_XPstart(); //(ino)
extern led_show( byte i); //(ino)

/*=======================================================================================*/
//cmd_XP: {1<crc>4<XP>2}1 =8 const={DDE3XP} //cmd_XP= heartbeat

void cmd_XP() {
  iHB++;
  if( iHB == 2) { //heartbeat every two cmd_XP
    lcd.setCursor( 15, 1); lcd.print( "\01");
    led_show( 4); //led_blue_on //!!! sprawdzic led_show() bo problem z key_arm
    iHB = 0;    
    time_XPstart(); 
    comenabled = true;
  } 
  else comenabled = true;
}
