/* KNK1303AR1 rev.1 | FirstStep Firing System | https://forumfajerwerki.pl
 * file: set_idno.cpp | date: 2023.10.27
 */
/*==========1==========2==========3==========4==========5==========6==========7==========*/
/*
public:
void idno_runPS();
void idno_runPX();
private:
void idno_idnoRead();
void idno_idnoWrite();
*/
#include <Arduino.h>
#include <EEPROM.h>
#include <CRC16.h>
#include "headerfile.hpp"
#include "unit_var.hpp"

extern boolean comenabled; //(ino)
extern char hDataBuffer[47]; //(ino)
extern char hSendBuffer[47]; //(out_485)

char hIdno[10] = {'K','K','0','1','2','3','0','0','1','\0'}; //SM01=PB22SM032T2,KK01=KNK1302AR1REV1

/*=========== EEPROM read & write IDNO ==================================================*/
/* 21/ idno_type1  / A..Z <- np. SM0122001 type=SM01 year=22 ser#=001 <- SM01=PB22SM032T2
   22/ idno_type2  / A..Z                                             <- SM02=PB22SM032T2NE
   23/ idno_type3  / 0..9
   24/ idno_type4  / 0..9
   25/ idno_year1  / 0..9
   26/ idno_year2  / 0..9
   27/ idno_ser#1  / 0..9
   28/ idno_ser#2  / 0..9
   29/ idno_ser#3  / 0..9
   30/ pusta       / 0
*/
/*---------------------------------------------------------------------------------------*/
void idno_idnoRead() { 
hIdno[0] = EEPROM.read( 21); //S
hIdno[1] = EEPROM.read( 22); //M
hIdno[2] = EEPROM.read( 23); //0
hIdno[3] = EEPROM.read( 24); //1
hIdno[4] = EEPROM.read( 25); //2
hIdno[5] = EEPROM.read( 26); //2
hIdno[6] = EEPROM.read( 27); //0
hIdno[7] = EEPROM.read( 28); //0
hIdno[8] = EEPROM.read( 29); //1
hIdno[9] = '\0'; 
}
/*---------------------------------------------------------------------------------------*/
void idno_idnoWrite() { 
EEPROM.write( 21, hIdno[0]); //S 
EEPROM.write( 22, hIdno[1]); //M 
EEPROM.write( 23, hIdno[2]); //0 
EEPROM.write( 24, hIdno[3]); //1 
EEPROM.write( 25, hIdno[4]); //2 
EEPROM.write( 26, hIdno[5]); //2 
EEPROM.write( 27, hIdno[6]); //0 
EEPROM.write( 28, hIdno[7]); //0 
EEPROM.write( 29, hIdno[8]); //1 
}

/*=========== write IDNO ================================================================*/
//zapis #<ser> str jako #IDNO str do EEPROM
//<ser>9 = DeviceID_2/TypeID_2/Year_2/NumID_3
//PB22MK1 -> MK01 | PB12RSLPT1R3 -> CL01 | PB22SM032T2 -> SM01 
/* SERVICE MODE ONLY:
   cmd_PS > Write_IDNO       {1<crc>4<PS>2<ser>9}1=17
   cmd_PS > Write_IDNO_check {1<crc>4<PS>2<ser>9}1=17
*/
void idno_runPS() { //cmd_PS > Write_IDNO_check {1<crc>4<PS>2<ser>9}1=17
  for( int i1 = 0; i1 < 9; i1++) { hIdno[i1] = hDataBuffer[i1 + 7]; } //kopiowanie <ser>9 do IDNO
  idno_idnoWrite(); //zapis IDNO do EEPROM
  for( int i2 = 0; i2< 17; i2++) { hSendBuffer[i2] = hDataBuffer[i2]; } //kopiowanie calego cmd_PS=17
  rs485_print(); //odeslanie odebranego cmd_PS jako check
  comenabled = true;
}

/*=========== read IDNO =================================================================*/
//odczyt #IDNO str z EEPROM i wyslanie #<ser> str do master
/* NORMAL MODE (!!! only one unit at a time in PBM_NET !!!):
   cmd_PX > Read_IDNO        {1<crc>4<PX>2}1=8 ---> const={D542PX}=8
   cmd_PX > Read_IDNO_answer {1<crc>4<PX>2<ser>9}1=17
*/
void idno_runPX() { //cmd_PX > Read_IDNO_answer {1<crc>4<PX>2<ser>9}1=17
  char htmp[12]; //<PX>+<ser>=2+9=11+1
  char htmpcrc[5];
  //data =<PX>2+<ser>9
  htmp[0] = 'P'; 
  htmp[1] = 'X';
  for( int i1 = 0; i1 < 9; i1++) { htmp[i1 + 2] = hIdno[i1]; } //kopiowanie <ser>9 
  htmp[11] = '\0'; //2+9=11+1 pozycja =null
  //crc
  CRC16 tmpcrc;
  tmpcrc.setPolynome( 0x1021); //CRC-CCITT
  for(int i2 = 0; i2 < 11; i2++) { tmpcrc.add( htmp[i2]); } //wczytujemy dane do tmpcrc
  uint16_t itmpcrc = tmpcrc.getCRC(); //kalkulujemy crc
  sprintf( htmpcrc, "%04X", itmpcrc); //przeksztalacamy crc int na crc hex
  //cmd
  hSendBuffer[0] = '{'; //=1
  for( int i3 = 0; i3 < 4;  i3++) { hSendBuffer[i3 + 1] = htmpcrc[i3]; } //<crc> hCmdBuffer[1..4]=4
  for( int i4 = 0; i4 < 11; i4++) { hSendBuffer[i4 + 5] = htmp[i4]; } //<data> hCmdBuffer[5..15]=11
  hSendBuffer[16] = '}'; //=1
  hSendBuffer[17] = '\0';
  rs485_print(); //send hSendBuffer
  comenabled = true;
}
