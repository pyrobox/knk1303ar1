/* KNK1303AR1 rev.1 | FirstStep Firing System | https://forumfajerwerki.pl
 * file: cmd_ts.cpp | date: 2023.11.12
 */
/*==========1==========2==========3==========4==========5==========6==========7==========*/
/*
public:
void cmd_TS();
void cmd_tsOneSec();
private:
void cmd_tsData();
*/
#include <Arduino.h>
#include <EEPROM.h>
#include "headerfile.hpp"
#include "unit_var.hpp"

#include "LiquidCrystal_PCF8574.h"
extern LiquidCrystal_PCF8574 lcd;

extern boolean comenabled; //(ino)
extern char tabhex[17]; //(unit_var) {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F','\0'}
extern int isec; //(out_inout) sec.range 0..3
extern int icha; //(out_inout)
extern int iArm; //(set_setup) arm.h+s 0..3
extern boolean bCueAddr; //(out_inout) true = addr.sec in the range 0..3
extern boolean bTestFlag; //(cmd_tc)

char hsecCheck[5] = {'0','0','0','0','\0'}; //sec em test result 0000..FFFF

/*=======================================================================================*/
void cmd_tsSecData( String s) { //-> 0000..FFFF
  char c;
  int i, i1, i2, i3, i4;
  //--cha_1_4
  c = s[0];  if( c == '0') i1 = 0; else i1 = 1;
  c = s[1];  if( c == '0') i2 = 0; else i2 = 2;
  c = s[2];  if( c == '0') i3 = 0; else i3 = 4;
  c = s[3];  if( c == '0') i4 = 0; else i4 = 8;
  i = i1 + i2 + i3 + i4; c = tabhex[i]; hsecCheck[0] = c;
  //--cha_5_8
  c = s[4];  if( c == '0') i1 = 0; else i1 = 1;
  c = s[5];  if( c == '0') i2 = 0; else i2 = 2;
  c = s[6];  if( c == '0') i3 = 0; else i3 = 4;
  c = s[7];  if( c == '0') i4 = 0; else i4 = 8;
  i = i1 + i2 + i3 + i4; c = tabhex[i]; hsecCheck[1] = c;
  //--cha_9_12
  c = s[8];  if( c == '0') i1 = 0; else i1 = 1;
  c = s[9];  if( c == '0') i2 = 0; else i2 = 2;
  c = s[10]; if( c == '0') i3 = 0; else i3 = 4;
  c = s[11]; if( c == '0') i4 = 0; else i4 = 8;
  i = i1 + i2 + i3 + i4; c = tabhex[i]; hsecCheck[2] = c;
  //--cha_13_16
  c = s[12]; if( c == '0') i1 = 0; else i1 = 1;
  c = s[13]; if( c == '0') i2 = 0; else i2 = 2;
  c = s[14]; if( c == '0') i3 = 0; else i3 = 4;
  c = s[15]; if( c == '0') i4 = 0; else i4 = 8;
  i = i1 + i2 + i3 + i4; c = tabhex[i]; hsecCheck[3] = c;
}

/*---------------------------------------------------------------------------------------*/
void cmd_tsOneSec() {
  boolean bTest;
  char c;
  String s = "";
  for( icha = 0; icha <= 15; icha++) {
    bTest = false;
    bTest = cmdtc_testCheck();
    if( bTest == true) c = '1'; else c = '0'; //cue test
    s = s + c; //sec test -> s string[16] 
  }
  cmd_tsSecData( s); //-> hsecCheck[]
}

/*---------------------------------------------------------------------------------------*/
//cmd_TS: {1<crc>4<TS>2<adr>2<mod>2<cue>2}1 =14 //cmd_TS= test section

void cmd_TS() {
  lcd.setCursor( 6, 1); lcd.print( F( "WAIT...   ")); //line_2
  cmdfc_setAddr(); //set addr.mod/sec/cha + check bCueAddr
  bTestFlag = false;
  if(( bCueAddr == true) && ( iArm == 0)) { //addr.sec in the range 0..3 and arm.safe
    cmd_tsOneSec(); //sec test
    bTestFlag = true;
    knk1303ar1_outputReset(); //74595 reset
  } else {
    lcd.setCursor( 6, 1); lcd.print( F( "TEST ERR  ")); //line_2
    myDelay( 3000); //wait 3sec
  }
  lcd.setCursor( 6, 1); lcd.print( F( "          ")); //line_2
  comenabled = true;
}

