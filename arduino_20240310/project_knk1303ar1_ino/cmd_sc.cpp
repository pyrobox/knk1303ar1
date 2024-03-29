/* KNK1303AR1 rev.1 | FirstStep Firing System | https://forumfajerwerki.pl
 * file: cmd_sc.cpp | date: 2023.10.27
 */
/*==========1==========2==========3==========4==========5==========6==========7==========*/
/*
public:
void cmd_SC();
*/
#include <Arduino.h>
#include <EEPROM.h>
#include <CRC16.h>
#include "headerfile.hpp"
#include "unit_var.hpp"

extern boolean comenabled; //(ino)
extern char tabhex[17]; //(unit_var) {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F','\0'}
extern char hSendBuffer[47]; //(out_485)
extern boolean bCueTest; //(cmd_tc) em test result
extern boolean bTestFlag; //(cmd_tc)

/*=======================================================================================*/
//cmd_SC:        {1<crc>4<SC>2<adr>2}1 =10       //cmd_SC= send_test_cue
//cmd_SC_answer: {1<crc>4<SC>2<adr>2<0/1>1}1 =11 //answer= send_test_cue_answer

void cmd_SC() {
  char c = '0';
  if( bCueTest == true) c = '1'; //em wired
    else                c = '0'; //em not wired
  //--przygotowac dane 
  char htmp[6]; 
  htmp[0] = 'S'; 
  htmp[1] = 'C'; //<cmd>2 = SC
  int a = EEPROM.read( 12); htmp[2] = tabhex[a - 1]; //eep_addr.unt
      a = EEPROM.read( 11); htmp[3] = tabhex[a - 1]; //eep_addr.grp
  htmp[4] = c; //em test result
  if( bTestFlag == true) bTestFlag = false; else htmp[4] = '0'; //jesli tc nie byl wykonany to zawsze wysylany '0'
  //--policzyc crc
  CRC16 tmpcrc;
  tmpcrc.setPolynome( 0x1021); 
  for(int i = 0; i < 5; i++) { tmpcrc.add( htmp[i]); } 
  uint16_t itmpcrc = tmpcrc.getCRC(); 
  char htmpcrc[5];
  sprintf( htmpcrc, "%04X", itmpcrc); 
  //--przygotowac cmd_answer
  hSendBuffer[0] = '{';
  for( int i1 = 0; i1 <= 3; i1++) { hSendBuffer[i1 + 1] = htmpcrc[i1]; } //hSendBuffer[1..4]
  for( int i2 = 0; i2 <= 4; i2++) { hSendBuffer[i2 + 5] = htmp[i2]; }    //hSendBuffer[5..9]
  hSendBuffer[10] = '}'; //11 chars ok
  hSendBuffer[11] = '\0'; 
  //--wyslac cmd_answer
  rs485_print(); //print hSendBuffer
  //- 
  comenabled = true;
}
