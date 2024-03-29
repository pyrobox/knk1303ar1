/* KNK1303AR1 rev.1 | FirstStep Firing System | https://forumfajerwerki.pl
 * file: cmd_tm.cpp | date: 2023.11.12
 */
/*==========1==========2==========3==========4==========5==========6==========7==========*/
/*
public:
void cmd_TM();
*/
#include <Arduino.h>
#include <EEPROM.h>
#include "headerfile.hpp"
#include "unit_var.hpp"

#include "LiquidCrystal_PCF8574.h"
extern LiquidCrystal_PCF8574 lcd;

extern boolean comenabled; //(ino)
extern char tabhex[17]; //(unit_var) {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F','\0'}
extern char hsecCheck[5]; //(cmd_ts)
extern int isec; //(out_inout) sec.range 0..3
extern int iArm; //(set_setup) arm.h+s 0..3
extern boolean bCueAddr; //(cmd_tc) true = addr.sec in the range 0..3
extern boolean bTestFlag; //(cmd_tc)

char hmodCheck[17] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F','\0'};

/*=======================================================================================*/
//cmd_TM: {1<crc>4<TM>2<adr>2<mod>2<cue>2}1 =14 //cmd_TM= test_module

void cmd_TM() {
  lcd.setCursor( 6, 1); lcd.print( F( "WAIT...   ")); //line_2
  cmdfc_setAddr(); //set addr.mod/sec/cha + check bCueAddr
  bTestFlag = false;
  if( iArm == 0) { //arm.safe, nie sprawdzamy bCueAddr bo testujemy zawsze sec 0..3
    for( isec = 0; isec <= 3; isec++) {
      cmd_tsOneSec(); //sec test
      int n = 4 * isec;
      for( int i = 0; i <= 3; i++) hmodCheck[n + i] = hsecCheck[i];
    }
    bTestFlag = true;
    knk1303ar1_outputReset(); //74595 reset
  } else {
    lcd.setCursor( 6, 1); lcd.print( F( "TEST ERR  ")); //line_2
    myDelay( 3000); //wait 3sec
  }
  lcd.setCursor( 6, 1); lcd.print( F( "          ")); //line_2
  comenabled = true;
}
