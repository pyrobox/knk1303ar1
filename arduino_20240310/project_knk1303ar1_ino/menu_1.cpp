/* KNK1303AR1 rev.1 | FirstStep Firing System | https://forumfajerwerki.pl
 * file: menu_1.cpp | date: 2023.10.26
 */
/*==========1==========2==========3==========4==========5==========6==========7==========*/
/*
public:
void menu_buttonMenu();
void menu_buttonClear();
void menu_buttonLink();
private:
void menu_lcdMenu1( int i);
void menu_menuIn();
void menu_menuOut();
void menu_clearLcd();
void menu_linkTest();
*/
#include <Arduino.h>
#include <EEPROM.h>
#include <CRC16.h>
#include "headerfile.hpp"
#include "unit_var.hpp"

#include "LiquidCrystal_PCF8574.h"
extern LiquidCrystal_PCF8574 lcd;

extern boolean comenabled; //(ino)
extern char tabhex[17]; //(unit_var) {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F','\0'}
extern char hSendBuffer[47]; //(out_485)
extern char hIdno[10];  //(set_idno) {'K','K','0','1','2','3','0','0','1','\0'}
extern char hAdr[11]; //(set_setup)
extern byte iAdrG; //(set_setup) addr.grp
extern byte iAdrU; //(set_setup) addr.unt
extern boolean bXmStart; //(cmd_xm) XM_reset_start_flag   
extern int iXmNext; //(cmd_xm) XM_reset_counter 

boolean bFlagMenu  = false; 
int iItemMenu  = 0; 
boolean bFlagMenu2 = false; 
int iItemMenu2 = 0; 
byte iLcdLine2 = 0; //=1 wylacza w trybie menu wyswietlanie info arm

extern time_YXstart(); //(ino)

/* button_MENU | button_CLEAR/NEXT | button_LINK/SET */ //uklad przyciskow
/*=======================================================================================*/
void menu_LcdMenu1( int i) {
  switch ( i) {
    case 1: lcd.setCursor( 0, 0); lcd.print( F( "                ")); break; //line_1 clear
    case 2: lcd.setCursor( 0, 1); lcd.print( F( "                ")); break; //line_2 clear
    case 3: lcd.setCursor( 0, 0); lcd.print( F( "MENU:           ")); break;
    case 4: lcd.setCursor( 0, 1); lcd.print( F( "      NEXT | SET")); break;
    case 5: lcd.setCursor( 0, 0); lcd.print( F( "ADDR:           ")); break;
//    case 6: lcd.setCursor( 6, 1); lcd.print( F( "LINK TEST "));       break;
//    case 7: lcd.setCursor( 6, 1); lcd.print( F( "LINK OK   "));       break;
//    case 8: lcd.setCursor( 6, 1); lcd.print( F( "LINK ERROR"));       break;
  }
}

/*==============================================================================MENU=====*/
void menu_menuIn() { //menu open
  comenabled = false;
  menu_LcdMenu1( 3); //"MENU:           " //line_1
  menu_LcdMenu1( 4); //"      NEXT | SET" //line_2
}
/*---------------------------------------------------------------------------------------*/
void menu_menuOut() { //menu close
  menu_LcdMenu1( 5);                              //"ADDR:           " //line_1
  lcd.setCursor( 6, 0); lcd.print( String(hAdr)); //"      16.16/4.13" //line_1
  menu_LcdMenu1( 2);                              //"                " //line_2
  comenabled = true; //rs485 open
}
/*---------------------------------------------------------------------------------------*/
void menu_buttonMenu() { //button_MENU
  if( bFlagMenu == false) { iLcdLine2 = 1; bFlagMenu = true;  iItemMenu = 0; menu_menuIn(); }
  else                    { iLcdLine2 = 0; bFlagMenu = false; iItemMenu = 0; menu_menuOut(); }
}

