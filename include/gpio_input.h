#ifndef GPIO_INPUT_h
#define GPIO_INPUT_h

#include <Arduino.h>
#include <constData.h>
#include <lcd_menu.h>

void input_buttons(LiquidCrystal_I2C* lcd, userDataStruct *ptrUserData);

#endif