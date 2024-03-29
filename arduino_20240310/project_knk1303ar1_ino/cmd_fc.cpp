/* KNK1303AR1 rev.1 | FirstStep Firing System | https://forumfajerwerki.pl
 * file: cmd_fc.cpp | date: 2023.11.12
 */
/*==========1==========2==========3==========4==========5==========6==========7==========*/
/*
public:
void cmd_FC();
void cmdfc_setAddr();
void cmdfc_readPulse();
void cmdfc_fireUp();
private:
void cmdfc_readCueCmd();
void cmdfc_readCueEep();
void cmdfc_checkCue();
*/
#include <Arduino.h>
#include <EEPROM.h>
#include "headerfile.hpp"
#include "unit_var.hpp"

extern boolean comenabled; //(ino)
extern char hDataBuffer[47]; //(ino)
extern byte iPulse; //(set_setup) 0, 100..900msec
extern byte iAdrM;  //(set_setup) addr.mod
extern byte iAdrS;  //(set_setup) addr.sec
extern byte iArm;   //(set_setup) arm.h+s 0..3

int imod = 0;  
int isec = 0;
int icha = 0;
int imodeep = 0;  
int iseceep = 0;
int ichaeep = 0;

boolean bCueAddr = false;       //true = addr.sec in the range 0..3
int knk1303ar1_timeFireUp = 40; //10+25+5=40msec czas wykonywania proc inout_fireUp()

/*=======================================================================================*/
//cmd_FC: {1<crc>4<FC>2<adr>2<mod>2<cue>2}1=14 //cmd_FC= fire_cue

void cmd_FC() {
  iPulse = 0; 
  cmdfc_setAddr();
  cmdfc_fireUp();
  comenabled = true;
}

/*=======================================================================================*/
/*-------------------------------------------------------------------------------ARM-----*/
void cmdfc_armCheck() {} //robi to automatycznie sw4 key_arm

/*------------------------------------------------------------------------------ADDR-----*/
//cmd_ FO{1<crc>4<FC>2<adr>2<mod>2<cue>2}1=14

void cmdfc_readCueCmd() { //z cmd odczytujemy cmd_cue.mod/sec/cha_hex=0..F i zamieniamy na _dec=0..15
  char c;
  c = hDataBuffer[10]; imod = hex2dec( c); //cmd_cue.mod_dec=0..3
  c = hDataBuffer[11]; isec = hex2dec( c); //cmd_cue.sec_dec=0..15
  c = hDataBuffer[12]; icha = hex2dec( c); //cmd_cue.cha_dec=0..15
}
//----------
void cmdfc_readCueEep() { //odczytujemy addr modulu eep_cue.mod/sec_dec=1..16
  imodeep = iAdrM; //EEPROM.read( 13); //cue.mod_dec=1..4  /iAdrM
  iseceep = iAdrS; //EEPROM.read( 14); //cue.sec_dec=1..16 /iAdrS
}
//----------
void cmdfc_checkCue() { //sprawdzamy czy cmd_cue miesci sie w zakresie eep_cue
  bCueAddr = false; 
  if( imod != (imodeep - 1)) return; //nie zgodne adresy imod oraz iAdrM-1
  if( isec  < (iseceep - 1)) return; //adres isec jest mniejszy niz iAdrS-1
  if( isec  > (iseceep + 2)) return; //adres isec jest wiekszy niz iAdrS+2
  isec = isec - (iseceep - 1); //dopasowanie cue.sec z cmd do zakresu cue.sec modulu
                               //np. isec=12..15 a iseceep(iAdrS)=13 wtedy isec=0..3 i jest ok
  bCueAddr = true; //potwierdzenie ze isec jest teraz w zakresie 0..3 obslugiwanym przez modul
}
//----------
void cmdfc_setAddr() {
  cmdfc_readCueCmd();
  cmdfc_readCueEep();
  cmdfc_checkCue();

  //Serial.print( "cue: "); Serial.print( imod); //<=== !!! TEST !!! 
  //Serial.print( "/");     Serial.print( isec);  
  //Serial.print( "/");     Serial.print( icha); 
}

/*-----------------------------------------------------------------------------PULSE-----*/
void cmdfc_readPulse() {
  char htmp[4]; htmp[3] = '\0';
  for( int i = 0; i <3; i++) { htmp[i] = hDataBuffer[i + 13]; } //<pls>=hDataBuffer[13..15]
  iPulse = atoi(htmp); //dec_100..900
}

/*------------------------------------------------------------------------------FIRE-----*/
void cmdfc_fireUp() { //time_exe =40msec
  if(( bCueAddr == true) && ( iArm == 3)) { //addr.sec in the range 0..3 and arm.armed
    //--set cue
    knk1303ar1_outputSet();           //set cue
    myDelay( 10);                     //relay_on debounce
    //--fire cue 
    digitalWrite( pin_outFire, HIGH); //relay_fire_on
    //--pulse
    if( iPulse |= 0) { 
      myDelay( iPulse);               //pulse= 100..900msec
      iPulse = 0;
    } else myDelay( 25);              //standard pulse= 25msec
    digitalWrite( pin_outFire, LOW);  //relay_fire_off
    //--reset all cues
    knk1303ar1_outputReset();         //reset cue
    myDelay( 5);                      //relay_off debounce   
  }
}
