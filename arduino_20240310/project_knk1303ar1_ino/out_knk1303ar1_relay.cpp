/* KNK1303AR1 rev.1 | FirstStep Firing System | https://forumfajerwerki.pl
 * file: out_knk1303ar1_relay.cpp | date: 2023.10.27
 */
/*==========1==========2==========3==========4==========5==========6==========7==========*/
/*
public:
void knk1303ar1_outputSet();
void knk1303ar1_outputReset();
void test_74595();
*/
#include <arduino.h>
#include "headerfile.hpp"
#include "unit_var.hpp"

extern byte isec; //(out_inout) cue.sec //0..15
extern byte icha; //(out_inout) cue.cha //0..15

/*=======================================================================================*/
#include <HC595.h>;

const int chipCount = 2; //number of serialy connected 74HC595 (8 maximum)
//const int pin_74595data  = 4; // Pin DS (14) of the 74HC595-Data In
//const int pin_74595latch = 5; // Pin ST_CP (12) of the 74HC595-Latch
//const int pin_74595clock = 6; // Pin SH_CP (11) of the 74HC595-Clock

HC595 relayArray( chipCount, pin_74595latch, pin_74595clock, pin_74595data);
int lastPin = relayArray.lastPin();

/*
74595_1 =>  C11|C12|C13|C14|C21|C22|C23|C24
74595_2 =>  H11|H12|H13|H14|H21|H22|H23|H24
SEC_1 : CHA_1..4   => C11 + H11..H14 => 1 + 9..12
        CHA_5..8   => C12 + H11..H14 => 2 + 9..12
        CHA_9..12  => C13 + H11..H14 => 3 + 9..12
        CHA_13..16 => C14 + H11..H14 => 4 + 9..12
SEC_2 : CHA_1..4   => C11 + H21..H24 => 1 + 13..16
        CHA_5..8   => C12 + H21..H24 => 2 + 13..16
        CHA_9..12  => C13 + H21..H24 => 3 + 13..16
        CHA_13..16 => C14 + H21..H24 => 4 + 13..16
SEC_3 : CHA_1..4   => C21 + H11..H14 => 5 + 9..12
        CHA_5..8   => C22 + H11..H14 => 6 + 9..12
        CHA_9..12  => C23 + H11..H14 => 7 + 9..12
        CHA_13..16 => C24 + H11..H14 => 8 + 9..12
SEC_4 : CHA_1..4   => C21 + H21..H24 => 5 + 13..16
        CHA_5..8   => C22 + H21..H24 => 6 + 13..16
        CHA_9..12  => C23 + H21..H24 => 7 + 13..16
        CHA_13..16 => C24 + H21..H24 => 8 + 13..16
*/
/*---------------------------------------------------------------------------------------*/
void knk1303ar1_outputSet() {
  relayArray.reset();
  int is = isec; //0..3  / sekcja 1..4, po korekcie adresu o iAdrS=1..13
  int ic = icha; //0..15 / kanal 1..16
  int pinC = 1; //1..8
  int pinH = 9; //9..16
  
  //SEC_1/is=0 lub SEC_2/is=1
  if( is <= 1) {                             //CHA:
    if( ic <= 3)               { pinC = 0; } //1..4
    if((ic > 3) && (ic <= 7))  { pinC = 1; } //5..8
    if((ic > 7) && (ic <= 11)) { pinC = 2; } //9..12
    if( ic > 11)               { pinC = 3; } //13..16
  }
  //SEC_3/is=2 lub SEC_4/is=3
  if( is > 1) {
    if( ic <= 3)               { pinC = 4; }
    if((ic > 3) && (ic <= 7))  { pinC = 5; }
    if((ic > 7) && (ic <= 11)) { pinC = 6; }
    if( ic > 11)               { pinC = 7; }
  }
  //SEC_1/is=0 lub SEC_3/is=2
  if( is == 0 || is == 2) {                                        //CHA:
    if((ic == 0) || (ic == 4) || (ic == 8)  || (ic == 12)) { pinH = 8; }  //1,5,9,13
    if((ic == 1) || (ic == 5) || (ic == 9)  || (ic == 13)) { pinH = 9; } //2,6,10,14
    if((ic == 2) || (ic == 6) || (ic == 10) || (ic == 14)) { pinH = 10; } //3,7,11,15
    if((ic == 3) || (ic == 7) || (ic == 11) || (ic == 15)) { pinH = 11; } //4,8,12,16
  }
  //SEC_2/is=1 lub SEC_4/is=3
  if( is == 1 || is == 3) { 
    if((ic == 0) || (ic == 4) || (ic == 8)  || (ic == 12)) { pinH = 12; }
    if((ic == 1) || (ic == 5) || (ic == 9)  || (ic == 13)) { pinH = 13; }
    if((ic == 2) || (ic == 6) || (ic == 10) || (ic == 14)) { pinH = 14; }
    if((ic == 3) || (ic == 7) || (ic == 11) || (ic == 15)) { pinH = 15; }
  }
  relayArray.setPin( pinC, ON); //set pin C (cold)
  relayArray.setPin( pinH, ON); //set pin H (hot)
}

/*---------------------------------------------------------------------------------------*/
void knk1303ar1_outputReset() {
  relayArray.reset();
}

/*------------------------------------------------------------------------------TEST-----*/
void test_74595() {
  for( isec = 0; isec <= 3; isec++) {
    for( icha = 0; icha <= 15; icha++) {
      knk1303ar1_outputSet(); //c11..14/c21..24_h11..14/h21..24_on
      myDelay( 500);
      digitalWrite( pin_outFire, HIGH); //fire_on
      myDelay( 500);
      digitalWrite( pin_outFire, LOW); //fire_off   
    }
  }
  knk1303ar1_outputReset();
}
