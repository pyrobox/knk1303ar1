/* KNK1303AR1 rev.1 | FirstStep Firing System | https://forumfajerwerki.pl
 * file: menu_2.cpp | date: 2023.10.26
 */
/*==========1==========2==========3==========4==========5==========6==========7==========*/
/*
public:
void menu_nextitem();
void menu_nextitem2();
void menu_setcfg();
void menu_setcfg2();
private:
void menu_LcdMenu2( int i);
void menu_setGrp();
void menu_setUnt();
void menu_setMod();
void menu_setSec();
void menu_reset();
void menu_disarm();
void menu_info();
void menu_testStart();
void menu_testShow();
void menu_disarm2()
void menu_LcdInfo1( int i);
void menu_info2_wait();
void menu_info2();
void menu_reset2();
void menu_testStart2();
void menu_testShow2();
void menu_setGrp2();
void menu_setUnt2();
void menu_setMod2();
void menu_setSec2();
void menu_nextitem3();
void menu_eeprom_write();
*/
#include <Arduino.h>
#include <EEPROM.h>
#include "headerfile.hpp"
#include "unit_var.hpp"

#include "LiquidCrystal_PCF8574.h"
extern LiquidCrystal_PCF8574 lcd;

extern char tabdec[11]; //(unit_var) {'0','1','2','3','4','5','6','7','8','9','\0'}
extern char hIdno[10];  //(set_idno) {'K','K','0','1','2','3','0','0','1','\0'} //=KNK1303AR1
extern char hUser[10];  //(set_user) {'P','S','O','K','N','K','0','0','1','\0'} //PSO KNK_module #1
extern boolean bFlagMenu; //(menu1)
extern int iItemMenu; //(menu1)
extern boolean bFlagMenu2; //(menu1)
extern int iItemMenu2; //(menu1)
extern byte iAdrG; //(set_setup) addr.grp
extern byte iAdrU; //(set_setup) addr.unt
extern byte iAdrM; //(set_setup) addr.mod
extern byte iAdrS; //(set_setup) addr.sec
extern byte iArmS; //(set_setup) arm.soft

/*=======================================================================================*/
void menu_LcdMenu2( int i) {
  switch ( i) {
    case 1:  lcd.setCursor( 0, 0); lcd.print( F( "SELECT:         ")); break;
    case 2:  lcd.setCursor( 0, 0); lcd.print( F( "SET:            ")); break;
    case 3:  lcd.setCursor( 0, 1); lcd.print( F( "ADDR-GROUP:     ")); break;
    case 4:  lcd.setCursor( 0, 1); lcd.print( F( "ADDR-UNIT:      ")); break;
    case 5:  lcd.setCursor( 0, 1); lcd.print( F( "ADDR-MODULE:    ")); break;
    case 6:  lcd.setCursor( 0, 1); lcd.print( F( "ADDR-SECTION:   ")); break;
    case 7:  lcd.setCursor( 0, 1); lcd.print( F( "ADDRESS RESET   ")); break;
    case 8:  lcd.setCursor( 0, 1); lcd.print( F( "MODULE DISARM   ")); break;
    case 9:  lcd.setCursor( 0, 1); lcd.print( F( "KNK1303AR1 /INFO")); break; //"PB23SN064R3 INFO"
    case 10: lcd.setCursor( 0, 1); lcd.print( F( "EM TEST   /START")); break;
    case 11: lcd.setCursor( 0, 1); lcd.print( F( "EM TEST    /SHOW")); break;
  }
}    
/*---------------------------------------------------------------------------------------*/
void menu_setGrp() {
  menu_LcdMenu2( 3); //"ADDR-GROUP:     " //line 2
  lcd.setCursor( 12, 1); lcd.print( iAdrG, DEC); //1..16
}                    //"ADDR-GROUP: 16  " //line 2
//----------
void menu_setUnt() { 
  menu_LcdMenu2( 4); //"ADDR-UNIT:      " //line 2
  lcd.setCursor( 11, 1); lcd.print( iAdrU, DEC); //1..16 
}                    //"ADDR-UNIT: 16   " //line 2
//----------
void menu_setMod() { 
  menu_LcdMenu2( 5); //"ADDR-MODULE:    " //line 2
  lcd.setCursor( 13, 1); lcd.print( iAdrM, DEC); //1..4 
}                    //"ADDR-MODULE: 4  " //line 2
//----------
void menu_setSec() { 
  menu_LcdMenu2( 6); //"ADDR_SECTION:   " //line 2
  lcd.setCursor( 14, 1); lcd.print( iAdrS, DEC); //1..13
}                    //"ADDR_SECTION: 13" //line 2
//----------
void menu_reset() { 
  menu_LcdMenu2( 7); //"ADDRESS RESET   " //line 2
}
//----------
void menu_disarm() { 
  menu_LcdMenu2( 8); //"MODULE DISARM   " //line 2
}
//----------
void menu_info() { 
  menu_LcdMenu2( 9); //"KNK1302AR1      " //line 2
}
//----------
void menu_testStart() { 
  menu_LcdMenu2( 10); //"EM TEST / START " //line 2
}
//----------
void menu_testShow() { 
  menu_LcdMenu2( 11); //"EM TEST / SHOW  " //line 2
}

