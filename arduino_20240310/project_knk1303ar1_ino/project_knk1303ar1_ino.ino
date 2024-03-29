/* KNK1303AR1 rev.1 | FirstStep Firing System | https://forumfajerwerki.pl
 * file: project_knk1303ar1_ino.ino | date: 2023.12.14
 *
 * Arduino IDE Version 2.3.2
 * board: "Arduino Nano Every" | registers emulation: "NONE (ATMEGA4809)"
 *
 * Copyright (c) 2024, by Zbigniew Kubicki (zk1959) https://pyrobox.com.pl/knk1301/
 * All rights reserved.
 * Redistribution and use in source and binary forms, with or without modification, 
 * are permitted provided that the following conditions are met:
 * * Redistributions of source code must retain the above copyright notice, this list 
 *   of conditions and the following disclaimer.
 * * Redistributions in binary form must reproduce the above copyright notice, this 
 *   list of conditions and the following disclaimer in the documentation and/or other 
 *   materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND 
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. 
 * IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, 
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
 * OF SUCH DAMAGE.  
 */

/*==========1==========2==========3==========4==========5==========6==========7==========*/
#include <Arduino.h>
#include <Wire.h>
#include <CRC16.h>
#include <EEPROM.h>
#include "headerfile.hpp" 
#include "unit_var.hpp" //pin declarations -> unit_var.hpp/cpp //pinout -> unit_var.cpp 

/*---------------------------------------------------------------------------------------*/
#include "LiquidCrystal_PCF8574.h" //https://github.com/mathertel/LiquidCrystal_PCF8574
LiquidCrystal_PCF8574 lcd( 0x27);  //addr=0x27 => i2c->HD44750 

/*---------------------------------------------------------------------------------------*/
#include "PCF8574.h" //https://github.com/xreef/PCF8574_library
PCF8574 led( 0x38);  //addr=0x20(..0x27) => PCF8574 | addr=0x38(..0x3F) => PCF8574A

/*---------------------------------------------------------------------------------------*/
#include <Toggle.h> //https://github.com/Dlloydev/Toggle/tree/main //-> buttons

/*---------------------------------------------------------------------------------------*/
#include <avr/wdt.h> //-> watchdog

/*---------------------------------------------------------------------------------------*/
//#include <ADS1115_WE.h> //https://github.com/wollewald/ADS1115_WE
//ADS1115_WE adc = ADS1115_WE( 0x48); //addr=0x48 => ADS1115 <- unit_ads1115.cpp

/*========== TIMERS =====================================================================*/
//zapalanie lcd na 10sec po nacisniecia przycisku

boolean bTimerLcd = false;
unsigned long iStartTimeLcd = 0; //local var

void time_LCDstart() { iStartTimeLcd = millis(); bTimerLcd = true; } //timerLCD start 
void time_LCDstop() { bTimerLcd = false; lcd.noDisplay(); lcd.setBacklight( 0); } //stop -> lcd off
//loop: if( bTimerLcd == true) { if( millis() - iStartTimeLcd >= 10000) time_LCDstop(); }

/*---------------------------------------------------------------------------------------*/
//usuniecie z lcd znaku heartbeat po 500msec od wyswietlenia

boolean bTimerXP = false;
unsigned long iStartTimeXP = 0; //local var

void led_show( byte i);

void time_XPstart() { iStartTimeXP = millis(); bTimerXP = true; } //timerXP start 
void time_XPstop() { bTimerXP = false; lcd.setCursor( 15, 1);  lcd.print( " "); //stop -> heartbeat char clear
                     int n = EEPROM.read( 3); led_show( n); } //n = iArm=iArmH+iArmS //led_blue_off
//loop: if( bTimerXP == true) { if( millis() - iStartTimeXP >= 500) time_XPstop(); }
//cmd_XP(): led_show( 4); //led_blue_on

/*---------------------------------------------------------------------------------------*/
//wyswietlenie na lcd info link.err po 500msec bez odpowiedzi master

boolean bTimerYX = false;
unsigned long iStartTimeYX = 0; //local var

void time_YXstart() { iStartTimeYX = millis(); bTimerYX = true; } //timerYX start
void time_YXstop() { bTimerYX = false; lcd.setCursor( 6, 1); lcd.print( F( "LINK ERROR")); } //stop -> link.err
//loop: if( bTimerYX == true) { if( millis() - iStartTimeYX >= 500) time_YXstop(); } 

