/* KNK1303AR1 rev.1 | FirstStep Firing System | https://forumfajerwerki.pl
 * file: set_addr.cpp | date: 2023.10.27
 */
/*==========1==========2==========3==========4==========5==========6==========7==========*/
/*
public:
void addr_runPA();
void addr_runPZ();
private:
void addr_addrRead();
void addr_addrWrite();
*/
#include <Arduino.h>
#include <EEPROM.h>
#include <CRC16.h>
#include "headerfile.hpp"
#include "unit_var.hpp"

extern char tabhex[17]; //(unit_var) {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F','\0'};
extern char hDataBuffer[47]; //(ino)
extern char hSendBuffer[47]; //(out_485)
extern char hIdno[10]; //(set_idno) {'S','M','0','1','2','2','0','0','1','\0'};

extern boolean comenabled; 
extern byte iAdrG; //(set_setup) 1..16
extern byte iAdrU; //(set_setup) 1..16
extern byte iAdrM; //(set_setup) 1..4
extern byte iAdrS; //(set_setup) 1..15
extern byte iAdrC; //(set_setup) 1/5/9/13

char hAddr[7]  = {'4','F','F','3','C','0','\0'}; //format: xgumsc

/*=========== EEPROM read & write ADDR ==================================================*/
/* 11/ iAdrG / 1..16           <- addr.grp 
   12/ iAdrU / 1..16           <- addr.unt
   13/ iAdrM / 1..4            <- addr.mod
   14/ iAdrS / 1..13           <- addr.sec //max 13 !!! because we have 4 sections
   15/ iAdrC / 1/5/9/13        <- addr.cha //not used //!!!only if 1 section, min. 1row=4cha 
   16/ x     / 2..5            <- addr.pcs  
   17/ iOutS / 1/2/3/4/8/12/16 <- out.sec  //number of sections, + 0=NC
   18/ iOutC / 4/8/12/16       <- out.cha  //not used //!!!only if 1 section, min. 1row=4cha
   19/ empty / 0                  
   20/ empty / 0                  
*/
/*---------------------------------------------------------------------------------------*/
void addr_addrRead() { //odczyt dec z eeprom, zamiana dec > hex, zapis hex do hAddr
  byte i;              //hAddr={'5','F','F','3','F','C','\0'};
  i = EEPROM.read( 16);     hAddr[0] = '4';       //addr.pcs //!!! KNK1302AR1 =4 <- G+U+M+S
  i = EEPROM.read( 11) - 1; hAddr[1] = tabhex[i]; //addr.grp
  i = EEPROM.read( 12) - 1; hAddr[2] = tabhex[i]; //addr.unt
  i = EEPROM.read( 13) - 1; hAddr[3] = tabhex[i]; //addr.mod
  i = EEPROM.read( 14) - 1; hAddr[4] = tabhex[i]; //addr.sec
  i = EEPROM.read( 15) - 1; hAddr[5] = '0';       //addr.cha //!!! KNK1302AR1 not used
                            hAddr[6] = '\0'; 
}
/*---------------------------------------------------------------------------------------*/
void addr_addrWrite() { //zamiana hex > dec, zapis dec do eeprom
  byte x = hAddr[0];               EEPROM.write( 16, '4');   //!!! KNK1302AR1 =4 <- G+U+M+S
  iAdrG  = hex2dec( hAddr[1]) + 1; EEPROM.write( 11, iAdrG); 
  iAdrU  = hex2dec( hAddr[2]) + 1; EEPROM.write( 12, iAdrU);  
  iAdrM  = hex2dec( hAddr[3]) + 1; EEPROM.write( 13, iAdrM);
  iAdrS  = hex2dec( hAddr[4]) + 1; EEPROM.write( 14, iAdrS);  
  iAdrC  = hex2dec( hAddr[5]) + 1; EEPROM.write( 15, '0');   //!!! KNK1302AR1  not used
}

/*=========== write ADDR ================================================================*/
//zapis #<add> hex jako #ADDR dec do EEPROM
/* PA Write_ADDR       {1<crc>4<PA>2<ser>9<add>6}1=23
   PA Write_ADDR_check {1<crc>4<PA>2<ser>9<add>6}1=23
*/
void addr_runPA() { //cmd_PA > Write_ADDR_check {1<crc>4<PA>2<ser>9<add>6}1=23
  for( int i1 = 0; i1 < 6; i1++) { hAddr[i1] = hDataBuffer[i1 + 16]; } //kopiowanie <add>6 do ADDR
  addr_addrWrite(); //zapis ADDR (hAddr[7]) do EEPROM jako iAdrG/U/M/S/C
  for( int i2 = 0; i2< 23; i2++) { hSendBuffer[i2] = hDataBuffer[i2]; } //kopiowanie calego cmd_PA=23
  rs485_print(); //odeslanie odebranego cmd_PA jako check
  comenabled = true;
}

/*=========== read ADDR =================================================================*/
//odczyt #ADDR (hAddr[7]) dec z EEPROM i wyslanie #<add> hex do master
/* PZ Read_ADDR        {1<crc>4<PZ>2<ser>9}1=17
   PZ Read_ADDR_answer {1<crc>4<PZ>2<ser>9<add>6}1=23
*/
void addr_runPZ() { //cmd_PZ > Read_ADDR_answer {1<crc>4<PZ>2<ser>9<add>6}1=23
  char htmp[18]; //<PZ>+<ser>+<add>=2+9+6=17+1
  char htmpcrc[5];
  //data =<PZ>2+<ser>9+<add>6
  htmp[0] = 'P'; 
  htmp[1] = 'Z';
  for( int i1 = 0; i1 < 9; i1++) { htmp[i1 + 2]  = hIdno[i1]; } //kopiowanie <ser>9
  for( int i2 = 0; i2 < 6; i2++) { htmp[i2 + 11] = hAddr[i2]; } //kopiowanie <add>6
  htmp[17] = '\0'; //2+9+6=17+1 pozycja =null
  //crc
  CRC16 tmpcrc;
  tmpcrc.setPolynome( 0x1021); //CRC-CCITT
  for(int i3 = 0; i3 < 17; i3++) { tmpcrc.add( htmp[i3]); } //wczytujemy dane do tmpcrc
  uint16_t itmpcrc = tmpcrc.getCRC(); //kalkulujemy crc
  sprintf( htmpcrc, "%04X", itmpcrc); //przeksztalacamy crc int na crc hex
  //cmd
  hSendBuffer[0] = '{'; //=1
  for( int i4 = 0; i4 < 4;  i4++) { hSendBuffer[i4 + 1] = htmpcrc[i4]; } //<crc> hCmdBuffer[1..4]=4
  for( int i5 = 0; i5 < 17; i5++) { hSendBuffer[i5 + 5] = htmp[i5]; }    //<dat> hCmdBuffer[5..21]=17
  hSendBuffer[22] = '}'; //=1
  hSendBuffer[23] = '\0'; //0=1+23=24
  rs485_print(); //send hSendBuffer
  comenabled = true;
}