/*---------------------------------------------------------------------------------------*/
void menu_nextitem() { 
  menu_LcdMenu2( 1); //"SELECT:         " //line 1
  switch( iItemMenu) {
    case 1: menu_testShow();  break;
    case 2: menu_testStart(); break;
    case 3: menu_setGrp();    break;
    case 4: menu_setUnt();    break;
    case 5: menu_setMod();    break;
    case 6: menu_setSec();    break;
    case 7: menu_reset();     break;
    case 8: menu_disarm();    break;
    case 9: menu_info();      break;
  }
}

/*=======================================================================================*/
void menu_disarm2() {
  iArmS = 0;                         
  lcd.setCursor( 0, 1); lcd.print( F( "    DISARMED    ")); myDelay( 1000); //line 2
  menu_setcfg2(); //zapis i reset!
}
//----------
void menu_LcdInfo1( int i) {
  switch ( i) {
    case 1: lcd.setCursor( 0, 0); lcd.print( F( "FIRSTSTEP       ")); break; //"PYROBOX(R)      "
    case 2: lcd.setCursor( 0, 1); lcd.print( F( "FIRING SYSTEM   ")); break;
    case 3: lcd.setCursor( 0, 0); lcd.print( F( "SLAVE MODULE    ")); break;
    case 4: lcd.setCursor( 0, 1); lcd.print( F( "KNK1303AR1 REV.1")); break; //"PB23SN064R1     "
    case 5: lcd.setCursor( 0, 0); lcd.print( F( "OSHW LICENCE    ")); break;
    case 6: lcd.setCursor( 0, 1); lcd.print( F( "SOFT#: 1.231004 ")); break;
    case 7: lcd.setCursor( 0, 0); lcd.print( F( "IDNO#:          ")); break;
    case 8: lcd.setCursor( 0, 1); lcd.print( F( "USER#:          ")); break;
  }
}
//-----
void menu_info2_wait() { myDelay( 3000); lcd.clear(); myDelay( 1000); }
//-----
void menu_info2() {
  menu_LcdInfo1( 1); //"FIRSTSTEP(TM)   " //line 1
  menu_LcdInfo1( 2); //"FIRING SYSTEM   " //line 2
  menu_info2_wait();
  //-
  menu_LcdInfo1( 3); //"SLAVE MODULE    " //line 1
  menu_LcdInfo1( 4); //"KNK1303AR1 REV.1" //line 2
  menu_info2_wait();
  //-
  menu_LcdInfo1( 5); //"OSHW LICENCE    " //line 2
  menu_LcdInfo1( 6); //"SOFT#: 1.231004 " //line 1
  menu_info2_wait();
  //-
  menu_LcdInfo1( 7); //"IDNO#: KK0123001" //line 1
  lcd.setCursor( 7, 0); lcd.print( String(hIdno));            
  menu_LcdInfo1( 8); //"USER#: PSOKNK001" //line 2
  lcd.setCursor( 7, 1); lcd.print( String(hUser));            
  menu_info2_wait();
  //-
  lcd.clear(); myDelay( 1000); menu_buttonMenu();
}
//----------
void menu_reset2() {
  iAdrG = 1; //addr.grp_1..16 
  iAdrU = 1; //addr.unt_1..16
  iAdrM = 1; //addr.mod_1..4
  iAdrS = 1; //addr.sec_1..13 
  menu_setcfg2(); //zapis ustawien i restart modulu
}
//----------
void menu_testStart2() { menu_modTest(); }
//----------
void menu_testShow2() { menu_modShow(); }

