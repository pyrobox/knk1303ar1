/* KNK1303AR1 rev.1 | FirstStep Firing System | https://forumfajerwerki.pl
 * file: cmd_tc.cpp | date: 2023.11.12
 */
/*==========1==========2==========3==========4==========5==========6==========7==========*/
/*
public:
void cmd_TC();
boolean cmdtc_testCheck();
*/
#include <Arduino.h>
#include <EEPROM.h>
#include "headerfile.hpp"
#include "unit_var.hpp"

extern boolean comenabled; //(ino)
extern int iArm; //(set_setup) arm.h+s 0..3
extern boolean bCueAddr; //(out_inout) true = addr.sec in the range 0..3

boolean bCueTest = false; //em test result
boolean bTestFlag = false; //potwierdzenie wykonania testu

//!!! jesli test zostanie wykonany to ustawiana jest flaga bTestFlag=true
//!!! jesli bTestFlag=true to po wyslaniu wyniku ustawiamy bTestFlag=false
//!!! jesli bTestFlag=false to wysylamy zawsze nagatywny wynik testu 

/*=======================================================================================*/
//cmd_TC: {1<crc>4<TC>2<adr>2<mod>2<cue>2}1 =14 //cmd_TC= test_cue

void cmd_TC() {
  cmdfc_setAddr(); //set addr.mod/sec/cha + check bCueAddr
  bCueTest = false;
  bTestFlag = false;
  if(( bCueAddr == true) && ( iArm == 0)) { //addr.sec in the range 0..3 and arm.safe
    bCueTest = cmdtc_testCheck(); 
    bTestFlag = true;
  }
  knk1303ar1_outputReset(); //74595 reset
  comenabled = true;
}

/*=======================================================================================*/
/*-------------------------------------------------------------------TEST_KNK1303AR1-----*/
boolean cmdtc_testCheck() { //time_exe =298msec
  //--set cue
  knk1303ar1_outputSet();           //set cue
  myDelay( 290);                    //relay_on debounce (10) + visual pause (280)
  //--test cue
  digitalWrite( pin_inTest, HIGH);  //input_pullup
  myDelay( 3);                      //wait =3msec
  int i = digitalRead( pin_inTest); //read input
  if( i == LOW) return true;        //em wired
    else        return false;       //em not wired
  //--reset all cues
  knk1303ar1_outputReset();         //reset cue
  myDelay( 5);                      //relay_off debounce
}
