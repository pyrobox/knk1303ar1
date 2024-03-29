/* KNK1303AR1 rev.1 | FirstStep Firing System | https://forumfajerwerki.pl
 * file: menu_3.cpp | date: 2023.10.26
 */
/*==========1==========2==========3==========4==========5==========6==========7==========*/
/*
public:
void menu_modTest();
void menu_modShow();
private:
void menu_check0();
void menu_check1();
void menu_check2();
void menu_check3();
void menu_check4();
boolean menu_cueTest();
void menu_secData();
void menu_secTest();
*/
#include <Arduino.h>
#include <EEPROM.h>
#include "headerfile.hpp"
#include "unit_var.hpp"

#include "LiquidCrystal_PCF8574.h"
extern LiquidCrystal_PCF8574 lcd;

extern boolean comenabled; //(ino)
extern char tabhex[17];    //(unit_var) {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F','\0'}
extern char hmodCheck[17]; //(cmd_tm) {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F','\0'}
extern char hsecCheck[5];  //(cmd_ts) {'0','0','0','0','\0'}
extern int isec; //(out_inout) range 0..3
extern int icha; //(out_inout) 
extern byte iArm; //(set_setup)

//locxal var:
char c1;
String stmp0;
String stmp1; //sec_1 
String stmp2; //sec_2 
String stmp3; //sec_3 
String stmp4; //sec_4 

/*=======================================================================================*/
void menu_check0() {
  switch( c1) {
    case '0': stmp0 = "xxxx"; break;
    case '1': stmp0 = "oxxx"; break;
    case '2': stmp0 = "xoxx"; break;
    case '3': stmp0 = "ooxx"; break;

    case '4': stmp0 = "xxox"; break;
    case '5': stmp0 = "oxox"; break;
    case '6': stmp0 = "xoox"; break;
    case '7': stmp0 = "ooox"; break;

    case '8': stmp0 = "xxxo"; break;
    case '9': stmp0 = "oxxo"; break;
    case 'A': stmp0 = "xoxo"; break;
    case 'B': stmp0 = "ooxo"; break;

    case 'C': stmp0 = "xxoo"; break;
    case 'D': stmp0 = "oxoo"; break;
    case 'E': stmp0 = "xooo"; break;
    case 'F': stmp0 = "oooo"; break;
  }
}

/*---------------------------------------------------------------------------------------*/
void menu_check1() { //sec_1
  c1 = hmodCheck[0]; menu_check0(); stmp1 = stmp0;
  c1 = hmodCheck[1]; menu_check0(); stmp1 = stmp1 + stmp0;
  c1 = hmodCheck[2]; menu_check0(); stmp1 = stmp1 + stmp0;
  c1 = hmodCheck[3]; menu_check0(); stmp1 = stmp1 + stmp0;
}

void menu_check2() { //sec_2
  c1 = hmodCheck[4]; menu_check0(); stmp2 = stmp0;
  c1 = hmodCheck[5]; menu_check0(); stmp2 = stmp2 + stmp0;
  c1 = hmodCheck[6]; menu_check0(); stmp2 = stmp2 + stmp0;
  c1 = hmodCheck[7]; menu_check0(); stmp2 = stmp2 + stmp0;
}

void menu_check3() { //sec_3
  c1 = hmodCheck[8];  menu_check0(); stmp3 = stmp0;
  c1 = hmodCheck[9];  menu_check0(); stmp3 = stmp3 + stmp0;
  c1 = hmodCheck[10]; menu_check0(); stmp3 = stmp3 + stmp0;
  c1 = hmodCheck[11]; menu_check0(); stmp3 = stmp3 + stmp0;
}

void menu_check4() { //sec_4
  c1 = hmodCheck[12]; menu_check0(); stmp4 = stmp0;
  c1 = hmodCheck[13]; menu_check0(); stmp4 = stmp4 + stmp0;
  c1 = hmodCheck[14]; menu_check0(); stmp4 = stmp4 + stmp0;
  c1 = hmodCheck[15]; menu_check0(); stmp4 = stmp4 + stmp0;
}

/*=======================================================================================*/
boolean menu_cueTest() {  //time_exe =18msec
  //--set cue
  knk1303ar1_outputSet();           //set cue
  myDelay( 100);                     //relay_on debounce + visual pause
  //--test cue
  digitalWrite( pin_inTest, HIGH);  //input_pullup
  myDelay( 3);                      //wait =3msec
  int i = digitalRead( pin_inTest); //read input
  if( i == LOW) return true;        //em wired
    else        return false;       //em not wired
  //--reset all cues
  knk1303ar1_outputReset();         //reset cue
  myDelay( 10);                     //relay_off debounce
}

