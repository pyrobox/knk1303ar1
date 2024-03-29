/* KNK1303AR1 rev.1 | FirstStep Firing System | https://forumfajerwerki.pl
 * file: unit_var.hpp | date: 2023.11.22
 */
#ifndef unit_var_hpp
#define unit_var_hpp
 
#include <Arduino.h>

/*===========================================================================ALIASES=====*/
//declared ---> unit_var.hpp/cpp

//-----485
extern const int pin_485dir;
//-----int0
//extern const int pin_trigger;
//-----595
extern const int pin_74595data;
extern const int pin_74595latch;
extern const int pin_74595clock;
//-----in/out
extern const int pin_inTest;
//extern const int pin_inArm;
extern const int pin_outFire;
//-----switch
//extern const int pin_buttonMenu;
//extern const int pin_buttonNext;
//extern const int pin_buttonSet;

void pinSetup();

/*=======================================================================================*/
#endif /* unit_var_hpp */
