/* FirstStep Firing System | KNK1303AR1 rev.1
 * file: unit_service.cpp | date: 2023.10.27 //!!! OK !!!
 */
/*==========1==========2==========3==========4==========5==========6==========7==========*/
/*
public:
void service_i2cScanner();
void myDelay( int t);
int hex2dec( char a);
String TwoDigitToString( const int& x);
*/
#include <Arduino.h>
#include <Wire.h>
#include "headerfile.hpp"
#include "unit_var.hpp"

/*=======================================================================================*/
void service_i2cScanner() {
  byte error, address;
  int nDevices;
  Serial.println( "Scanning...");
  nDevices = 0;
  for( address = 1; address < 127; address++ )
  {
    Wire.beginTransmission( address);
    error = Wire.endTransmission();
    if( error == 0) {
      Serial.print( "I2C device found at address 0x");
      if ( address<16) Serial.print( "0");
      Serial.print( address, HEX);
      Serial.println( "  !");
      nDevices++;
    }
    else if( error == 4) {
      Serial.print( "Unknown error at address 0x");
      if( address<16) Serial.print( "0");
      Serial.println( address, HEX);
    }    
  }
  if( nDevices == 0) Serial.println( "No I2C devices found\n");
  else Serial.println( "done\n");
}

/*=========================================================================FUNCTION======*/
#include <avr/wdt.h>

void myDelay( int t) {
  for (int i = 0; i < t; i++) { delay(1); wdt_reset(); }
}

/*---------------------------------------------------------------------------------------*/
int hex2dec( char a) {
  int i = 0;
  if(a <= 57) { i = a - 48; } //'9'=57 -48=9
    else      { i = a - 55; } //'F'=70 -55=15
  return i;
}

/*---------------------------------------------------------------------------------------*/
String TwoDigitToString( const int& x) { // 00-99
  if( x == 0) { return "00"; }
  if( x < 10) { return "0" + String(x); }
  return String(x);
}
