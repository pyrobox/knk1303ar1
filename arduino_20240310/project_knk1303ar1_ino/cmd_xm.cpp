/* KNK1303AR1 rev.1 | FirstStep Firing System | https://forumfajerwerki.pl
 * file: cmd_xm.cpp | date: 2023.10.26
 */
/*==========1==========2==========3==========4==========5==========6==========7==========*/
/*
public:
void cmd_XM();
*/
#include <Arduino.h>
#include "headerfile.hpp"
#include "unit_var.hpp"

#include "LiquidCrystal_PCF8574.h"
extern LiquidCrystal_PCF8574 lcd;

extern boolean comenabled; //(ino)

boolean bXmStart = false; //XM_start_flag false/true
int iXmNext = 0;          //XM_cmd_counter 0/1

/*=======================================================================================*/
//cmd_XM: {1<crc>4<XM>2}1 =8 const={1E7FXM} //cmd_XM= start_time

void cmd_XM() { 
  iXmNext = 0;     //cmd counter reset
  bXmStart = true; //xm start flag set
  lcd.setCursor( 6, 1); lcd.print( F( "STEP:     ")); //line_2
  comenabled = true;
}
