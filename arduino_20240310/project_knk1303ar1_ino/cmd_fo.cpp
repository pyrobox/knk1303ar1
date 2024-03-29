/* KNK1303AR1 rev.1 | FirstStep Firing System | https://forumfajerwerki.pl
 * file: cmd_fo.cpp | date: 2023.11.12
 */
/*==========1==========2==========3==========4==========5==========6==========7==========*/
/*
publkic:
void cmd_FO();
*/
#include <Arduino.h>
#include <EEPROM.h>
#include "headerfile.hpp"
#include "unit_var.hpp"

extern boolean comenabled; //(ino)
extern char hDataBuffer[47]; //(ino)
extern byte iPulse; //(set_setup) 0, 100..900msec          

/*=======================================================================================*/
//cmd_FO: {1<crc>4<FO>2<adr>2<mod>2<cue>2<one>1}1=15 //cmd_FO= fire_one

void cmd_FO() {
  char c = hDataBuffer[13]; //cmd_addr.one(unt)_hex=0..F
  int n = hex2dec( c) + 1;  //cmd_addr.one(unt)_int=1..16
  int u = EEPROM.read( 12); //eep_addr.unt_int=1..16
  if( n != u) { comenabled = true; return; } //different addrs -> exit
  //--
  iPulse = 0;
  cmdfc_setAddr();
  cmdfc_fireUp();
  comenabled = true;
}
