/* KNK1303AR1 rev.1 | FirstStep Firing System | https://forumfajerwerki.pl
 * file: set_info.cpp | date: 2023.10.27
 */
/*==========1==========2==========3==========4==========5==========6==========7==========*/
/*
public:
void info_runPD();
*/
#include <Arduino.h>
#include <CRC16.h>
#include "headerfile.hpp"
#include "unit_var.hpp"

extern boolean comenabled; //(ino)
extern char tabdec[11]; //(unit_var) {'0','1','2','3','4','5','6','7','8','9','\0'};
extern char hSendBuffer[47]; //(out_485)
extern char hInfo[5];  //(set_setup) {'0','0','0','0','\0'};
extern char hIdno[10]; //(set_idno) {'S','M','0','1','2','2','0','0','1','\0'};
extern char hUser[10]; //(set_user) {'P','S','O','S','L','V','0','0','1','\0'};
extern char hAddr[7];  //(set_addr) {'5','F','F','3','F','C','\0'};
extern byte iArm;      //(set_setup)

char hPwr[7] = { '0', '0', '0', '0', '0', '0', '\0' }; //format: pwr1/pwr2 <- 256256
char hAnt[4] = { '0', '0', '0', '\0'}; //000..100

/*=========== read DATA =================================================================*/
//odczyt DATA z EEPROM i wyslanie do master
/* PD Read_DATA        {1<crc>4<PD>2<ser>9}1=17
   PD Read_DATA_answer {1<crc>4<PD>2<inf>4<pwr>6<ser>9<usr>9<add>6<ant>3<arm>1}1=46+1=47
   <inf> = xxxx <- x=0,1,2,3,7,B,F
   <pwr> = xxxyyy <- xxx=256 / yyy=256
   <ser> = #IDNO = xxxxyynnn
   <usr> = #USER = xxxyyynnn
   <add> = #ADDR = xgumsc <- x=4 / g=0..F / u=0..F / m=0..3 / s=0..F / c=0
   <ant> = xxx <- 000..100 % RF
   <arm> = x <- 0=safe 1=H+ 2=S+ 3=arm
*/

void info_runPD() {
  ads1115_pwrtocmdpd(); //pomiar napiecia akumulatorow, dostajemy aktualny hPwr
  char htmp[41]; //<PD>+<inf>+<pwr>+<ser>+<usr>+<add>+<ant>+<arm>=2+4+6+9+9+6+3+1=40+1
  char htmpcrc[5];
  //data =<PD>2+<ser>9+<add>6
  htmp[0] = 'P'; 
  htmp[1] = 'D';                                 //kopiowanie  <PD>2
  for( int i1 = 0; i1 < 4; i1++) { htmp[i1 + 2]  = hInfo[i1]; } //kopiowanie <inf>4=6
  for( int i2 = 0; i2 < 6; i2++) { htmp[i2 + 6]  = hPwr[i2]; }  //kopiowanie <pwr>6=12
  for( int i3 = 0; i3 < 9; i3++) { htmp[i3 + 12] = hIdno[i3]; } //kopiowanie <ser>9=21
  for( int i4 = 0; i4 < 9; i4++) { htmp[i4 + 21] = hUser[i4]; } //kopiowanie <usr>9=30
  for( int i5 = 0; i5 < 6; i5++) { htmp[i5 + 30] = hAddr[i5]; } //kopiowanie <add>6=36
  for( int i6 = 0; i6 < 3; i6++) { htmp[i6 + 36] = hAnt[i6]; }  //kopiowanie <ant>3=39
  htmp[39] = tabdec[iArm];                                      //kopiowanie <arm>1=40
  htmp[40] = '\0'; //2+4+6+9+9+6+3+1=40+1 pozycja =null
  //crc
  CRC16 tmpcrc;
  tmpcrc.setPolynome( 0x1021); //CRC-CCITT
  for(int i7 = 0; i7 < 40; i7++) { tmpcrc.add( htmp[i7]); } //wczytujemy dane do tmpcrc
  uint16_t itmpcrc = tmpcrc.getCRC(); //kalkulujemy crc
  sprintf( htmpcrc, "%04X", itmpcrc); //przeksztalacamy crc int na crc hex
  //cmd
  hSendBuffer[0] = '{'; //=1
  for( int i8 = 0; i8 < 4;  i8++) { hSendBuffer[i8 + 1] = htmpcrc[i8]; } //<crc> hCmdBuffer[1..4]=4
  for( int i9 = 0; i9 < 40; i9++) { hSendBuffer[i9 + 5] = htmp[i9]; }    //<dat> hCmdBuffer[5..44]=40
  hSendBuffer[45] = '}'; //=1
  hSendBuffer[46] = '\0'; //0=1+46=47
  rs485_print(); //send hSendBuffer
  comenabled = true;
}
