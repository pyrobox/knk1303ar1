/* KNK1303AR1 rev.1 | FirstStep Firing System | https://forumfajerwerki.pl
 * file: out_usb.cpp | date: 2023.10.27
 */
/*==========1==========2==========3==========4==========5==========6==========7==========*/
/*
public:
void usb_dataRead();
private:
void usb_setG(); 
void usb_setU();
void usb_setM();
void usb_setS();
*/
#include <arduino.h>
#include <EEPROM.h>
#include "headerfile.hpp"
#include "unit_var.hpp"

extern char hUsbBuffer[4]; //(ino) {'G','0','1','\0'}; //usb_terminal data buffer
extern byte iAdrG; //(set_setup) addr.grp =1..16 <- G01..G16 
extern byte iAdrU; //(set_setup) addr.unt =1..16 <- U01..U16 
extern byte iAdrM; //(set_setup) addr.mod =1..4  <- M01..M04
extern byte iAdrS; //(set_setup) addr.sec =1..16 <- S01..S16
byte iadrx; //local var

extern resetModule();

/*=======================================================================================*/
void usb_setG() { iAdrG = iadrx; EEPROM.write( 11, iAdrG); delay( 5); } 
void usb_setU() { iAdrU = iadrx; EEPROM.write( 12, iAdrU); delay( 5); }
void usb_setM() { iAdrM = iadrx; EEPROM.write( 13, iAdrM); delay( 5); }
void usb_setS() { iAdrS = iadrx; EEPROM.write( 14, iAdrS); delay( 5); }

/*---------------------------------------------------------------------------------------*/
void usb_dataRead() { 
  char htmp[3] = { '0', '1', '\0' };
  for( int i = 0; i < 2; i++) { htmp[i] = hUsbBuffer[i + 1]; }
  iadrx = atoi( htmp); //atoi -> converts a char string to an int value
  char c = hUsbBuffer[0];
  switch( c) {
    case 'G': usb_setG(); break; //grp
    case 'U': usb_setU(); break; //unt
    case 'M': usb_setM(); break; //mod
    case 'S': usb_setS(); break; //sec
    //case 'C': usb_setC(); break; //cha
    //case 'A': usb_setA(); break; //cha
  } 
  setup_readAddr();
  resetModule(); 
}
