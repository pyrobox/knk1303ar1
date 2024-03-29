
/* KNK1303AR1 rev.1 | FirstStep Firing System | https://forumfajerwerki.pl
 * file: cmd_data.cpp | date: 2023.10.26
 */
/*==========1==========2==========3==========4==========5==========6==========7==========*/
/*
public:
void data_runCmd();
private:
void data_monitor();
*/
#include <Arduino.h>
#include <EEPROM.h>
#include <avr/wdt.h>
#include "headerfile.hpp"
#include "unit_var.hpp"

#include "LiquidCrystal_PCF8574.h"
extern LiquidCrystal_PCF8574 lcd;

extern boolean comenabled; //(ino)
extern char hDataBuffer[47]; //(ino)
extern boolean bXmStart; //(cmd_xm) cmd_XM start flag 
extern int iXmNext; //(cmd_xm) cmd counter

/*=======================================================================================*/
/* cmd_XM -> {1E7FXM} <- https://crccalc.com/
   Algorithm       ASCII  Result  Check   Poly    Init    RefIn  RefOut  XorOut
   CRC-16/XMODEM   XM     0x1E7F  0x31C3  0x1021  0x0000  false  false   0x0000
*/
//boolean data_checkCmd() {} //crc jest weryfikowane w loop

/*=======================================================================================*/
void data_monitor() { //zlicza odebrane cmd 
  if( bXmStart == true) { //jesli monitor jest wlaczony
    iXmNext++;            //to zwiekszamy licznik odebranych cmd
    lcd.setCursor( 11, 1); lcd.print( iXmNext); //i pokazujemy jego stan na lcd
  }
}

/*---------------------------------------------------------------------------------------*/
void data_runCmd() { //kieruje cmd do procedur wukonujacych zadania
  data_monitor(); 
  //-----ALL
  char s[3]; s[2] = '\0'; //tmp array 2+1
  for( int i = 0; i <= 1; i++) { s[i] = hDataBuffer[i + 5]; } //read cmd id
  if( strcmp( s, "PS") == 0) { idno_runPS(); goto data_out; } //cmd_PS/IDNO_write/tx_check/SERVICE_mode_only
  if( strcmp( s, "PX") == 0) { idno_runPX(); goto data_out; } //cmd_PX/IDNO_read/tx_answer/{D542PX}
  if( strcmp( s, "PU") == 0) { user_runPU(); goto data_out; } //cmd_PU/USER_write/tx_check
  if( strcmp( s, "PY") == 0) { user_runPY(); goto data_out; } //cmd_PY/USER_read/tx_answer      
  if( strcmp( s, "PA") == 0) { addr_runPA(); goto data_out; } //cmd_PA/ADDR_write/tx_check
  if( strcmp( s, "PZ") == 0) { addr_runPZ(); goto data_out; } //cmd_PZ/ADDR_read/tx_answer      
  if( strcmp( s, "PD") == 0) { info_runPD(); goto data_out; } //cmd_PD/DATA_read/tx_answer      
  //--
  if( strcmp( s, "XM") == 0) { cmd_XM(); goto data_out; } //cmd_XM/start/{1E7FXM}
  if( strcmp( s, "XP") == 0) { cmd_XP(); goto data_out; } //cmd_XP/heartbeat/{DDE3XP}
  if( strcmp( s, "MN") == 0) { cmd_MN(); goto data_out; } //cmd_MN/arm_on/{D29AMN}
  if( strcmp( s, "MF") == 0) { cmd_MF(); goto data_out; } //cmd_MF/arm_off/{5392MF}
  if( strcmp( s, "MR") == 0) { cmd_MR(); goto data_out; } //cmd_MR/reset/{0127MR}
  //if( strcmp( s, "FA") == 0) { cmd_FA(); goto data_out; } //cmd_FA/all_unit

  wdt_reset();
  //-----GROUP
  char c = hDataBuffer[8];   //cmd_addr.grp_hex=0..F
  int n = hex2dec( c) + 1;   //cmd_addr.grp_int=1..16
  byte a = EEPROM.read( 11); //eep_addr.grp_int=1..16
  if( n == a) { //cmd_addr.grp=ok
    if( strcmp( s, "FC") == 0) { cmd_FC(); goto data_out; } //cmd_FC/fire_cue
    if( strcmp( s, "FO") == 0) { cmd_FO(); goto data_out; } //cmd_FO/one_in_group
    if( strcmp( s, "FP") == 0) { cmd_FP(); goto data_out; } //cmd_FP/fire_pulse
    //if( strcmp( s, "FN") == 0) { cmd_FN(); goto data_out; } //cmd_FN/zero_delay_fire
    //if( strcmp( s, "FS") == 0) { cmd_FS(); goto data_out; } //cmd_FS/regular_sequencer
    //if( strcmp( s, "FH") == 0) { cmd_FH(); goto data_out; } //cmd_FH/speed_up_sequencer
    //if( strcmp( s, "FL") == 0) { cmd_FL(); goto data_out; } //cmd_FL/speed_down_sequencer
    //if( strcmp( s, "FT") == 0) { cmd_FT(); goto data_out; } //cmd_FT/programming_sequencer

    wdt_reset();
    //-----GROUP+UNIT
    c = hDataBuffer[7];   //cmd_addr.unt_hex=0..F
    n = hex2dec( c) + 1;  //cmd_addr.unt_int=1..16
    a = EEPROM.read( 12); //eep_addr.unt_int=1..16
    if( n == a) { //cmd_addr.unt=ok
      if( strcmp( s, "YX") == 0) { cmd_YX(); goto data_out; } //cmd_YX/link_slave_master/rx_check
      if( strcmp( s, "XY") == 0) { cmd_XY(); goto data_out; } //cmd_XY/link_master_slave/tx_check
      if( strcmp( s, "TC") == 0) { cmd_TC(); goto data_out; } //cmd_TC/test_cue
      if( strcmp( s, "TS") == 0) { cmd_TS(); goto data_out; } //cmd_TS/test_section
      if( strcmp( s, "TM") == 0) { cmd_TM(); goto data_out; } //cmd_TM/test_module
      if( strcmp( s, "SC") == 0) { cmd_SC(); goto data_out; } //cmd_SC/tx_answer
      if( strcmp( s, "SS") == 0) { cmd_SS(); goto data_out; } //cmd_SS/tx_answer
      if( strcmp( s, "SM") == 0) { cmd_SM(); goto data_out; } //cmd_SM/tx_answer
     } //if addr.unt
  } //if addr.grp

  data_out:
  comenabled = true;
}
