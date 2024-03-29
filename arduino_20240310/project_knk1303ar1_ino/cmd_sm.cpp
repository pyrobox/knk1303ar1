/* KNK1303AR1 rev.1 | FirstStep Firing System | https://forumfajerwerki.pl
 * file: cmd_sm.cpp | date: 2023.10.27
 */
/*==========1==========2==========3==========4==========5==========6==========7==========*/
/*
public:
void cmd_SM();
*/
#include <Arduino.h>
#include <EEPROM.h>
#include <CRC16.h>
#include "headerfile.hpp"
#include "unit_var.hpp"

extern boolean comenabled; //(ino)
extern char tabhex[17]; //(unit_var) {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F','\0'}
extern char hSendBuffer[47]; //(out_485)
extern char hmodCheck[17]; //(cmd_tm)
extern boolean bTestFlag; //(cmd_tc)

/*=======================================================================================*/
//cmd_SM:        {1<crc>4<SM>2<adr>2} =10                     //cmd_SM= send_test_module
//cmd_SM_answer: {1<crc>4<SM>2<adr>2<tdp>2<tdm>16}1 =28 x1..4 //answer= send_test_module_answer
//<tdp>=00..33 
//<tdm>=0000000000000000..FFFFFFFFFFFFFFFF <- FFFFx4 = sec:1/2/3/4

void cmd_SM() {
  //--przygotowac dane 
  char htmp[21]; 
  htmp[0] = 'S'; 
  htmp[1] = 'M'; //<cmd>2 = SM
  int a = EEPROM.read( 12); htmp[2] = tabhex[a - 1]; //eep_addr.unt
      a = EEPROM.read( 11); htmp[3] = tabhex[a - 1]; //eep_addr.grp
  htmp[4] = '0'; //<tdp>2 //0..3 liczba pakietow z danymi do wyslania 
  htmp[5] = '0';          //0..3 numer wysylanego pakietu 
  for( int i = 0; i < 16; i++) htmp[6 + i] = hmodCheck[i]; //wyniki z 4 sekcji
  if( bTestFlag == true) {
    bTestFlag = false;
  } else {
    for( int i = 0; i < 16; i++) htmp[6 + i] = '0'; //wyniki z 4 sekcji
  }
  //--policzyc crc
  CRC16 tmpcrc;
  tmpcrc.setPolynome( 0x1021); 
  for( int i = 0; i < 22; i++) { tmpcrc.add( htmp[i]); } 
  uint16_t itmpcrc = tmpcrc.getCRC(); 
  char htmpcrc[5];
  sprintf( htmpcrc, "%04X", itmpcrc); 
  //--przygotowac cmd_answer
  hSendBuffer[0] = '{';
  for( int i1 = 0; i1 <= 3; i1++) { hSendBuffer[i1 + 1] = htmpcrc[i1]; } //hSendBuffer[1..4]
  for( int i2 = 0; i2 <= 23; i2++) { hSendBuffer[i2 + 5] = htmp[i2]; }   //hSendBuffer[5..26]
  hSendBuffer[27] = '}'; //28 chars ok 
  hSendBuffer[28] = '\0'; 
  //--wyslac cmd_answer
  rs485_print(); //print hSendBuffer
  //-- 
  comenabled = true;
}
