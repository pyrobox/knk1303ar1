/* KNK1303AR1 rev.1 | FirstStep Firing System | https://forumfajerwerki.pl
 * file: set_user.cpp | date: 2023.10.27
 */
/*==========1==========2==========3==========4==========5==========6==========7==========*/
/*
public:
void user_runPU();
void user_runPY();
private:
void user_userRead();
void user_userWrite();
*/
#include <Arduino.h>
#include <EEPROM.h>
#include <CRC16.h>
#include "headerfile.hpp"
#include "unit_var.hpp"

extern boolean comenabled; //(ino)
extern char hDataBuffer[47]; //ino
extern char hSendBuffer[47]; //(out_485)
extern char hIdno[10]; //(set_idno) {'S','M','0','1','2','2','0','0','1','\0'};

char hUser[10] = {'P','S','O','K','N','K','0','0','1','\0'}; //PSOKNK001

/*=========== EEPROM read & write USER ==================================================*/
/* 31/ user_firm1  / A..Z <- np. PSOSLV001 firm=PSO tool=SLV idn#=001
   32/ user_firm2  / A..Z
   33/ user_firm3  / A..Z
   34/ user_tool1  / A..Z
   35/ user_tool2  / A..Z
   36/ user_tool3  / A..Z
   37/ user_idn#1  / 0..9
   38/ user_idn#2  / 0..9
   39/ user_idn#3  / 0..9
   40/ pusta       / 0
*/
/*---------------------------------------------------------------------------------------*/
void user_userRead() { 
hUser[0] = EEPROM.read( 31); //P
hUser[1] = EEPROM.read( 32); //S
hUser[2] = EEPROM.read( 33); //O
hUser[3] = EEPROM.read( 34); //S
hUser[4] = EEPROM.read( 35); //L
hUser[5] = EEPROM.read( 36); //V
hUser[6] = EEPROM.read( 37); //0
hUser[7] = EEPROM.read( 38); //0
hUser[8] = EEPROM.read( 39); //1
hUser[9] = '\0'; 
}
/*---------------------------------------------------------------------------------------*/
void user_userWrite() { 
EEPROM.write( 31, hUser[0]); //P 
EEPROM.write( 32, hUser[1]); //S 
EEPROM.write( 33, hUser[2]); //O 
EEPROM.write( 34, hUser[3]); //S 
EEPROM.write( 35, hUser[4]); //L 
EEPROM.write( 36, hUser[5]); //V 
EEPROM.write( 37, hUser[6]); //0 
EEPROM.write( 38, hUser[7]); //0 
EEPROM.write( 39, hUser[8]); //1 
}

/*=========== write IDNO ================================================================*/
//zapis #<usr> str jako #USER str do EEPROM
//<usr>9 > UserID_3/ToolID_3/NumID_3
//CL01 -> PSOSLV001
/* cmd_PU > Write_USER       {1<crc>4<PU>2<ser>9<usr>9}=26
   cmd_PU > Write_USER_check {1<crc>4<PU>2<ser>9<usr>9}=26
*/
void user_runPU() { //cmd_PU > Write_USER_check {1<crc>4<PU>2<ser>9<usr>9}=26
  char htmp[10]; //<ser>9+1
  //check <ser>/IDNO
  for( int i1 = 0; i1 < 9; i1++) { htmp[i1] = hDataBuffer[i1 + 7]; } // kopiowanie <ser>9 do htmp
  htmp[9] = '\0'; //10 pozycja = null
  if( strcmp(htmp, hIdno) == 0) { //zgodne <ser>=idno
    //write <usr>/USER
    for( int i2 = 0; i2 < 9; i2++) { hUser[i2] = hDataBuffer[i2 + 16]; } // kopiowanie <usr>9 do USER
    user_userWrite(); //zapis USER do EEPROM
    //send answer/check
    for( int i3 = 0; i3 < 27; i3++) { hSendBuffer[i3] = hDataBuffer[i3]; } //kopiowanie calego odebranego cmd_PU=26+1
    rs485_print(); //odeslanie odebranego cmd_PU jako check
  }
  comenabled = true;
}

/*=========== read IDNO =================================================================*/
//odczyt #USER ser z EEPROM i wyslanie #<usr> ser do master
/* cmd_PY > Read_USER        {1<crc>4<PY>2<ser>9}1=17
   cmd_PY > Read_USER_answer {1<crc>4<PY>2<ser>9<usr>9}1=26
*/
void user_runPY() { //cmd_PY > Read_USER_answer {1<crc>4<PY>2<ser>9<usr>9}1=26
  char htmp[21]; //<PY>+<ser>+<usr>=2+9+9=20+1
  char htmpcrc[5];
  //check <ser>/IDNO
  for( int i1 = 0; i1 < 9; i1++) { htmp[i1] = hDataBuffer[i1 + 7]; } //kopiowanie <ser>9 do htmp
  htmp[9] = '\0'; //9+1 pozycja =null
  if( strcmp(htmp, hIdno) == 0) { //zgodne <ser>=idno
    //data
    htmp[0] = 'P'; 
    htmp[1] = 'Y';
    for( int i2 = 0; i2 < 9; i2++) { htmp[i2 + 2]  = hIdno[i2]; } //kopiowanie IDNO=9
    for( int i3 = 0; i3 < 9; i3++) { htmp[i3 + 11] = hUser[i3]; } //kopiowanie USER=9
    htmp[20] = '\0'; //2+9+9=20+1 pozycja =null
    //crc
    CRC16 tmpcrc;
    tmpcrc.setPolynome( 0x1021); //CRC-CCITT
    for(int i4 = 0; i4 < 20; i4++) { tmpcrc.add( htmp[i4]); } //wczytujemy dane=20 do tmpcrc
    uint16_t itmpcrc = tmpcrc.getCRC(); //kalkulujemy crc
    sprintf( htmpcrc, "%04X", itmpcrc); //przeksztalacamy crc int na crc hex
    //cmd
    hSendBuffer[0] = '{'; //=1
    for( int i5 = 0; i5 < 4;  i5++) { hSendBuffer[i5 + 1] = htmpcrc[i5]; } //<crc> hCmdBuffer[1..4]=4
    for( int i6 = 0; i6 < 20; i6++) { hSendBuffer[i6 + 5] = htmp[i6]; } //<data> hCmdBuffer[5..24]=20
    hSendBuffer[25] = '}'; //=1
    hSendBuffer[26] = '\0'; 
    rs485_print(); //send hSendBuffer
  }
  comenabled = true;
}
