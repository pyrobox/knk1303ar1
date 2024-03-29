/* KNK1303AR1 rev.1 | FirstStep Firing System | https://forumfajerwerki.pl
 * file: cmd_mn.cpp | date: 2023.10.26
 */
/*==========1==========2==========3==========4==========5==========6==========7==========*/
/*
public:
void cmd_MN();
void cmd_MF();
*/
#include <Arduino.h>
#include <EEPROM.h>
#include "headerfile.hpp"
#include "unit_var.hpp"

extern boolean comenabled; //(ino)
extern char hDataBuffer[47]; //(ino)
extern byte iArmH; //(set_setup) arm.hard 0/1
extern byte iArmS; //(set_setup) arm.soft 0/2
extern byte iArm;  //(set_setup) arm.h+s  0..3

/*=======================================================================================*/
//cmd_MN: {1<crc>4<MN>2}1=8        //cmd_MN= arm_on_all
//cmd_MN: {1<crc>4<MN>2<adr>2}1=10 //cmd_MN= arm_on_<adr>_pbms2019

void cmd_MN() {
  //-----<adr>
  int i = strlen( hDataBuffer); //get cmd data len
  if( i == 10) { //len=10 -> <adr>
    char c = hDataBuffer[8]; int g = hex2dec( c) + 1; //cmd_addr.grp
         c = hDataBuffer[7]; int u = hex2dec( c) + 1; //cmd_addr.unt
    int g2 = EEPROM.read( 11);                        //eep_addr.grp
    int u2 = EEPROM.read( 12);                        //eep_addr.unt
    if(( g == g2) && ( u == u2)) { //compatible addrs
      iArmS = 2; EEPROM.write( 2, iArmS); myDelay( 5); //arm.soft =2
      iArm = iArmH + iArmS;
      arm_Check();
      comenabled = true; return; //exit
    } else { comenabled = true; return; } //different addrs -> exit
  }
  //-----all
  iArmS = 2; EEPROM.write( 2, iArmS); myDelay( 5);
  iArm = iArmH + iArmS;
  arm_Check();
  comenabled = true;
}

/*---------------------------------------------------------------------------------------*/
//cmd_MF: {1<crc>4<MF>2}1=8        //cmd_MF= arm_off_all
//cmd_MF: {1<crc>4<MF>2<adr>2}1=10 //cmd_MF= arm_off_<adr>_pbms2019

void cmd_MF() {
  //-----<adr>
  int i = strlen( hDataBuffer); //get cmd data len
  if( i == 10) { //len=10 -> <adr>
    char c = hDataBuffer[8]; int g = hex2dec( c) + 1; //cmd_addr.grp
         c = hDataBuffer[7]; int u = hex2dec( c) + 1; //cmd_addr.unt
    int g2 = EEPROM.read( 11);                        //eep_addr.grp
    int u2 = EEPROM.read( 12);                        //eep_addr.unt
    if(( g == g2) && ( u == u2)) { //compatible addrs
      iArmS = 0; EEPROM.write( 2, iArmS); myDelay( 5); //arm.soft =0
      iArm = iArmH + iArmS;
      arm_Check();
      comenabled = true; return; //exit
    } else { comenabled = true; return; } //different addrs -> exit
  }
  //-----all
  iArmS = 0; EEPROM.write( 2, iArmS); myDelay( 5);
  iArm = iArmH + iArmS;
  arm_Check();
  comenabled = true;
}
