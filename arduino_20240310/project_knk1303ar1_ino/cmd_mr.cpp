/* KNK1303AR1 rev.1 | FirstStep Firing System | https://forumfajerwerki.pl
 * file: cmd_mr.cpp | date: 2023.10.26
 */
/*==========1==========2==========3==========4==========5==========6==========7==========*/
/*
public:
void cmd_MR();
*/
#include <Arduino.h>
#include <EEPROM.h>
#include "headerfile.hpp"
#include "unit_var.hpp"

extern boolean comenabled; //(ino)
extern char hDataBuffer[47]; //(ino)

//void(* resetFunc) (void) = 0;
extern resetModule(); //(ino)

/*=======================================================================================*/
//cmd_MR: {1<crc>4<MR>2}1=8        //cmd_MR= reset_all
//cmd_MR: {1<crc>4<MR>2<adr>2}1=10 //cmd_MR= reset_<adr>_pbms2019

void cmd_MR() {
  //-----<adr>
  int i = strlen( hDataBuffer); //get cmd data len
  if( i == 10) { //len=10 -> <adr>
    char c = hDataBuffer[8]; int g = hex2dec( c) + 1; //cmd_addr.grp
         c = hDataBuffer[7]; int u = hex2dec( c) + 1; //cmd_addr.unt
    int g2 = EEPROM.read( 11);                        //eep_addr.grp
    int u2 = EEPROM.read( 12);                        //eep_addr.unt
    if(( g == g2) && ( u == u2)) { //compatible addrs
      resetModule();
      comenabled = true; return; //exit
    } else { comenabled = true; return; } //different addrs -> exit
  }
  //-----all
  resetModule(); //soft reset
  comenabled = true;
}