/*========================================================================CLEAR/NEXT=====*/
void menu_clearLcd() { 
  comenabled = false;
  menu_LcdMenu1( 2); //"                " //line_2
  myDelay( 500);
  //-----batt
  ads1115_pwrtoclear(); //pomiar i prezentacja napiecia akumulatorow //"BATT 25.6/25.6 V" //line_2
  //-----
  myDelay( 2000);
  menu_LcdMenu1( 2); //"                " //line_2
  iXmNext = 0;      //reset xm cmd counter
  bXmStart = false; //reset xm start flag
  iLcdLine2 = 0;
  arm_Check();
  comenabled = true;
}
/*---------------------------------------------------------------------------------------*/
void menu_buttonClear() { //button_CLEAR/NEXT
  if( bFlagMenu == false) { iLcdLine2 = 1; menu_clearLcd(); }
  else { if( bFlagMenu2 == false) { 
           iItemMenu++; 
           if( iItemMenu == 10) { iItemMenu = 1; }
           menu_nextitem();           
         } else { menu_nextitem2(); } 
  }
}

/*==========================================================================TEST/SET=====*/
//cmd_YX: {1<crc>4<YX>2<adr>2}1=10       //cmd_YX= slave_master_link_test
//answer= {1<crc>4<YX>2<adr>2}1=10       //answer= SMLT_answer
//cmd_YX: {1<crc>4<YX>2<adr>2<ser>9}1=19 //cmd_YX= slave_master_link_test_pbms2019
//answer= {1<crc>4<YX>2<adr>2<ser>9}1=19 //answer= SMLT_answer_pbms2019

void menu_linkTest() { 
  //menu_LcdMenu1( 6); //"LINK TEST       " //line_2
  lcd.setCursor( 6, 1); lcd.print( F( "LINK TEST ")); //line_2
  comenabled = false;
  //---przygotowac dane
  char htmp[5]; 
  htmp[0] = 'Y'; htmp[1] = 'X'; //<cmd>2 = YX
  byte a = EEPROM.read( 12); htmp[2] = tabhex[a - 1]; //eep_addr.unt_U
       a = EEPROM.read( 11); htmp[3] = tabhex[a - 1]; //eep_addr.grp_G
  for( int i1 = 0; i1 < 9; i1++) { htmp[i1 + 4] = hIdno[i1]; } //kopiowanie idno <ser>9 
  htmp[13] = '\0'; //13chars+1 ok
  //---policzyc crc
  CRC16 tmpcrc;
  tmpcrc.setPolynome( 0x1021); 
  for(int i = 0; i < 13; i++) { tmpcrc.add( htmp[i]); } 
  uint16_t itmpcrc = tmpcrc.getCRC(); 
  char htmpcrc[5];
  sprintf( htmpcrc, "%04X", itmpcrc); 
  //---przygotowac cmd_YX
  hSendBuffer[0] = '{';
  for( int i1 = 0; i1 <= 3; i1++) { hSendBuffer[i1 + 1] = htmpcrc[i1]; } //hSendBuffer[1..4]
  for( int i2 = 0; i2 <= 12; i2++) { hSendBuffer[i2 + 5] = htmp[i2]; }   //hSendBuffer[5..17]
  hSendBuffer[18] = '}';  
  hSendBuffer[19] = '\0'; 
  //---wyslac cmd_YX
  time_YXstart(); //timerB1start(); //uruchomic timer 500msec 
  comenabled = true;
  rs485_print(); //print hSendBuffer
  //odpowiedz przychodzi to cmd_YX()          //"      LINK OK   " //line_2  
  //odpowiedz nie przychodzi to time_YXstop() //"      LINK ERROR" //line_2
}
/*---------------------------------------------------------------------------------------*/
void menu_buttonLink() { 
  if( bFlagMenu == false) { iLcdLine2 = 1; menu_linkTest(); }
  else { if( bFlagMenu2 == false) { menu_setcfg(); }  
         else                     { menu_setcfg2(); } 
  }
}
