/* KNK1303AR1 rev.1 | FirstStep Firing System | https://forumfajerwerki.pl
 * file: dev_e32ttl100.cpp | date: 2023.10.27 //!!! nie testowane
 */
/*==========1==========2==========3==========4==========5==========6==========7==========*/
/*
public:
void lora32_setup();
void lora32_loop();
*/
#include <Arduino.h>
#include "headerfile.hpp"
#include "unit_var.hpp"
/*
E32-TTL-100 -> nano every
1.M0  -> GND
2.M1  -> GND
3.TX  -> D7 (PullUP 4,7KΩ)
4.RX  -> D2 (PullUP 4,7KΩ & Voltage divider)
5.AUX -> not connected
6.VCC -> 3.3v
7.GND -> GND
*/
/*================================================================================#1=====*/
/*
#include <SoftwareSerial.h>
SoftwareSerial mySerial( 7, 8); //rxPin=7 txPin=8 ===> e32_TX e32_RX

void lora32_setup() {
  mySerial.begin( 9600);
}
*/ 
/*---------------------------------------------------------------------------------------*/
/*
void lora32_loop() {
  if( mySerial.available()) { 
    Serial.write( mySerial.read()); 
  }
  if( Serial.available()) { 
    mySerial.write( Serial.read()); 
  }
}
*/
/*================================================================================#2=====*/
//https://mischianti.org/lora-e32-device-for-arduino-esp32-or-esp8266-specs-and-basic-usage-part-1/

#include "LoRa_E32.h" //https://github.com/xreef/LoRa_E32_Series_Library

#include <SoftwareSerial.h>
SoftwareSerial mySerial( 7, 8); //rxPin=7 txPin=8 ===> e32_TX e32_RX
LoRa_E32 e32ttl100( &mySerial);

/*
select one:
#define E32_TTL_100
#define E32_TTL_500
#define E32_TTL_1W
select one:
#define FREQUENCY_433
#define FREQUENCY_170
#define FREQUENCY_470
#define FREQUENCY_868
#define FREQUENCY_915
*/
/* normal mode / transparent transmission: (arduino uno)
1.M0  -> GND (Set normal mode)
2.M1  -> GND (Set normal mode)
3.TX  -> PIN 2 (PullUP 4,7KΩ)
4.RX  -> PIN 3 (PullUP 4,7KΩ & Voltage divider)
5.AUX -> Not connected
6.VCC -> 3.3v
7.GND -> GND

For normal transmission, you must set M0 and M1 to LOW, 
and It’s better if you connect the AUX pin to have a better synchronization, but not needed. 
You can check the connection of the AUX pin in the library complete example connection, 
and you only must add the pin number to the constructor.

LoRa_E32 e32ttl100( 2, 3);    //TX, RX
LoRa_E32 e32ttl100( 2, 3, 5); //TX, RX, AUX
LoRa_E32 e32ttl100( 2, 3, 5, 7, 6); //with AUX and M0 M1
 
#include <SoftwareSerial.h>
SoftwareSerial mySerial( 2, 3);
LoRa_E32 e32ttl100( &mySerial, 5, 7, 6);
*/

void lora32_setup() {
  //startup all pins and UART
  e32ttl100.begin();
  //send message
  ResponseStatus rs = e32ttl100.sendMessage( "Hello, world?");
  //check if there is some problem of successfully send
  Serial.println( rs.getResponseDescription());
}
 
/*---------------------------------------------------------------------------------------*/
void lora32_loop() {
  //if something available
  if( e32ttl100.available()>1) {
    //read the String message
    ResponseContainer rc = e32ttl100.receiveMessage();
    //is something goes wrong print error
    if( rc.status.code!=1){
      rc.status.getResponseDescription();
    } else {
      //print the data received
      Serial.println( rc.data);
    }
  }
  if( Serial.available()) {
    String input = Serial.readString();
    e32ttl100.sendMessage( input);
  }
}

/*---------------------------------------------------------------------------------------*/
//if you have ever change configuration you must restore it
/*
  ResponseStructContainer c;
  c = e32ttl100.getConfiguration();
  Configuration configuration = *(Configuration*) c.data;
  Serial.println(c.status.getResponseDescription());
  configuration.CHAN = 0x17;
  configuration.OPTION.fixedTransmission = FT_TRANSPARENT_TRANSMISSION;
  e32ttl100.setConfiguration(configuration, WRITE_CFG_PWR_DWN_SAVE);
*/