/*---------------------------------------------------------------------------------------*/
void menu_setcfg() {
  menu_LcdMenu2( 2); //"SET:            " //line 1
  switch( iItemMenu) {
    case 1: menu_testShow2();                      break;
    case 2: menu_testStart2();                     break;
    case 3: iItemMenu2 = iAdrG; bFlagMenu2 = true; break;
    case 4: iItemMenu2 = iAdrU; bFlagMenu2 = true; break;
    case 5: iItemMenu2 = iAdrM; bFlagMenu2 = true; break;
    case 6: iItemMenu2 = iAdrS; bFlagMenu2 = true; break;
    case 7: menu_reset2();                         break;
    case 8: menu_disarm2();                        break;
    case 9: menu_info2();                          break;
  }
}

/*=======================================================================================*/
void menu_setGrp2() { iAdrG = iItemMenu2; menu_setGrp(); } //addr.grp_1..16
void menu_setUnt2() { iAdrU = iItemMenu2; menu_setUnt(); } //addr.unt_1..16
void menu_setMod2() { iAdrM = iItemMenu2; menu_setMod(); } //addr.mod_1..4
void menu_setSec2() { iAdrS = iItemMenu2; menu_setSec(); } //addr.sec_1..15

//----------
void menu_nextitem3() {
  switch( iItemMenu) {
    case 1:                 break;
    case 2:                 break;
    case 3: menu_setGrp2(); break;
    case 4: menu_setUnt2(); break;
    case 5: menu_setMod2(); break;
    case 6: menu_setSec2(); break;
    case 7:                 break;
    case 8:                 break;
    case 9:                 break;
  }
}
//----------
void menu_nextitem2() {
  menu_LcdMenu2( 2); //line 1 
  switch( iItemMenu) {
    case 1: break;   
    case 2: break;   
    case 3: iItemMenu2++; if( iItemMenu2 == 17) { iItemMenu2 = 1; } 
            menu_nextitem3(); 
            break; //addr.group_1..16
    case 4: iItemMenu2++; if( iItemMenu2 == 17) { iItemMenu2 = 1; } 
            menu_nextitem3(); 
            break; //addr.unit_1..16
    case 5: iItemMenu2++; if( iItemMenu2 == 5)  { iItemMenu2 = 1; } 
            menu_nextitem3(); 
            break; //addr.module_1..4
    case 6: iItemMenu2++; if( iItemMenu2 == 14) { iItemMenu2 = 1; } 
            menu_nextitem3(); 
            break; //addr.section_1..13
    case 7: break;
    case 8: break;
    case 9: break;   
  }
}

/*=======================================================================================*/
void menu_eeprom_write() {
  EEPROM.write( 11, iAdrG); delay( 5); //addr.group_1..16 
  EEPROM.write( 12, iAdrU); delay( 5); //addr.unit_1..16
  EEPROM.write( 13, iAdrM); delay( 5); //addr.module_1..4
  EEPROM.write( 14, iAdrS); delay( 5); //addr.section_1..13 
  EEPROM.write(  2, iArmS); delay( 5); //arm.soft_0/2
}

//----------
//void(* resetFunc) (void) = 0;
extern resetModule();

void menu_setcfg2() {
  menu_eeprom_write(); //zapis danych
  lcd.setCursor( 0, 0); lcd.print( F( " DATA RECORDING "));                 //line 1
  lcd.setCursor( 0, 1); lcd.print( F( "        3       ")); myDelay( 1000); //line 2
  lcd.setCursor( 0, 1); lcd.print( F( "        2       ")); myDelay( 1000); //line 2
  lcd.setCursor( 0, 1); lcd.print( F( "        1       ")); myDelay( 1000); //line 2
  lcd.clear();
  lcd.setCursor( 0, 1); lcd.print( F( "      RESET     ")); myDelay( 1000); //line 2
  resetModule(); //!!! soft reset !!!
}
