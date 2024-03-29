/* KNK1303AR1 rev.1 | FirstStep Firing System | https://forumfajerwerki.pl
 * file: cmd_ss.cpp | date: 2023.10.27
 */
/*==========1==========2==========3==========4==========5==========6==========7==========*/
/*
public:
void cmd_SS();
*/
#include <Arduino.h>
#include <EEPROM.h>
#include <CRC16.h>
#include "headerfile.hpp"
#include "unit_var.hpp"

extern boolean comenabled; //(ino)
extern char tabhex[17]; //(unit_var) {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F','\0'}
extern char hSendBuffer[47]; //(out_485)
extern char hsecCheck[5]; //(cmd_ts)
extern boolean bTestFlag; //(cmd_tc)

/*=======================================================================================*/
//cmd_SS:        {1<crc>4<SS>2<adr>2}1 =10       //cmd_SS= send_test_section
//cmd_SS_answer: {1<crc>4<SS>2<adr>2<tds>4}1 =14 //answer= send_test_section_answer
//<tds>=0000..FFFF <- cha:1..4/5..8/9..12/13..16

void cmd_SS() {
  //--przygotowac dane 
  char htmp[9]; htmp[8] = '\0';
  htmp[0] = 'S'; 
  htmp[1] = 'S'; //<cmd>2 = SS
  int a = EEPROM.read( 12); htmp[2] = tabhex[a - 1]; //eep_addr.unt
      a = EEPROM.read( 11); htmp[3] = tabhex[a - 1]; //eep_addr.grp
  htmp[4] = hsecCheck[0]; //cha_1..4
  htmp[5] = hsecCheck[1]; //cha_5..8
  htmp[6] = hsecCheck[2]; //cha_9..12
  htmp[7] = hsecCheck[3]; //cha_13_16
  if( bTestFlag == true) {
    bTestFlag = false;
  } else {
    htmp[4] = '0';
    htmp[5] = '0';
    htmp[6] = '0';
    htmp[7] = '0';
  } 
  //--policzyc crc
  CRC16 tmpcrc;
  tmpcrc.setPolynome( 0x1021); 
  for( int i = 0; i < 8; i++) { tmpcrc.add( htmp[i]); } 
  uint16_t itmpcrc = tmpcrc.getCRC(); 
  char htmpcrc[5];
  sprintf( htmpcrc, "%04X", itmpcrc); 
  //--przygotowac cmd_answer
  hSendBuffer[0] = '{';
  for( int i1 = 0; i1 <= 3; i1++) { hSendBuffer[i1 + 1] = htmpcrc[i1]; } //hSendBuffer[1..4]
  for( int i2 = 0; i2 <= 7; i2++) { hSendBuffer[i2 + 5] = htmp[i2]; }    //hSendBuffer[5..12]
  hSendBuffer[13] = '}'; //14 chars ok
  hSendBuffer[14] = '\0'; 
  //--wyslac cmd_answer
  rs485_print(); //print hSendBuffer
  //--  
  comenabled = true;
}