/*================================================================================#3=====*/
//arduino uno -> e32ttl100 -> M0 M1 -> 3.3v
/*
LoRa_E32 e32ttl100( 2, 3);
void printParameters( struct Configuration configuration);
void printModuleInformation( struct ModuleInformation moduleInformation);
 
void setup() {
    Serial.begin(9600);
    delay(500);
 
    // Startup all pins and UART
    e32ttl100.begin();
 
    ResponseStructContainer c;
    c = e32ttl100.getConfiguration();
    // It's important get configuration pointer before all other operation
    Configuration configuration = *(Configuration*) c.data;
    Serial.println(c.status.getResponseDescription());
    Serial.println(c.status.code);
 
    printParameters(configuration);
    configuration.ADDL = 0x0;
    configuration.ADDH = 0x1;
    configuration.CHAN = 0x19;
 
    configuration.OPTION.fec = FEC_0_OFF;
    configuration.OPTION.fixedTransmission = FT_TRANSPARENT_TRANSMISSION;
    configuration.OPTION.ioDriveMode = IO_D_MODE_PUSH_PULLS_PULL_UPS;
    configuration.OPTION.transmissionPower = POWER_17;
    configuration.OPTION.wirelessWakeupTime = WAKE_UP_1250;
 
    configuration.SPED.airDataRate = AIR_DATA_RATE_011_48;
    configuration.SPED.uartBaudRate = UART_BPS_115200;
    configuration.SPED.uartParity = MODE_00_8N1;
 
    // Set configuration changed and set to not hold the configuration
    ResponseStatus rs = e32ttl100.setConfiguration(configuration, WRITE_CFG_PWR_DWN_LOSE);
    Serial.println(rs.getResponseDescription());
    Serial.println(rs.code);
    printParameters(configuration);
    c.close();
}
 
void loop() {}

void printParameters( struct Configuration configuration) {
    Serial.print(F("HEAD : "));  
	  Serial.print(configuration.HEAD, BIN); Serial.print(" ");
	  Serial.print(configuration.HEAD, DEC); Serial.print(" ");
	  Serial.println(configuration.HEAD, HEX);
    Serial.print(F("AddH : "));  Serial.println(configuration.ADDH, BIN);
    Serial.print(F("AddL : "));  Serial.println(configuration.ADDL, BIN);
    Serial.print(F("Chan : ")); 
      Serial.print(configuration.CHAN, DEC); Serial.print(" -> "); 
	  Serial.println(configuration.getChannelDescription());
    Serial.print(F("SpeedParityBit     : "));  
	  Serial.print(configuration.SPED.uartParity, BIN); Serial.print(" -> "); 
	  Serial.println(configuration.SPED.getUARTParityDescription());
    Serial.print(F("SpeedUARTDatte  : "));  
	  Serial.print(configuration.SPED.uartBaudRate, BIN); Serial.print(" -> "); 
	  Serial.println(configuration.SPED.getUARTBaudRate());
    Serial.print(F("SpeedAirDataRate   : "));  
	  Serial.print(configuration.SPED.airDataRate, BIN); Serial.print(" -> "); 
	  Serial.println(configuration.SPED.getAirDataRate());
    Serial.print(F("OptionTrans        : "));  
	  Serial.print(configuration.OPTION.fixedTransmission, BIN); Serial.print(" -> "); 
	  Serial.println(configuration.OPTION.getFixedTransmissionDescription());
    Serial.print(F("OptionPullup       : "));  
	  Serial.print(configuration.OPTION.ioDriveMode, BIN); Serial.print(" -> "); 
	  Serial.println(configuration.OPTION.getIODroveModeDescription());
    Serial.print(F("OptionWakeup       : "));  
	  Serial.print(configuration.OPTION.wirelessWakeupTime, BIN); Serial.print(" -> "); 
	  Serial.println(configuration.OPTION.getWirelessWakeUPTimeDescription());
    Serial.print(F("OptionFEC          : "));  
	  Serial.print(configuration.OPTION.fec, BIN); Serial.print(" -> "); 
	  Serial.println(configuration.OPTION.getFECDescription());
    Serial.print(F("OptionPower        : "));  
	  Serial.print(configuration.OPTION.transmissionPower, BIN); Serial.print(" -> "); 
	  Serial.println(configuration.OPTION.getTransmissionPowerDescription());
}
*/

