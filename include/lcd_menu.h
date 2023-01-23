#ifndef LCD_MENU_h
#define LCD_MENU_h

#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include <constData.h>

void lcd_init(LiquidCrystal_I2C *lcd, userDataStruct *ptrUserData);

String lcd_color_string(uint8_t key);

void lcd_menu(LiquidCrystal_I2C *lcd, uint8_t key, userDataStruct *ptrUserData);

uint8_t lcd_update(LiquidCrystal_I2C *lcd, userDataStruct *ptrUserData);

#endif