/* KNK1303AR1 rev.1 | FirstStep Firing System | https://forumfajerwerki.pl
 * file: out_rs485.cpp | date: 2023.10.27
 */
/*==========1==========2==========3==========4==========5==========6==========7==========*/
/*
public:
void rs485_print();
*/
#include <arduino.h>
#include "headerfile.hpp"
#include "unit_var.hpp"

char hSendBuffer[47]; //cmd_PD_answer={1<crc>4<PD>2<inf>4<pwr>6<ser>9<usr>9<add>6<ant>3<arm>1}1=46+1

/*=======================================================================================*/
void rs485_print() { 
  digitalWrite( pin_485dir, HIGH);
  Serial1.print( hSendBuffer);
  Serial1.flush(); //zaczekac na koniec transmisji danych
  digitalWrite( pin_485dir, LOW);
  //Serial.print( "TX:"); Serial.println( hSendBuffer); //!!! TEST !!! //usunac, takze w loop
}

/* copy string to buffer
String s = "{1E7FXM}"; 
s.toCharArray( hSendBuffer, s.length() + 1); 
*/
