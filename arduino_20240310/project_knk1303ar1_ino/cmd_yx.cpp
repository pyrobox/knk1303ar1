/* KNK1303AR1 rev.1 | FirstStep Firing System | https://forumfajerwerki.pl
 * file: cmd_yx.cpp | date: 2023.10.26
 */
/*==========1==========2==========3==========4==========5==========6==========7==========*/
/*
public:
void cmd_YX();
*/
#include <Arduino.h>
#include "headerfile.hpp"
#include "unit_var.hpp"

#include "LiquidCrystal_PCF8574.h"
extern LiquidCrystal_PCF8574 lcd;

extern boolean comenabled; //(ino)
extern boolean bTimerYX; //(ino) timerYX flag

/*=======================================================================================*/
//cmd_YX: {1<crc>4<YX>2<adr>2}1=10       //cmd_YX= slave_master_link_test
//answer= {1<crc>4<YX>2<adr>2}1=10       //answer= SMLT_answer
//cmd_YX: {1<crc>4<YX>2<adr>2<ser>9}1=19 //cmd_YX= slave_master_link_test_pbms2019
//answer= {1<crc>4<YX>2<adr>2<ser>9}1=19 //answer= SMLT_answer_pbms2019

void cmd_YX() { 
  bTimerYX = false; //loop timer stop
  lcd.setCursor( 6, 1); lcd.print( F( "LINK OK   ")); //line_2
  myDelay( 2000); //wait & show link.ok =2sec
  lcd.setCursor( 6, 1); lcd.print( F( "          ")); //line_2
  comenabled = true;
}
