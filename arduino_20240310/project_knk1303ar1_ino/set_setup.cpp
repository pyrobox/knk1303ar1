/* KNK1303AR1 rev.1 | FirstStep Firing System | https://forumfajerwerki.pl
 * file: set_setup.cpp | date: 2023.10.27
 */
/*==========1==========2==========3==========4==========5==========6==========7==========*/
/*
public:
void setup_readSetup();
void setup_default();
private:
void setup_sethAdr();
void setup_readAddr(); 
void setup_readArm();
void setup_sethInfo();
*/
#include <Arduino.h>
#include <EEPROM.h>
#include "headerfile.hpp"
#include "unit_var.hpp"

extern char tabdec[11]; //(unit_var) {'0','1','2','3','4','5','6','7','8','9','\0'};
extern char hIdno[10];  //(set_idno) {'S','M','0','1','2','2','0','0','1','\0'};
extern char hUser[10];  //(set_user) {'P','S','O','S','L','V','0','0','1','\0'};
extern char hAddr[7];   //(set_addr) {'5','F','F','3','F','C','\0'};

char hAdr[11] = {'1','6','.','1','6','/','4','.','1','6','\0'}; //format: "16.16/4.13";
char hInfo[5] = {'0','0','0','0','\0'}; //format: 0000..FFFF

byte iAdrG = 1; //addr.grp =1..16
byte iAdrU = 1; //addr.unt =1..16
byte iAdrM = 1; //addr.mod =1..4
byte iAdrS = 1; //addr.sec =1..15 //PB22SM032T2
byte iAdrC = 1; //addr.cha =1/5/9/13 //not used < PB22SM032T2

byte iArmH = 0; //0/1 hard 1=H+
byte iArmS = 0; //0/2 soft 2=S+
byte iArm = 0;  //0..3 0=safe 3=armed

byte iPls = 25;  //standrad pulse //not used
byte iPulse = 0; //0, 100..900 <- <pls>

/*=========== read & set ADDR ===========================================================*/
void setup_sethAdr() { 
  if( iAdrG > 9) { hAdr[0] = '1'; hAdr[1] = tabdec[iAdrG - 10]; } 
    else         { hAdr[0] = '0'; hAdr[1] = tabdec[iAdrG]; }      
  if( iAdrU > 9) { hAdr[3] = '1'; hAdr[4] = tabdec[iAdrU - 10]; } 
    else         { hAdr[3] = '0'; hAdr[4] = tabdec[iAdrU]; }
                                  hAdr[6] = tabdec[iAdrM]; 
  if( iAdrS > 9) { hAdr[8] = '1'; hAdr[9] = tabdec[iAdrS - 10]; }
    else         { hAdr[8] = '0'; hAdr[9] = tabdec[iAdrS]; }
  hAdr[2] = '.'; hAdr[5] = '/'; hAdr[7] = '.'; hAdr[10] = '\0'; //format: 16.16/4.15
}
/*
void setup_sethAdr2() { //ok ale bez zer poprzedzajacych
  String adrtmp = String(iAdrG) + "." + String(iAdrU) + "/" + String(iAdrM) + "." + String(iAdrS); //format: 16.16/4.15
  adrtmp.toCharArray( hAdr, sizeof( adrtmp) + 1);
}
*/
//----------
void setup_readAddr() {
  iAdrG = EEPROM.read( 11); //addr.grp =1..16    //grupa jednostek
  iAdrU = EEPROM.read( 12); //addr.unt =1..16    //jednostka w grupie
  iAdrM = EEPROM.read( 13); //addr.mod =1..4     //modul w jednostce
  iAdrS = EEPROM.read( 14); //addr.sec =1..16    //pierwsza sekcja w module <- KNK1302AR1 =1..13
  //iAdrC = EEPROM.read( 15); //addr.cha =1,5,9,13 //pierwszy kanal w sekcji  <- KNK1302AR1 !!! not used
  setup_sethAdr();
  //inout_readCueEep(); //!!! potrzebne do weryfikacji addr.cue range
}

/*=========== read & set ARM ============================================================*/
void setup_readArm() {
  iArmS = EEPROM.read( 2); //arm.soft = 0/2
}

/*=========== read & set INFO ===========================================================*/
/* function UnitInfo_ReadCfg2(s: String): String; //<- pbm700
begin
if (s = 'F') then Result := '16sec/256cha';
if (s = 'B') then Result := '12sec/192cha';
if (s = '7') then Result := '8sec/128cha';
if (s = '3') then Result := '4sec/64cha';
if (s = '2') then Result := '3sec/48cha';
if (s = '1') then Result := '2sec/32cha'; <- PB22SM032T2 =2 sekcje
if (s = '0') then Result := 'null'; end; */
//----------
void setup_sethInfo() { 
  hInfo[5] = "0000"; //{'0','0','0','0','\0'};
  switch (iAdrM) { //nr ustawionego modulu w jednostce
    case 1: hInfo[0] = '3'; break; //x000
    case 2: hInfo[1] = '3'; break; //0x00
    case 3: hInfo[2] = '3'; break; //00x0
    case 4: hInfo[3] = '3'; break; //000x
  }
}

