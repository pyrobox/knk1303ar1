/* KNK1303AR1 rev.1 | FirstStep Firing System | https://forumfajerwerki.pl
 * file: dev_ads1115.cpp | date: 2023.10.26
 */
/*==========1==========2==========3==========4==========5==========6==========7==========*/
/*
public:
void ads1115_setup();
void ads1115_pwrtoclear();
void ads1115_pwrtocmdpd();
private:
void ads1115_read();
*/
#include <Arduino.h>
#include "headerfile.hpp"
#include "unit_var.hpp"

#include "LiquidCrystal_PCF8574.h"
extern LiquidCrystal_PCF8574 lcd;

extern char hPwr[7]; //(set_info) {'0','0','0','0','0','0','\0'} //pwr1/pwr2 <- 256256

float fBatt1 = 0.0; //local var //pwr1_batt_inside
float fBatt2 = 0.0; //local var //pwr2_batt_outside
float fCalc = 6.0;  //local var //!!! dzielnik R1=15k,R2=3k -> V1*(R2/R1+R2)=30*3/18=5 -> 30/5=6 !!!

/*=======================================================================================*/
#include <ADS1115_WE.h> //https://github.com/wollewald/ADS1115_WE
#define ADS1115_ADDRESS 0x48

/* There are several ways to create your ADS1115_WE object: */
//ADS1115_WE adc = ADS1115_WE(); -> uses Wire / I2C Address = 0x48
//ADS1115_WE adc = ADS1115_WE( ADS1115_ADDRESS); -> uses Wire / ADS1115_ADDRESS
//ADS1115_WE adc = ADS1115_WE( &Wire); -> you can pass any TwoWire object / I2C Address = 0x48
//ADS1115_WE adc = ADS1115_WE( &Wire, ADS1115_ADDRESS); -> all together
ADS1115_WE adc = ADS1115_WE( ADS1115_ADDRESS);

/*---------------------------------------------------------------------------------------*/
void ads1115_setup() {
  //if( !adc.init()) { Serial.println("ADS1115 not connected!"); }

  /* Set the voltage range of the ADC to adjust the gain:
     !you must not apply more than VDD + 0.3V to the input pins! */
  //ADS1115_RANGE_6144  ->  +/- 6144 mV
  //ADS1115_RANGE_4096  ->  +/- 4096 mV
  //ADS1115_RANGE_2048  ->  +/- 2048 mV (default)
  //ADS1115_RANGE_1024  ->  +/- 1024 mV
  //ADS1115_RANGE_0512  ->  +/- 512 mV
  //ADS1115_RANGE_0256  ->  +/- 256 mV
  adc.setVoltageRange_mV( ADS1115_RANGE_6144); 

  /* Set the inputs to be compared: */
  //ADS1115_COMP_0_1    ->  compares 0 with 1 (default)
  //ADS1115_COMP_0_3    ->  compares 0 with 3
  //ADS1115_COMP_1_3    ->  compares 1 with 3
  //ADS1115_COMP_2_3    ->  compares 2 with 3
  //ADS1115_COMP_0_GND  ->  compares 0 with GND
  //ADS1115_COMP_1_GND  ->  compares 1 with GND
  //ADS1115_COMP_2_GND  ->  compares 2 with GND
  //ADS1115_COMP_3_GND  ->  compares 3 with GND
  //adc.setCompareChannels( ADS1115_COMP_0_GND); //uncomment if you want to change the default

  /* Set number of conversions after which the alert pin will assert:
     - or you can disable the alert */
  //ADS1115_ASSERT_AFTER_1  -> after 1 conversion
  //ADS1115_ASSERT_AFTER_2  -> after 2 conversions
  //ADS1115_ASSERT_AFTER_4  -> after 4 conversions
  //ADS1115_DISABLE_ALERT   -> disable comparator / alert pin (default) 
  //adc.setAlertPinMode( ADS1115_ASSERT_AFTER_1); //uncomment if you want to change the default

  /* Set the conversion rate in SPS (samples per second) Options should be self-explaining: */
  //ADS1115_8_SPS 
  //ADS1115_16_SPS  
  //ADS1115_32_SPS 
  //ADS1115_64_SPS  
  //ADS1115_128_SPS (default)
  //ADS1115_250_SPS 
  //ADS1115_475_SPS 
  //ADS1115_860_SPS 
  //adc.setConvRate( ADS1115_128_SPS); //uncomment if you want to change the default

  /* Set continuous or single shot mode: */
  //ADS1115_CONTINUOUS  ->  continuous mode
  //ADS1115_SINGLE      ->  single shot mode (default)
  //adc.setMeasureMode( ADS1115_CONTINUOUS); //uncomment if you want to change the default

  /* Choose maximum limit or maximum and minimum alert limit (window) in volts - alert pin will 
     assert when measured values are beyond the maximum limit or outside the window 
     Upper limit first: setAlertLimit_V( MODE, maximum, minimum)
     In max limit mode the minimum value is the limit where the alert pin assertion will be 
     be cleared (if not latched) */
  //ADS1115_MAX_LIMIT
  //ADS1115_WINDOW
  //adc.setAlertModeAndLimit_V( ADS1115_MAX_LIMIT, 3.0, 1.5); //uncomment if you want to change the default
  
  /* Enable or disable latch. If latch is enabled the alert pin will assert until the
     conversion register is read (getResult functions). If disabled the alert pin assertion
     will be cleared with next value within limits. */
  //ADS1115_LATCH_DISABLED (default)
  //ADS1115_LATCH_ENABLED
  //adc.setAlertLatch( ADS1115_LATCH_ENABLED); //uncomment if you want to change the default

  /* Sets the alert pin polarity if active: */
  //ADS1115_ACT_LOW  -> active low (default)   
  //ADS1115_ACT_HIGH -> active high
  //adc.setAlertPol( ADS1115_ACT_LOW); //uncomment if you want to change the default
 
  /* With this function the alert pin will assert, when a conversion is ready.
     In order to deactivate, use the setAlertLimit_V function */
  //adc.setAlertPinToConversionReady(); //uncomment if you want to change the default
}