/*========== LCD_I2C ====================================================================*/
byte heartbeat[] = { 0b11111, 0b10001, 0b10101, 0b10101, 0b10101, 0b10101, 0b10001, 0b11111 }; //heartbeat
extern char hAdr[11]; //(set_setup.cpp)

void lcd_begin() {
  lcd.begin( 16, 2);
  lcd.setBacklight( 64); //max 255
  lcd.createChar(1, heartbeat); //znak specjalny hb
  lcd.home();
  lcd.noCursor(); 
  lcd.clear();
  lcd.setCursor( 0, 0); lcd.print( F("ADDR:           ")); //"ADDR:           " //line_1
  lcd.setCursor( 6, 0); lcd.print( String(hAdr));          //      "16.16/4.13" //line_1
  //lcd_lightOff(); //wlacza wygaszanie lcd
}

void lcd_lightOff() { time_LCDstart(); } //wygaszenie lcd po 10sec
void lcd_lightOn()  { time_LCDstart(); lcd.display(); lcd.setBacklight( 64); } //podswietlenie lcd na 10sec

/*========== LED_I2C ====================================================================*/
void led_begin() {
  led.pinMode(P4, OUTPUT, LOW); //led_blue
	led.pinMode(P5, OUTPUT, LOW); //led_green
	led.pinMode(P6, OUTPUT, LOW); //led_red
	led.pinMode(P7, OUTPUT, LOW); //buzzer
  led.begin();
}

void led_show( byte i) { //0..3 plus 4=blue
  switch ( i) {        //led:red                         green                       blue
    case 0: led.digitalWrite(P6, LOW);  led.digitalWrite(P5, HIGH); led.digitalWrite(P4, LOW);  break; //green  //arm.safe
    case 1: led.digitalWrite(P6, HIGH); led.digitalWrite(P5, HIGH); led.digitalWrite(P4, LOW);  break; //yellow //arm.hard_on
    case 2: led.digitalWrite(P6, HIGH); led.digitalWrite(P5, HIGH); led.digitalWrite(P4, LOW);  break; //yellow //arm.soft_on
    case 3: led.digitalWrite(P6, HIGH); led.digitalWrite(P5, LOW);  led.digitalWrite(P4, LOW);  break; //red    //arm.armed
    case 4: led.digitalWrite(P6, LOW);  led.digitalWrite(P5, LOW);  led.digitalWrite(P4, HIGH); break; //blue   //heartbeat
   }
}

void led_buzzer() { led.digitalWrite(P7, HIGH); myDelay( 1000); led.digitalWrite(P7, LOW); } //nie uzywane

/*========== BUTTONS ====================================================================*/
const byte pin_buttonMenu  = A3; //17
const byte pin_buttonClear = A6; //20
const byte pin_buttonLink  = A7; //21
const byte pin_inArm       = A1; //15

Toggle sw1( pin_buttonMenu);  //button_MENU
Toggle sw2( pin_buttonClear); //button_NEXT/CLEAR
Toggle sw3( pin_buttonLink);  //button_SET/LINK
Toggle sw4( pin_inArm);       //key_ARM


/*****************************************************************************************/
/*========== SETUP_START                                                       ==========*/
/*****************************************************************************************/

void(* resetFunc) (void) = 0; //soft reset
void resetModule() { resetFunc(); } //przekazanie do cpp

