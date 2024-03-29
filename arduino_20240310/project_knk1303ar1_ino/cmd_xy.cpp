/* KNK1303AR1 rev.1 | FirstStep Firing System | https://forumfajerwerki.pl
 * file: cmd_xy.cpp | date: 2023.10.26
 */
/*==========1==========2==========3==========4==========5==========6==========7==========*/
/*
public:
void cmd_XY();
*/
#include <Arduino.h>
#include <EEPROM.h>
#include <CRC16.h>
#include "headerfile.hpp"
#include "unit_var.hpp"

extern boolean comenabled; //(ino)
extern char tabhex[17]; //(unit_var0 {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F','\0'}
extern char hSendBuffer[47]; //(out_485)
extern char hIdno[10];  //(set_idno) {'K','K','0','1','2','3','0','0','1','\0'}
extern char hInfo[5];   //(set_info) {'0','0','0','0','\0'}

/*=======================================================================================*/
//cmd_XY: {1<crc>4<XY>2<adr>2}1=10             //cmd_XY= master_slave_link_test
//        {1<crc>4<XY>2<adr>2<inf>4}1=14       //answer= MSLT_answer
//        {1<crc>4<XY>2<adr>2<inf>4<pwr>6}1=20 //answer= MSLT_answer_pwr
//answer: {1<crc>4<XY>2<adr>2<ser>9}1=19       //answer= MSLT_answer_pbms2019 <- selected !!!

void cmd_XY() { 
  //--przygotowac dane 
  char htmp[14]; 
  htmp[0] = 'X'; 
  htmp[1] = 'Y'; //<cmd>2 = XY
  int a = EEPROM.read( 12); htmp[2] = tabhex[a - 1]; //addr.unt
      a = EEPROM.read( 11); htmp[3] = tabhex[a - 1]; //addr.grp
  for( int i1 = 0; i1 < 9; i1++) { htmp[i1 + 4] = hIdno[i1]; } //<ser>9 copy
  htmp[13] = '\0'; //2+2+9=13+1null
  //--policzyc crc
  CRC16 tmpcrc;
  tmpcrc.setPolynome( 0x1021); 
  for(int i = 0; i < 13; i++) { tmpcrc.add( htmp[i]); } 
  uint16_t itmpcrc = tmpcrc.getCRC(); 
  char htmpcrc[5];
  sprintf( htmpcrc, "%04X", itmpcrc); 
  //--przygotowac cmd_answer
  hSendBuffer[0] = '{';
  for( int i1 = 0; i1 <= 3; i1++) { hSendBuffer[i1 + 1] = htmpcrc[i1]; } //hSendBuffer[1..4]
  for( int i2 = 0; i2 <= 12; i2++) { hSendBuffer[i2 + 5] = htmp[i2]; }   //hSendBuffer[5..17]
  hSendBuffer[18] = '}'; //19 chars ok 
  hSendBuffer[19] = '\0'; 
  //--wyslac cmd_answer
  rs485_print(); //print hSendBuffer
  //-- 
  comenabled = true;
}