/*---------------------------------------------------------------------------------------*/
/*
akumulator zelowy:
w trakcie ladowania = 14.8v
po odlaczeniu ladowania = 13.5-13.8v
naladowany w 100% = 12.8v
naladowany w 80% = 12.5v
naladowany w 20% = 11.0v
calkowicie rozladowany = 10.5v
calkowicie rozladowany aku z obnizonym pkt. krytycznym napiecia = 9.5v
trup < 9.5v
*/
void ads1115_read() {
  float voltage = 0.0;
  //--
  adc.setCompareChannels( ADS1115_COMP_0_GND);
  adc.startSingleMeasurement();
  while( adc.isBusy()) {}
  voltage = adc.getResult_V(); //alternative: getResult_mV for Millivolt
  fBatt1 = voltage * fCalc; 
  //--
  adc.setCompareChannels( ADS1115_COMP_1_GND);
  adc.startSingleMeasurement();
  while( adc.isBusy()) {}
  voltage = adc.getResult_V();
  fBatt2 = voltage * fCalc;
}

/*---------------------------------------------------------------------------------------*/
void ads1115_pwrtoclear() {
  ads1115_read(); //read pwr1(inside) & pwr2(outside)
  char buff1[10]; dtostrf( fBatt1, 4, 1, buff1); //4 is mininum width, 1 is precision
  char buff2[10]; dtostrf( fBatt2, 4, 1, buff2); 
  String sbatt = "BATT " + String( buff1) + "/" + String( buff2) + " V";
  lcd.setCursor( 0, 1); lcd.print( sbatt); //"BATT 25.6/25.6 V" //line_2
}

/*---------------------------------------------------------------------------------------*/
void ads1115_pwrtocmdpd() {
  ads1115_read(); //read pwr1(inside) & pwr2(outside)
  //przepisywanie wynikow do bufora hPwr
  char buff1[10]; dtostrf( fBatt1, 4, 1, buff1);
  if( buff1[0] == ' ') hPwr[0] = '0'; else hPwr[0] = buff1[0]; //2
  hPwr[1] = buff1[1];                                          //5
  hPwr[2] = buff1[3];                                          //6
  char buff2[10]; dtostrf( fBatt2, 4, 1, buff2); 
  if( buff2[0] == ' ') hPwr[3] = '0'; else hPwr[3] = buff2[0]; //2
  hPwr[4] = buff2[1];                                          //5
  hPwr[5] = buff2[3];                                          //6	
}