/*=========== read SETUP ================================================================*/
void setup_readSetup() {
  idno_idnoRead(); //idno# = hIdno[10]={'K','K','0','1','2','3','0','0','1','\0'};
  user_userRead(); //user# = hUser[10]={'P','S','O','K','N','K','0','0','1','\0'};
  addr_addrRead(); //addr# = hAddr[7] ={'5','F','F','3','F','C','\0' };
  //-
  setup_readAddr(); //iAdrG/U/M/S/C
  setup_readArm();  //iArmS
  setup_sethInfo(); //info# = hInfo[5]={'0','0','0','0','\0'};
}

/*========== write CONFIG ===============================================================*/
/* 0/  empty    / 0            <- 
   1/  iArmH    / 0/1          <- arm.hard //!!! not saved in eeprom
   2/  iArmS    / 0/2          <- arm.soft
   3/  iArm     / 0..3         <- arm.h+s //!!! not saved in eeprom
   4/  iPls     / 10..90       <- pulse =100..900msec / standrad=25msec //not used !!! used only by MK
   5/  iFta     / 0/1          <- hard.fta no/yes //not used
   6/  iSam     / 0/1          <- hard.sam no/yes //not used
   7/  iSamMode / 0/1          <- mode.SAM off/on //not used
   8/  empty    / 0            <-   
   9/  empty    / 0            <-
   10/ iReset   / 0/1          <- soft reset flag //not used
*/
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
void setup_default() { //!!! wgrywanie ustawien fabrycznych 
  //-----IDNO#
  String s;
  s = "KK0123001"; s.toCharArray(hIdno, 10); idno_idnoWrite(); delay(5); 
  idno_idnoRead(); Serial.print("idno="); Serial.println( hIdno); //!!! terminal
  //-----USER# 
  s = "PSOKNK001"; s.toCharArray(hUser, 10); user_userWrite(); delay(5); 
  user_userRead(); Serial.print("user="); Serial.println( hUser); //!!! terminal
  //-----ADDR# 
  s = "4420C0";    s.toCharArray(hAddr, 10); addr_addrWrite(); delay(5); //5.3/1.13
  addr_addrRead(); Serial.print("addr="); Serial.println( hAddr); //!!! terminal
  //-----ADDR/OUT
  iAdrG = 1; EEPROM.write( 11, iAdrG); delay( 5); //1..16 
  iAdrU = 1; EEPROM.write( 12, iAdrU); delay( 5); //1..16
  iAdrM = 1; EEPROM.write( 13, iAdrM); delay( 5); //1..4
  iAdrS = 1; EEPROM.write( 14, iAdrS); delay( 5); //1..15
  iAdrC = 1; EEPROM.write( 15, iAdrC); delay( 5); //not used //addr.channel
  /*x*/      EEPROM.write( 16, 4);     delay( 5); //2..5 4=gums
  //iOutS = 2;  EEPROM.write( 17, iOutS); delay( 5); //not used //start section
  //iOutC = 16; EEPROM.write( 18, iOutC); delay( 5); //not used //start channel
  //!!! terminal
  setup_readAddr(); String tmps;
  tmps = "grp=" + String(iAdrG); Serial.println( tmps);
  tmps = "unt=" + String(iAdrU); Serial.println( tmps);
  tmps = "mod=" + String(iAdrM); Serial.println( tmps);
  tmps = "sec=" + String(iAdrS); Serial.println( tmps);
  tmps = "cha=" + String(iAdrC); Serial.println( tmps);
  //-----INFO#
  setup_sethInfo(); Serial.print("info="); Serial.println( hInfo); //!!! terminal
  //-----ARM/PLS/FTA/SAM/RST
  iArmH = 0; EEPROM.write( 1, iArmH);  delay( 5); //0/1
  iArmS = 0; EEPROM.write( 2, iArmS);  delay( 5); //0/2
  iArm = 0;  EEPROM.write( 3, iArm);   delay( 5); //0..3
  //!!! terminal
  setup_readArm(); Serial.print("arm="); Serial.println(iArm);
  //iPls = 5;  EEPROM.write( 4, iPls);   delay( 5); //not used //fire pulse
  //iReset = 0; EEPROM.write( 10, iReset); delay( 5); //0/1    //!!! soft reset modulu
  byte i = EEPROM.read( 16); Serial.println( i);
}