/*---------------------------------------------------------------------------------------*/
void setup() { //!!! nie zmieniac kolejnosci wywolan funkcji
  //----------CONFIG
  pinSetup(); //pins alliances
  setup_readSetup(); //read/set: idno+user+addr+info+arm
  //----------SERIALS
  Serial. begin( 9600); //usb_terminal
  Serial1.begin( 9600); //rs485
  Wire.begin(); delay( 5); //i2c_100kHz 
  //----------INTERFACE
  lcd_begin(); //lcd
  led_begin(); //led
  //--buttons 
  sw1.begin( pin_buttonMenu);
  sw2.begin( pin_buttonClear);
  sw3.begin( pin_buttonLink);
  sw4.begin( pin_inArm);
  //----------OTHERS
  knk1303ar1_outputReset(); //74595 reset
  ads1115_setup(); //adc_pwr1/pwr2
  //lora32_setup();
  //----------WATCHDOG
  wdt_enable(WDT_PERIOD_512CLK_gc);
}
/*
typedef enum WDT_PERIOD_enum {
WDT_PERIOD_OFF_gc    = (0x00<<0), / Off /
WDT_PERIOD_8CLK_gc   = (0x01<<0), / 8 cycles (8ms) /
WDT_PERIOD_16CLK_gc  = (0x02<<0), / 16 cycles (16ms) /
WDT_PERIOD_32CLK_gc  = (0x03<<0), / 32 cycles (32ms) /
WDT_PERIOD_64CLK_gc  = (0x04<<0), / 64 cycles (64ms) /
WDT_PERIOD_128CLK_gc = (0x05<<0), / 128 cycles (0.128s) /
WDT_PERIOD_256CLK_gc = (0x06<<0), / 256 cycles (0.256s) /
WDT_PERIOD_512CLK_gc = (0x07<<0), / 512 cycles (0.512s) /
WDT_PERIOD_1KCLK_gc  = (0x08<<0), / 1K cycles (1.0s) /
WDT_PERIOD_2KCLK_gc  = (0x09<<0), / 2K cycles (2.0s) /
WDT_PERIOD_4KCLK_gc  = (0x0A<<0), / 4K cycles (4.1s) /
WDT_PERIOD_8KCLK_gc  = (0x0B<<0), / 8K cycles (8.2s) /
} WDT_PERIOD_t;
*/

/*****************************************************************************************/
/*========== SETUP_STOP                                                        ==========*/
/*****************************************************************************************/


/*****************************************************************************************/
/*========== LOOP_START                                                        ==========*/
/*****************************************************************************************/

boolean comenabled = true; //rs485 flag true=open/false=close
char hDataBuffer[47]; //rs485 buffer <= cmd_PD_answer={1<crc>4<PD>2<inf>4<pwr>6<ser>9<usr>9<add>6<ant>3<arm>1}1=46+1
char hUsbBuffer[4] = {'G','0','1','\0'}; //usb_terminal buffer

//extern char hCmdFTbuffer[8];  
//-> cmd_FT //{'&','0','0','0','0','2','&','\0'} 
//extern char hCmdFAbuffer[24]; 
//-> cmd_FA //{'$','5','0','0','1','0','B','4','F','H','0','0','0','3','3','3','0','5','1','0','0','1','$','\0'}
//extern char hCmdSAMbuffer[26]; //cmdFH=20-2{}+3@,@+5time

