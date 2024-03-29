/* KNK1303AR1 rev.1 | FirstStep Firing System | https://forumfajerwerki.pl
 * file: cmd_fp.cpp | date: 2023.11.12
 */
/*==========1==========2==========3==========4==========5==========6==========7==========*/
/*
public:
void cmd_FP();
*/
#include <Arduino.h>
#include <EEPROM.h>
#include "headerfile.hpp"
#include "unit_var.hpp"

extern boolean comenabled; //(ino)
extern char hDataBuffer[47]; //(ino)

/*=======================================================================================*/
//cmd_FP: {1<crc>4<FP>2<adr>2<mod>2<cue>2<pls>3}1=17       //cmd_FP= fire_pulse
//cmd_FP: {1<crc>4<FP>2<adr>2<mod>2<cue>2<pls>3<one>1}1=18 //cmd_FP= fire_pulse_one

void cmd_FP() {
  int i = strlen( hDataBuffer); //get cmd data len
  if( i == 18) { //len=18 -> <one>
    char c = hDataBuffer[16]; //cmd_addr.one(unt)_hex=0..F
    int n = hex2dec( c) + 1;  //cmd_addr.one(unt)_int=1..16
    int u = EEPROM.read( 12); //eep_addr.unt_int=1..16
    if( n != u) { comenabled = true; return; }; //different addrs -> exit
  }
  cmdfc_readPulse(); //iPulse 100..900msec
  cmdfc_setAddr();
  cmdfc_fireUp();
  comenabled = true;
}
/*
void inout_readPulse() {
  char htmp[4]; htmp[3] = '\0';
  for( int i = 0; i <3; i++) { htmp[i] = hDataBuffer[i + 13]; } //<pls>=hDataBuffer[13..15]
  iPulse = atoi(htmp); //dec_100..900
}
*/