/*---------------------------------------------------------------------------------------*/
void menu_secData( String s) {
  char c;
  int i, i1, i2, i3, i4;
  //--cha_1_4
  c = s[0]; if( c == '0') i1 = 0; else i1 = 1;
  c = s[1]; if( c == '0') i2 = 0; else i2 = 2;
  c = s[2]; if( c == '0') i3 = 0; else i3 = 4;
  c = s[3]; if( c == '0') i4 = 0; else i4 = 8;
  i = i1 + i2 + i3 + i4; c = tabhex[i]; hsecCheck[0] = c;
  //--cha_5_8
  c = s[4]; if( c == '0') i1 = 0; else i1 = 1;
  c = s[5]; if( c == '0') i2 = 0; else i2 = 2;
  c = s[6]; if( c == '0') i3 = 0; else i3 = 4;
  c = s[7]; if( c == '0') i4 = 0; else i4 = 8;
  i = i1 + i2 + i3 + i4; c = tabhex[i]; hsecCheck[1] = c;
  //--cha_9_12
  c = s[8]; if( c == '0') i1 = 0; else i1 = 1;
  c = s[9]; if( c == '0') i2 = 0; else i2 = 2;
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
void menu_secTest() {
  boolean bTest;
  char c;
  String s = "";
  for( icha = 0; icha <= 15; icha++) {
    bTest = false;
    bTest = menu_cueTest();
    if( bTest == true) c = '1'; else c = '0'; //cue test
    s = s + c; //sec test = string[16] 
  }
  menu_secData( s);
}

/*---------------------------------------------------------------------------------------*/
void menu_modTest() {
  lcd.setCursor( 0, 0); lcd.print( F( "EM TEST IS RUN  ")); //line_1
  lcd.setCursor( 0, 1); lcd.print( F( "WAIT...         ")); //line_2
  if( iArm == 0) { //eep_arm.safe
    for( isec = 0; isec <= 3; isec++) {
      menu_secTest(); //one section test
      int n = 4 * isec;
      for( int i = 0; i <= 3; i++) hmodCheck[n + i] = hsecCheck[i];
    }
    knk1303ar1_outputReset();
//    Serial.println( hmodCheck); //!!! TEST !!!
    menu_check1(); //-> stmp1 -> sec_1
    menu_check2(); //-> stmp2 -> sec_2
    menu_check3(); //-> stmp3 -> sec_3
    menu_check4(); //-> stmp4 -> sec_4
  } else {
    lcd.setCursor( 0, 1); lcd.print( F( "DISARM MODULE   ")); //line_2
    myDelay( 5000); //wait 5sec
  }
  lcd.clear();
  myDelay( 1000); //wait 1sec
  menu_buttonMenu();
}

/*=======================================================================================*/
void menu_modShow() {
  lcd.setCursor( 0, 0); lcd.print( F( "EM TEST RESULTS:")); //line_1
  lcd.setCursor( 0, 1); lcd.print( F( "o=OK x=BED      ")); //line_2
  myDelay( 3000); //wait 3sec
  //-----sec_1
  int asec = EEPROM.read( 14); //addr.sec
  lcd.setCursor( 0, 0); lcd.print( F( "SECTION         "));       //line_1
  lcd.setCursor( 8, 0); lcd.print( asec); lcd.print( " (1)");     //line_1 //print sec #
  lcd.setCursor( 0, 1); lcd.print( F( "                "));       //line_2 //clear
  myDelay( 1000); //wait 1sec
  //--
  lcd.setCursor( 0, 0); lcd.print( F( "1234567890123456"));       //line_1
  lcd.setCursor( 0, 1); lcd.print( stmp1);                        //line_2 //print result
  myDelay( 5000); //wait 5sec
  //-----sec_2
  lcd.setCursor( 0, 0); lcd.print( F( "SECTION         "));       //line_1
  lcd.setCursor( 8, 0); lcd.print( asec + 1); lcd.print( " (2)"); //line_1 
  lcd.setCursor( 0, 1); lcd.print( F( "                "));       //line_2
  myDelay( 1000); //wait 1sec
  //--
  lcd.setCursor( 0, 0); lcd.print( F( "1234567890123456"));       //line_1
  lcd.setCursor( 0, 1); lcd.print( stmp2);                        //line_2  
  myDelay( 5000); //wait 5sec
  //-----sec_3
  lcd.setCursor( 0, 0); lcd.print( F( "SECTION         "));       //line_1
  lcd.setCursor( 8, 0); lcd.print( asec + 2); lcd.print( " (3)"); //line_1
  lcd.setCursor( 0, 1); lcd.print( F( "                "));       //line_2
  myDelay( 1000); //wait 1sec
  //--
  lcd.setCursor( 0, 0); lcd.print( F( "1234567890123456"));       //line_1
  lcd.setCursor( 0, 1); lcd.print( stmp3);                        //line_2
  myDelay( 5000); //wait 5sec
  //-----sec_4
  lcd.setCursor( 0, 0); lcd.print( F( "SECTION         "));       //line_1
  lcd.setCursor( 8, 0); lcd.print( asec + 3); lcd.print( " (4)"); //line_1
  lcd.setCursor( 0, 1); lcd.print( F( "                "));       //line_2
  myDelay( 1000); //wait 1sec
  //--
  lcd.setCursor( 0, 0); lcd.print( F( "1234567890123456"));       //line_1
  lcd.setCursor( 0, 1); lcd.print( stmp4);                        //line_2
  myDelay( 5000); //wait 5sec
  //-----
  lcd.clear();
  myDelay( 1000); //wait 1sec
  menu_buttonMenu();
}