/*---------------------------------------------------------------------------------------*/
void loop() {
  //----------TIMERS
  if( bTimerXP  == true) { if( millis() - iStartTimeXP  >=   500) time_XPstop(); }  //cmd_XP
  if( bTimerYX  == true) { if( millis() - iStartTimeYX  >=   500) time_YXstop(); }  //cmd_YX
  if( bTimerLcd == true) { if( millis() - iStartTimeLcd >= 10000) time_LCDstop(); } //lcd_on/off

  //----------BUTTONS //!!! odblokowac lcd_lightOn() !!!
  sw1.poll(); if( sw1.onPress()) { /*lcd_lightOn();*/ menu_buttonMenu(); }  //wlacza lcd przy aktywnym wygaszaniu
  sw2.poll(); if( sw2.onPress()) { /*lcd_lightOn();*/ menu_buttonClear(); }
  sw3.poll(); if( sw3.onPress()) { /*lcd_lightOn();*/ menu_buttonLink(); }
  sw4.poll(); if( sw4.isPressed()) { key_On(); }  //key_arm_on
                else               { key_Off(); } //key_arm_off 

  //----------RS485
  if( comenabled == true) { //mozna korzystac z rs485, port dostepny
    if( Serial1.available()) { 
      char z = Serial1.read(); //pobieramy pierwszy znak 
      //----------CMD_
      if( z == '{') { //sprawdzamy czy to { ('&'=sdf '$'=asu '@'=sam)
        String sCmd = Serial1.readStringUntil( '}'); //pobieramy reszte do }
        comenabled = false; //zamykamy port
        //--bufor roboczy hDataBuffer
        sCmd = '{' + sCmd + '}'; //dodajemy terminator { bo zostal pominiety oraz } bo readStringUntil go obcial
        int iCmd = sCmd.length() + 1;
        sCmd.toCharArray( hDataBuffer, iCmd); //przenosimy cmd do bufora roboczego
        //Serial.print( "RX:"); Serial.println( hDataBuffer); //<=== !!! TEST !!! //usunac, takze w rs485_print()
        //--kalkulacja crc
        char hCrc[5]; //bufor na crc z cmd
        for( int i = 0; i <= 3; i++) { hCrc[i] = hDataBuffer[i + 1]; } //pobieramy crc
        char hTmp[iCmd - 6]; //tworzymy bufor tmp do kalkulacji crc, -6 bo {1 + <crc>4 + }1 
        for( int i = 0; i < (sizeof(hTmp) / sizeof(hTmp[0]) - 1); i++) { hTmp[i] = hDataBuffer[i + 5]; } //pobieramy dane bez crc
        CRC16 tmpcrc;
        tmpcrc.setPolynome( 0x1021); //CRC-CCITT
        for( int i = 0; i < (sizeof(hTmp) / sizeof(hTmp[0]) - 1); i++) { tmpcrc.add( hTmp[i]); } //wczytujemy dane do tmpcrc
        uint16_t itmpcrc = tmpcrc.getCRC(); //kalkulujemy crc
        char htmpcrc[5]; sprintf( htmpcrc, "%04X", itmpcrc); //przeksztalacamy crc int na crc hex
        //--porownanie crc
        if((htmpcrc[0] == hCrc[0]) && (htmpcrc[1] == hCrc[1]) && (htmpcrc[2] == hCrc[2]) && (htmpcrc[3] == hCrc[3])) { 
          data_runCmd(); //przekazanie cmd do analizy
        }
        comenabled = true; //udostepniamy port
      } //end if z={
    
      //----------CMD_FT__SDF.sft
      else if( z == '&') {
        String sSdf = Serial1.readStringUntil( '&'); 
        comenabled = false; 
        sSdf = '&' + sSdf + '&';
        //sSdf.toCharArray( hCmdFTbuffer, sSdf.length() + 1);
        //Serial.print( "RX:"); Serial.println( hCmdFTbuffer); //<=== !!! TEST !!!
        //cmd_FTeepWrite();
        comenabled = true;
      }

      //----------CMD_FA__ASU.sfa   
      else if( z == '$') {
        String sAsu = Serial1.readStringUntil( '$'); 
        comenabled = false; 
        sAsu = '$' + sAsu + '$'; 
        //sAsu.toCharArray( hCmdFAbuffer, sAsu.length() + 1); 
        //Serial.print( "RX:"); Serial.println( hCmdFAbuffer); //<=== !!! TEST !!!
        //cmd_FAeepWrite();
        comenabled = true;
      }

      //----------CMD_SAM__SAM.sam  
      else if( z == '@') {
        String sSam = Serial1.readStringUntil( '@'); 
        comenabled = false; 
        sSam = '@' + sSam + '@'; 
        //sAsu.toCharArray( hCmdSAMbuffer, sSam.length() + 1); 
        //Serial.print( "RX:"); Serial.println( hCmdSAMbuffer); //<=== !!! TEST !!!
        //cmd_SAMeepWrite();
        comenabled = true;
      }

      //----------LoRa_E32TTL100
      //lora32_loop();

    } //end if Serial1.available
  } //end if comenabled=true

  //----------USB_TERMINAL
  if( Serial.available()) {
    char u = Serial.read(); 
    //-----PRESET
    if(  u == 'q') { setup_default(); } //factory preset
    if( (u == 'G') || (u == 'U') || (u == 'M') || (u == 'S')) { //grp/unt/mod/sec preset <- G01..16
      hUsbBuffer[0] = u; 
      int n = 1;
      while( Serial.available()) { u = Serial.read(); hUsbBuffer[n] = u; n++; }
      hUsbBuffer[3] = '\0';
      usb_dataRead();
      Serial.println( hUsbBuffer); //usb_terminal
    }
    //-----TEST
    if(  u == 'i') service_i2cScanner(); //i2c device scanner
   }
  
  //----------LOOP_END
  wdt_reset(); //watchdog reset
  delay( 1);   //delay 1msec
}

/*****************************************************************************************/
/*========== LOOP_STOP                                                         ==========*/
/*****************************************************************************************/
