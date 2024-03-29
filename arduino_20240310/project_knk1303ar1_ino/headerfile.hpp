/* KNK1303AR1 rev.1 | FirstStep Firing System | https://forumfajerwerki.pl
 * file: headerfile.hpp | date: 2023.11.12
 */
/*==========1==========2==========3==========4==========5==========6==========7==========*/
 
#ifndef headerfile_hpp
#define headerfile_hpp

#include <Arduino.h>

/*=======================================================================================*/ 

//==============================INO
//-----timers
//void time_LCDstart();
//void time_LCDstop();
//void time_XPstart();
//void time_XPstop();
//void time_YXstart();
//void time_YXstop();
//-----lcd
//void lcd_begin();
//void lcd_lightOff();
//void lcd_lightOn();
//-----led
//void led_begin();
//void led_show( byte i);
//void led_buzzer();
//-----
//void resetModule();

//==============================SETUP
void pinSetup();
//-----idno#
void idno_runPS();
void idno_runPX();
void idno_idnoRead(); //private
void idno_idnoWrite(); //private
//-----user#
void user_runPU();
void user_runPY();
void user_userRead(); //private
void user_userWrite(); //private
//-----addr#
void addr_runPA();
void addr_runPZ();
void addr_addrRead(); //private
void addr_addrWrite(); //private
//-----info#
void info_runPD();
//-----setup
void setup_readSetup();
void setup_default();
void setup_sethAdr(); //private
void setup_readAddr(); //private
void setup_readArm(); //private
void setup_sethInfo(); //private
//-----arm
void arm_Check();
void key_On();
void key_Off();

//==============================MENU
//-----menu1
void menu_buttonMenu();
void menu_buttonClear();
void menu_buttonLink();
void menu_lcdMenu1( int i); //private
void menu_menuIn(); //private
void menu_menuOut(); //private
void menu_clearLcd(); //private
void menu_linkTest(); //private
//-----menu2
void menu_nextitem();
void menu_nextitem2();
void menu_setcfg();
void menu_setcfg2();
void menu_LcdMenu2( int i); //private
void menu_setGrp(); //private
void menu_setUnt(); //private
void menu_setMod(); //private
void menu_setSec(); //private
void menu_reset(); //private
void menu_disarm(); //private
void menu_info(); //private
void menu_testStart(); //private
void menu_testShow(); //private
void menu_disarm2(); //private
void menu_LcdInfo1( int i); //private
void menu_info2_wait(); //private
void menu_info2(); //private
void menu_reset2(); //private
void menu_testStart2(); //private
void menu_testShow2(); //private
void menu_setGrp2(); //private
void menu_setUnt2(); //private
void menu_setMod2(); //private
void menu_setSec2(); //private
void menu_nextitem3(); //private
void menu_eeprom_write(); //private
//-----menu3
void menu_modTest();
void menu_modShow();
void menu_check0(); //private
void menu_check1(); //private
void menu_check2(); //private
void menu_check3(); //private
void menu_check4(); //private
boolean menu_cueTest(); //private
void menu_secData(); //private
void menu_secTest(); //private

//==============================CMD
void data_runCmd();
void data_monitor(); //private
//-----
void cmd_FC();
void cmdfc_setAddr();
void cmdfc_readPulse();
void cmdfc_fireUp();
//void cmdfc_readCueCmd(); //private
//void cmdfc_readCueEep(); //private
//void cmdfc_checkCue(); //private
void cmd_FO();
void cmd_FP();
//-----
//void cmd_FS();
//void cmd_FH();
//void cmd_FL();
//void cmd_FN();
//void cmd_FNfireNext( int ifn); //private
//-----
//void cmd_FT();
//void cmd_FTeepWrite();
//void cmd_FA();
//void cmd_FAeepWrite();
//-----
void cmd_MN();
void cmd_MF();
void cmd_MR();
//-----
void cmd_SC();
void cmd_SS();
void cmd_SM();
void cmd_TC();
boolean cmdtc_testCheck();
void cmd_TS();
void cmd_tsOneSec();
void cmd_tsData(); //private
void cmd_TM();
//-----
void cmd_XM();
void cmd_XP();
//-----
void cmd_XY();
void cmd_YX();

//==============================OUT
//-----knk1303ar1_relay:
void knk1303ar1_outputSet();
void knk1303ar1_outputReset();
void test_74595();
//-----serials
void usb_dataRead();
void usb_setG(); //private 
void usb_setU(); //private
void usb_setM(); //private
void usb_setS(); //private
void rs485_print();

//==============================DEVICE
//-----ads1115
void ads1115_setup();
void ads1115_pwrtoclear();
void ads1115_pwrtocmdpd();
void ads1115_read(); //private
//-----e32ttl100
void lora32_setup();
void lora32_loop();

//==============================OTHER
void service_i2cScanner();
void myDelay( int t);
int hex2dec( char a);
String TwoDigitToString( const int& x);

/*=======================================================================================*/
#endif /* headerfile_hpp */
